/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Mechanical.h
 * Created: Friday, 23rd October 2020 7:00:48 pm
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "BeltSlot.h"
#include "Detail.h"
#include "Fingerprint.h"
#include "ApparatusRuntime.h"

#include "Mechanical.generated.h"


// Forward declarations:
class AMechanism;
class UMachine;
template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
struct TChain;
template < typename SubjectHandleT >
struct TChunkIt;
template < typename SubjectHandleT >
struct TBeltIt;
struct FSubjectHandle;
struct FSolidSubjectHandle;

/**
 * @internal Exists only for reflection. Do not use.
 */
UINTERFACE(MinimalAPI, BlueprintType)
class UMechanical : public UInterface
{
	GENERATED_BODY()
};

/**
 * A common interface for all mechanisms.
 */
class APPARATUSRUNTIME_API IMechanical
{
	GENERATED_BODY()

	friend class UMachine;

  public:

	/**
	 * Default steady delta time value for descendants.
	 */
#define IMechanical_DefaultSteadyDeltaTime (1.0f / 30.0f)

  protected:

	/**
	 * The steady frame we are currently part of.
	 */
	int64 SteadyFrame = 0;

	/**
	 * The last, actually processed steady frame. -1 if the steady tick was not run yet.
	 */
	int64 ProcessedSteadyFrame = -1;

	/**
	 * The game time of the previous tick.
	 */
	float PrevTime = NAN;

	/**
	 * Is this mechanism currently updating?
	 */
	uint32 bInsideTick : 1;

	/**
	 * Is this mechanism currently in the process of steady ticking?
	 */
	uint32 bInsideSteadyTick : 1;

	/**
	 * Is this mechanism currently in the process of presentation ticking?
	 */
	uint32 bInsidePresentationTick : 1;

	/**
	 * Perform a standard ticking routine.
	 * 
	 * The function can be used in the descendants to perform
	 * a standard ticking routine.
	 */
	virtual void DoTick(float NewTime,
						float DeltaTime,
						float SteadyDeltaTime);

	void
	DoRegister();

	void
	DoUnregister();

	void
	DoPostLogin(APlayerController* NewPlayer);

	FORCEINLINE float
	DoGetProcessedSteadyTime(float SteadyDeltaTime) const
	{
		if (ProcessedSteadyFrame == -1)
			return NAN;
		return ProcessedSteadyFrame * SteadyDeltaTime;
	}

	FORCEINLINE float
	DoCalcSteadyFrameRatio(const float Time,
						   const float SteadyDeltaTime) const
	{
		if (!bInsidePresentationTick)
		{
			UE_LOG(LogApparatus, Error, TEXT("Steady Frame Ratio is only available during a presentation tick handling. "
										     "Are you using it within Steady ticking?"));
			return 0;
		}
		check(ProcessedSteadyFrame != -1);
		const float ProcessedSteadyTime = ProcessedSteadyFrame * SteadyDeltaTime;
		if (Time >= ProcessedSteadyTime + SteadyDeltaTime)
			return 1;
		if (Time <= ProcessedSteadyTime)
			return 0;
		return (Time - ProcessedSteadyTime) / SteadyDeltaTime;
	}

	FORCEINLINE float
	DoCalcSteadyFutureFactor(const float Time,
							 const float SteadyDeltaTime) const
	{
		if (!bInsidePresentationTick)
		{
			UE_LOG(LogApparatus, Error, TEXT("Steady Future Factor is only available during a presentation tick handling. "
											 "Are you using it within Steady ticking?"));
			return 0;
		}
		if (!FMath::IsFinite(PrevTime))
		{
			return 0;
		}
		const float FutureSteadyTime = (ProcessedSteadyFrame + 1) * SteadyDeltaTime;
		if (Time >= FutureSteadyTime)
			return 1;
		if (Time <= PrevTime)
			return 0;
		return (Time - PrevTime) / (FutureSteadyTime - PrevTime);
	}

  public:

	/**
	 * Get the mechanism this mechanical is part of.
	 */
	AMechanism*
	GetMechanism() const;
	
	/**
	 * Spawn a new subject, returning its handle.
	 * 
	 * @param Flagmark The initial flagmark of the subject to spawn with.
	 */
	FSubjectHandle
	SpawnSubject(const EFlagmark Flagmark = FM_None);

	/**
	 * Enchain iterables using the supplied filter.
	 * 
	 * @param InFilter A filter to enchain with.
	 * @return A pointer to a chain.
	 */
	template < typename ChainT = TChain<TChunkIt<FSubjectHandle>, TBeltIt<FSubjectHandle>> >
	TSharedRef<ChainT>
	Enchain(const FFilter& InFilter) const;

	/**
	 * Solid-enchain iterables using the supplied filter.
	 * 
	 * @param InFilter A filter to enchain with.
	 * @return A pointer to a chain.
	 */
	TSharedRef<TChain<TChunkIt<FSolidSubjectHandle>, TBeltIt<FSolidSubjectHandle>>>
	EnchainSolid(const FFilter& InFilter) const;

	/**
	 * Process newly created subjects.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,
			  Meta = (DisplayName = "Boot"),
			  Category = "Apparatus|Mechanism")
	void ReceiveBoot();

	/**
	 * Process a pre-steady input tick.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,
			  Meta = (DisplayName = "Input Tick"),
			  Category = "Apparatus|Mechanism")
	void ReceiveInputTick();

	/**
	 * Process a fixed-rate steady tick.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,
			  Meta = (DisplayName = "Steady Tick"),
			  Category = "Apparatus|Mechanism")
	void ReceiveSteadyTick(float DeltaSeconds);

	/**
	 * Process a past-steady presentation tick.
	 * 
	 * @param DeltaSeconds The fixed delta time in seconds.
	 * @param FrameRatio The steady frame ratio.
	 * @param FutureFactor The steady future factor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,
			  Meta = (DisplayName = "Presentation Tick"),
			  Category = "Apparatus|Mechanism")
	void ReceivePresentationTick(float DeltaSeconds, float FrameRatio, float FutureFactor);

	/**
	 * Process newly created subjects.
	 */
	virtual FORCEINLINE void
	Boot()
	{
		Execute_ReceiveBoot(this->_getUObject());
	}

	/**
	 * Process a pre-steady input tick.
	 */
	virtual FORCEINLINE void
	InputTick()
	{
		Execute_ReceiveInputTick(this->_getUObject());
	}

	/**
	 * Process a fixed-rate steady tick.
	 */
	virtual FORCEINLINE void
	SteadyTick(float DeltaTime)
	{
		Execute_ReceiveSteadyTick(this->_getUObject(), DeltaTime);
	}

	/**
	 * Process a past-steady presentation tick.
	 * 
	 * @param DeltaSeconds The fixed delta time in seconds.
	 * @param FrameRatio The steady frame ratio.
	 * @param FutureFactor The steady future factor.
	 */
	virtual FORCEINLINE void 
	PresentationTick(float DeltaSeconds, float FrameRatio, float FutureFactor)
	{
		Execute_ReceivePresentationTick(this->_getUObject(), DeltaSeconds, FrameRatio, FutureFactor);
	}

	FORCEINLINE
	IMechanical()
	{
		bInsideTick             = false;
		bInsideSteadyTick       = false;
		bInsidePresentationTick = false;
	}

	/**
	 * Get the time interval for steady ticking.
	 * 
	 * Should be overriden in the descendants to
	 * provide a propertie's value.
	 */
	virtual float
	GetSteadyDeltaTime() const
	{
		unimplemented();
		return NAN;
	}

	/**
	 * Get own Mechanical's local life time.
	 * 
	 * Should be overriden in the descendants.
	 */
	virtual float
	GetOwnTime() const
	{
		unimplemented();
		return NAN;
	}

	/**
	 * Get the time of the last processed steady frame.
	 */
	UFUNCTION(BlueprintCallable,
			  BlueprintNativeEvent,
			  Category="Apparatus|Mechanical",
			  Meta = (DisplayName = "Processed Steady Time"))
	FORCEINLINE float
	GetProcessedSteadyTime() const;

	/**
	 * The current ratio within the steady frame.
	 * 
	 * This is in relation between the previous steady
	 * frame and the current next one.
	 * Should be used for interframe interpolation.
	 */
	UFUNCTION(BlueprintCallable,
			  BlueprintNativeEvent,
			  Category="Apparatus|Mechanical",
			  Meta = (DisplayName = "Steady Frame Ratio"))
	FORCEINLINE float
	CalcSteadyFrameRatio() const;

	/**
	 * Get the current steady frame.
	 */
	UFUNCTION(BlueprintCallable,
			  BlueprintNativeEvent,
			  Category="Apparatus|Mechanical",
			  Meta = (DisplayName = "Steady Frame"))
	void
	GetSteadyFrame(int64& OutFrame) const;

	/**
	 * Get the total steady time elapsed.
	 */
	UFUNCTION(BlueprintCallable,
			  BlueprintNativeEvent,
			  Category="Apparatus|Mechanical",
			  Meta = (DisplayName = "Steady Time"))
	FORCEINLINE float
	GetSteadyTime() const;

	/**
	 * The current steady future factor.
	 * 
	 * This is in relation between the previous change time
	 * delta to the next steady frame change delta time.
	 */
	UFUNCTION(BlueprintCallable,
			  BlueprintNativeEvent,
			  Category="Apparatus|Mechanical",
			  Meta = (DisplayName = "Steady Future Factor"))
	FORCEINLINE float
	CalcSteadyFutureFactor() const;

#pragma region Operating
	/// @name Operating
	/// @{

	/**
	 * Process the mechanism using a functor mechanic.
	 * Supports lambdas. Constant mechanic version.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor argument types by default.
	 * @tparam FilterT The type of filter that takes place.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Filter The filter to query with.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename FilterT = void, typename MechanicT = void >
	typename std::enable_if<!std::is_function<MechanicT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
	Operate(FilterT&& Filter, const MechanicT& Mechanic);

	/**
	 * Process the mechanism using a functor mechanic.
	 * Supports lambdas. Mutable mechanic version.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor argument types by default.
	 * @tparam FilterT The type of filter that takes place.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Filter The filter to query with.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename FilterT = void, typename MechanicT = void >
	typename std::enable_if<!std::is_function<MechanicT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
	Operate(FilterT&& Filter, MechanicT& Mechanic);

	/**
	 * Process the chain using a free function mechanic.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on function's argument types by default.
	 * @tparam FilterT The type of filter that takes place.
	 * @tparam FunctionT The type of the mechanic function
	 * to operate on the slots.
	 * @param Filter The filter to query with.
	 * @param Mechanic The mechanical function to operate with.
	 */
	template < typename ChainT = void, typename FilterT = void, typename FunctionT = void >
	typename std::enable_if<std::is_function<FunctionT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
	Operate(FilterT&& Filter, FunctionT* const Mechanic);

	/**
	 * Process the mechanism using a functor mechanic.
	 * Supports lambdas. Constant mechanic auto-filter version.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor argument types by default.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename MechanicT = void >
	typename std::enable_if<!std::is_function<MechanicT>::value, EApparatusStatus>::type
	Operate(const MechanicT& Mechanic);

	/**
	 * Process the mechanism using a functor mechanic.
	 * Supports lambdas. Mutable mechanic auto-filter version.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor argument types by default.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename MechanicT = void >
	typename std::enable_if<!std::is_function<MechanicT>::value, EApparatusStatus>::type
	Operate(MechanicT& Mechanic);

	/**
	 * Process the chain using a free function mechanic.
	 * Auto-filter version.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on function's argument types by default.
	 * @tparam FunctionT The type of the mechanic function
	 * to operate on the slots.
	 * @param Mechanic The mechanical function to operate with.
	 */
	template < typename ChainT = void, typename FunctionT = void >
	typename std::enable_if<std::is_function<FunctionT>::value, EApparatusStatus>::type
	Operate(FunctionT* const Mechanic);

	/**
	 * Process the mechanism using a functor mechanic in a threaded manner.
	 * Supports lambdas. Constant mechanic version.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FilterT The type of filter to query with.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Filter The filter to query with.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::None, typename FilterT = void, typename MechanicT = void >
	typename std::enable_if<!std::is_function<MechanicT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
	OperateConcurrently(FilterT&&        Filter,
						const MechanicT& Mechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true);

	/**
	 * Process the mechanism using a functor mechanic in a threaded manner.
	 * Supports lambdas. Mutable mechanic version.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FilterT The type of filter to query with.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Filter The filter to query with.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::None, typename FilterT = void, typename MechanicT = void >
	typename std::enable_if<!std::is_function<MechanicT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
	OperateConcurrently(FilterT&&   Filter,
						MechanicT&  Mechanic,
						const int32 ThreadsCountMax,
						const int32 SlotsPerThreadMin = 1,
						const bool  bSync = true);

	/**
	 * Process the chain using a free function mechanic in a
	 * parallel manner.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on function's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FilterT The type of filter to query with.
	 * @tparam FunctionT The type of the mechanic function
	 * to operate on the slots.
	 * @param Filter The filter to query with.
	 * @param Mechanic The mechanical function to operate with.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::None, typename FilterT = void, typename FunctionT = void >
	typename std::enable_if<std::is_function<FunctionT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
	OperateConcurrently(FilterT&&        Filter,
						FunctionT* const Mechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true);

	/**
	 * Process the mechanism using a functor mechanic in a threaded manner.
	 * Supports lambdas. Constant mechanic auto-filter version.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::None, typename MechanicT = void >
	typename std::enable_if<!std::is_function<MechanicT>::value, EApparatusStatus>::type
	OperateConcurrently(const MechanicT& Mechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true);

	/**
	 * Process the mechanism using a functor mechanic in a threaded manner.
	 * Supports lambdas. Mutable mechanic auto-filter version.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::None, typename MechanicT = void >
	typename std::enable_if<!std::is_function<MechanicT>::value, EApparatusStatus>::type
	OperateConcurrently(MechanicT&  Mechanic,
						const int32 ThreadsCountMax,
						const int32 SlotsPerThreadMin = 1,
						const bool  bSync = true);

	/**
	 * Process the chain using a free function mechanic in a
	 * parallel manner. Auto-filter version.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on function's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FunctionT The type of the mechanic function
	 * to operate on the slots.
	 * @param Mechanic The mechanical function to operate with.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::None, typename FunctionT = void >
	typename std::enable_if<std::is_function<FunctionT>::value, EApparatusStatus>::type
	OperateConcurrently(FunctionT* const Mechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true);

	/// @}
#pragma endregion Operating

  protected:

	// This is needed for some third-party compatibility:
	friend class UMechanical;

	FORCEINLINE float
	GetProcessedSteadyTime_Implementation() const
	{
		return DoGetProcessedSteadyTime(GetSteadyDeltaTime());
	}

	FORCEINLINE float
	CalcSteadyFrameRatio_Implementation() const
	{
		return DoCalcSteadyFrameRatio(GetOwnTime(),
									  GetSteadyDeltaTime());
	}

	FORCEINLINE void
	GetSteadyFrame_Implementation(int64& OutFrame) const
	{
		OutFrame = SteadyFrame;
	}

	FORCEINLINE float
	CalcSteadyFutureFactor_Implementation() const
	{
		return DoCalcSteadyFutureFactor(GetOwnTime(),
										GetSteadyDeltaTime());
	}

	FORCEINLINE float
	GetSteadyTime_Implementation() const
	{
		return SteadyFrame * GetSteadyDeltaTime();
	}
}; // class IMechanical

inline void
IMechanical::DoTick(float NewTime,
					float DeltaTime,
					float SteadyDeltaTime)
{
	bInsideTick = true;
	{
		InputTick();
	}

	const int64 NeededSteadyFrame = int64(NewTime / SteadyDeltaTime);

	if (ProcessedSteadyFrame != NeededSteadyFrame)
	{
		while (ProcessedSteadyFrame < NeededSteadyFrame)
		{
			if (ProcessedSteadyFrame == TNumericLimits<int64>::Max())
			{
				UE_LOG(LogApparatus, Fatal, TEXT("Steady frame count overflow detected. "
										         "Was your game session running for too long?"));
			}

			SteadyFrame = ProcessedSteadyFrame + 1;

			bInsideSteadyTick = true;
			SteadyTick(SteadyDeltaTime);
			bInsideSteadyTick = false;

			ProcessedSteadyFrame = SteadyFrame;
		}
	}

	bInsidePresentationTick = true;
	AActor* Actor = Cast<AActor>(this);
	if (Actor)
	{
		Actor->AActor::Tick(DeltaTime);
	}

	{
		PresentationTick(DeltaTime,
						 DoCalcSteadyFrameRatio(NewTime, SteadyDeltaTime),
						 DoCalcSteadyFutureFactor(NewTime, SteadyDeltaTime));
	}
	bInsidePresentationTick = false;

	PrevTime = NewTime;
	bInsideTick = false;
}
