/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Fingerprint.h
 * Created: Friday, 23rd October 2020 7:00:48 pm
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include <atomic>
#include "More/type_traits"

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "More/Templates/TypeHash.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define FINGERPRINT_H_SKIPPED_MACHINE_H
#endif

#include "BootFilter.h"
#include "Flagmark.h"
#include "Traitmark.h"
#include "Detailmark.h"

#ifdef FINGERPRINT_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Fingerprint.generated.h"


// Forward declarations:
template < typename ...Ts >
struct TFingerprint;
struct FFilter;

/**
 * The traits/details fingerprint.
 */
USTRUCT(BlueprintType, Meta = (HasNativeMake = "ApparatusRuntime.ApparatusFunctionLibrary.MakeFingerprint"))
struct APPARATUSRUNTIME_API FFingerprint
{
	GENERATED_BODY()

  public:

	/**
	 * The all-zeroed fingerprint.
	 */
	static const FFingerprint Zero;

	/**
	 * The type of traits array.
	 */
	typedef FTraitmark::TraitsType TraitsType;

	/**
	 * The type of details array.
	 */
	typedef FDetailmark::DetailsType DetailsType;

  private:

	/**
	 * The traitmark of the fingerprint.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fingerprint",
			  Meta = (AllowPrivateAccess = "true", DisplayAfter="Flagmark"))
	FTraitmark Traitmark;

	/**
	 * The details mark of the fingerprint.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fingerprint",
			  Meta = (AllowPrivateAccess = "true", DisplayAfter="Traitmark"))
	FDetailmark Detailmark;

#if WITH_EDITORONLY_DATA

	/**
	 * The flagmark of the fingerprint. 
	 */
	UPROPERTY(EditAnywhere, Category = "Fingerprint",
			  Meta = (Bitmask, BitmaskEnum = "EFlagmark", AllowPrivateAccess = "true"))
	int32 Flagmark = FM_None; // This field is used for serialization/editing purposes only.

#endif // WITH_EDITORONLY_DATA

	/**
	 * This is the real thread-safe
	 * flagmark that is used for the operations.
	 */
	std::atomic<int32> RealFlagmark{FM_None};

	/**
	 * The cached hash of the fingerprint.
	 * 
	 * If 0, has to be updated.
	 * This is made atomic for thread safety.
	 *
	 * The flagmark is not added up to this
	 * hash for the atomicity of the flagmark
	 * operations.
	 */
	mutable std::atomic<uint32> HashCache{0};

	/**
	 * Get the hash of a fingerprint.
	 */
	friend uint32
	GetTypeHash(const FFingerprint& Fingerprint);

	friend class UBelt;
	friend class AMechanism;
	friend class UApparatusFunctionLibrary;
	friend class UChunk;
	friend struct FSubjectHandle;
	friend struct FFilter;

  public:

#pragma region Flagmark
	/// @name Flagmark
	/// @{

	/**
	 * Get the flagmark.
	 * 
	 * @param MemoryOrder The memory order to use for the load.
	 * @return The current flagmark of the fingerprint.
	 * 
	 * @see SetFlagmark() HasFlag()
	 */
	FORCEINLINE EFlagmark
	GetFlagmark(const std::memory_order MemoryOrder = std::memory_order_acquire) const 
	{
		return (EFlagmark)RealFlagmark.load(MemoryOrder);
	}

	/**
	 * Set the new active flagmark.
	 *
	 * This operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flagmark to set to.
	 * @param MemoryOrder The memory order to use for the exchange.
	 * @return The previous flagmark.
	 * 
	 * @see GetFlagmark() SetFlag()
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, EFlagmark>
	SetFlagmark(const EFlagmark         InFlagmark,
				const std::memory_order MemoryOrder = std::memory_order_release)
	{
		if (IsHarsh(Paradigm))
		{
			return (EFlagmark)RealFlagmark.exchange((EFlagmarkType)InFlagmark, MemoryOrder);
		}
		else
		{
			const auto PrevFlagmark = (EFlagmark)RealFlagmark.exchange((EFlagmarkType)InFlagmark, MemoryOrder);
			return MakeOutcome<Paradigm, EFlagmark>(PrevFlagmark == InFlagmark ? EApparatusStatus::Noop : EApparatusStatus::Success,
													PrevFlagmark);
		}
	}

	/**
	 * Set the new active flagmark.
	 * Status version.
	 *
	 * This operation is atomic and thread-safe.
	 *
	 * @param InFlagmark The flagmark to set to.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 * 
	 * @see GetFlagmark() SetFlag()
	 */
	[[deprecated("The method is now deprecated. Use 'SetFlagmark' in a polite paradigm, please.")]]
	FORCEINLINE EApparatusStatus
	SetFlagmark_Status(const EFlagmark InFlagmark)
	{
		if ((EFlagmark)RealFlagmark.exchange((EFlagmarkType)InFlagmark) == InFlagmark)
			return EApparatusStatus::Noop;
		return EApparatusStatus::Success;
	}

	/**
	 * Set the new active flagmark to a masked flagmark.
	 * 
	 * An additional mask argument is used to only
	 * change certain bits within the flagmark.
	 * 
	 * The method is atomic and thread-safe.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flagmark to set to. Get masked by the second argument.
	 * @param InMask The bit selection to use. Only the set (1) bits get changed.
	 * @param MemoryOrder The memory order to use for the compare exchange operation.
	 * @return The previous value of the flagmark masked with the passed-in mask.
	 * 
	 * @see GetFlagmark(), SetFlagmark(), SetFlag()
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, EFlagmark>
	SetFlagmarkMasked(const EFlagmark         InFlagmark,
					  const EFlagmark         InMask,
					  const std::memory_order MemoryOrder = std::memory_order_release)
	{
		if (IsHarsh(Paradigm))
		{
			auto RawFlagmark   = RealFlagmark.load(std::memory_order_relaxed);
			const auto RawMask = (EFlagmarkType)InMask;
			while (!RealFlagmark.compare_exchange_weak(RawFlagmark,
													   (RawFlagmark & (~RawMask)) | (((EFlagmarkType)InFlagmark) & RawMask),
													   MemoryOrder,
													   std::memory_order_relaxed));
			return (EFlagmark)RawFlagmark & InMask;
		}
		else
		{
			auto RawFlagmark   = RealFlagmark.load(std::memory_order_relaxed);
			const auto RawMask = (EFlagmarkType)InMask;
			EFlagmarkType NewRawFlagmark;
			do
			{
				NewRawFlagmark = (RawFlagmark & (~RawMask)) | (((EFlagmarkType)InFlagmark) & RawMask);
			}
			while (!RealFlagmark.compare_exchange_weak(RawFlagmark, NewRawFlagmark, MemoryOrder, std::memory_order_relaxed));
			return MakeOutcome<Paradigm, EFlagmark>(
						(RawFlagmark == NewRawFlagmark) ? EApparatusStatus::Noop : EApparatusStatus::Success,
						(EFlagmark)RawFlagmark & InMask);
		}
	}

	/**
	 * Set the new active flagmark to a masked flagmark.
	 * Status version.
	 * 
	 * An additional mask argument is used to only
	 * change certain bits within the flagmark.
	 * 
	 * The method is atomic and thread-safe.
	 * 
	 * @param InFlagmark The flagmark to set to. Get masked by the second argument.
	 * @param InMask The bit selection to use. Only the set (1) bits get changed.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 * 
	 * @see GetFlagmark() SetFlag()
	 */
	[[deprecated("The method is now deprecated. Use 'SetFlagmarkMasked' in a polite paradigm, please.")]]
	FORCEINLINE EApparatusStatus
	SetFlagmarkMasked_Status(const EFlagmark InFlagmark,
							 const EFlagmark InMask)
	{
		auto RawFlagmark   = RealFlagmark.load();
		const auto RawMask = (EFlagmarkType)InMask;
		EFlagmarkType NewRawFlagmark;
		do
		{
			NewRawFlagmark = (RawFlagmark & (~RawMask)) | (((EFlagmarkType)InFlagmark) & RawMask);
		}
		while (!RealFlagmark.compare_exchange_weak(RawFlagmark, NewRawFlagmark));
		return (RawFlagmark == NewRawFlagmark) ? EApparatusStatus::Noop : EApparatusStatus::Success;
	}

	/**
	 * Add flags to the flagmark.
	 * 
	 * The method is atomic and thread-safe.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flags to add with a binary "or".
	 * @param MemoryOrder The memory order to use for the 'fetch or' operation.
	 * @return The previous flagmark.
	 * 
	 * @see SetFlag()
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, EFlagmark>
	AddToFlagmark(const EFlagmark         InFlagmark,
				  const std::memory_order MemoryOrder = std::memory_order_release)
	{
		if (IsHarsh(Paradigm))
		{
			return (EFlagmark)RealFlagmark.fetch_or((EFlagmarkType)InFlagmark, MemoryOrder);
		}
		else
		{
			auto       RawFlagmark   = RealFlagmark.load(std::memory_order_relaxed);
			const auto RawInFlagmark = (EFlagmarkType)InFlagmark;
			EFlagmarkType NewRawFlagmark;
			do
			{
				NewRawFlagmark = RawFlagmark | RawInFlagmark;
			}
			while (!RealFlagmark.compare_exchange_weak(RawFlagmark, NewRawFlagmark,
													   MemoryOrder, std::memory_order_relaxed));
			return MakeOutcome<Paradigm, EFlagmark>((RawFlagmark == NewRawFlagmark) ? EApparatusStatus::Noop : EApparatusStatus::Success,
													(EFlagmark)RawFlagmark);
		}
	}

	/**
	 * Add flags to the flagmark.
	 * Status version.
	 * 
	 * The method is atomic and thread-safe.
	 * 
	 * @tparam ParadigmT The type of the paradigm to work under.
	 * @param InFlagmark The flags to add with a binary "or".
	 * @return The previous flagmark.
	 * 
	 * @see AddToFlagmark()
	 */
	[[deprecated("The method is now deprecated. Use 'AddToFlagmark' in a polite paradigm, please.")]]
	FORCEINLINE EApparatusStatus
	AddToFlagmark_Status(const EFlagmark InFlagmark)
	{
		auto       RawFlagmark   = RealFlagmark.load();
		const auto RawInFlagmark = (EFlagmarkType)InFlagmark;
		EFlagmarkType NewRawFlagmark;
		do
		{
			NewRawFlagmark = RawFlagmark | RawInFlagmark;
		}
		while (!RealFlagmark.compare_exchange_weak(RawFlagmark, NewRawFlagmark));
		return (RawFlagmark == NewRawFlagmark) ? EApparatusStatus::Noop : EApparatusStatus::Success;
	}

	/**
	 * Add flagmark to the fingerprint.
	 */
	FORCEINLINE FFingerprint&
	operator+=(const EFlagmark InFlagmark)
	{
		AddToFlagmark(InFlagmark);
		return *this;
	}

	/**
	 * Remove flags from the flagmark.
	 * 
	 * The method is atomic and thread-safe.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flags to remove.
	 * @param MemoryOrder The memory order to use for the 'fetch and' operation.
	 * @return The previous flagmark.
	 * 
	 * @see SetFlag(), AddToFlagmark(const EFlagmark)
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, EFlagmark>
	RemoveFromFlagmark(const EFlagmark         InFlagmark,
					   const std::memory_order MemoryOrder = std::memory_order_release)
	{
		if (IsHarsh(Paradigm))
		{
			return (EFlagmark)RealFlagmark.fetch_and(~((EFlagmarkType)InFlagmark), MemoryOrder);
		}
		else
		{
			auto       RawFlagmark   = RealFlagmark.load(std::memory_order_relaxed);
			const auto RawInFlagmark = (EFlagmarkType)InFlagmark;
			EFlagmarkType NewRawFlagmark;
			do
			{
				NewRawFlagmark = RawFlagmark & ~RawInFlagmark;
			}
			while (!RealFlagmark.compare_exchange_weak(RawFlagmark, NewRawFlagmark, MemoryOrder, std::memory_order_relaxed));
			return MakeOutcome<Paradigm, EFlagmark>((RawFlagmark == NewRawFlagmark) ? EApparatusStatus::Noop : EApparatusStatus::Success,
													(EFlagmark)RawFlagmark);
		}
	}

	/**
	 * Remove flagmark from the fingerprint.
	 */
	FORCEINLINE FFingerprint&
	operator-=(const EFlagmark InFlagmark)
	{
		RemoveFromFlagmark(InFlagmark);
		return *this;
	}

	/**
	 * Remove flags from the flagmark.
	 * Status version.
	 * 
	 * The method is atomic and thread-safe.
	 * 
	 * @param InFlagmark The flags to remove.
	 * @return The previous flagmark.
	 * 
	 * @see RemoveFromFlagmark(const EFlagmark), AddFromFlagmark_Status(const EFlagmark)
	 */
	[[deprecated("The method is now deprecated. Use 'RemoveFromFlagmark' in a polite paradigm, please.")]]
	FORCEINLINE EApparatusStatus
	RemoveFromFlagmark_Status(const EFlagmark InFlagmark)
	{
		auto       RawFlagmark   = RealFlagmark.load();
		const auto RawInFlagmark = (EFlagmarkType)InFlagmark;
		EFlagmarkType NewRawFlagmark;
		do
		{
			NewRawFlagmark = RawFlagmark & ~RawInFlagmark;
		}
		while (!RealFlagmark.compare_exchange_weak(RawFlagmark, NewRawFlagmark));
		return (RawFlagmark == NewRawFlagmark) ? EApparatusStatus::Noop : EApparatusStatus::Success;
	}

	/**
	 * Get the flag state of the fingerprint.
	 * 
	 * @param Flag The flag to examine.
	 * @param MemoryOrder The memory order to use for the 'load' operation.
	 * @return The current state of the flag.
	 * 
	 * @see SetFlag()
	 */
	FORCEINLINE bool
	HasFlag(const EFlagmarkBit      Flag,
			const std::memory_order MemoryOrder = std::memory_order_acquire) const
	{
		return (RealFlagmark.load(MemoryOrder) & ((EFlagmarkType)1 << (std::underlying_type_t<EFlagmarkBit>)Flag));
	}

	/**
	 * Set a flagmark bit to a specific state.
	 *
	 * The method is atomic and thread-safe.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flag The flag to set or clear.
	 * @param bState The state to set the flag to.
	 * @param MemoryOrder The memory order to use for the 'load' and 'compare_exchange' operations.
	 * @return The previous state of the flag.
	 * 
	 * @see AddToFlagmark(), HasFlag()
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, bool>
	SetFlag(const EFlagmarkBit      Flag,
			const bool              bState = true,
			const std::memory_order MemoryOrder = std::memory_order_release)
	{
		const auto RawFlag = (std::underlying_type_t<EFlagmarkBit>)Flag;
		auto RawFlagmark = RealFlagmark.load(std::memory_order_relaxed);
		if (LIKELY(bState))
		{
			while (!RealFlagmark.compare_exchange_weak(RawFlagmark,
													   RawFlagmark | ((EFlagmarkType)1 << RawFlag),
													   MemoryOrder, std::memory_order_relaxed));
		}
		else
		{
			while (!RealFlagmark.compare_exchange_weak(RawFlagmark,
													   RawFlagmark & ~((EFlagmarkType)1 << RawFlag),
													   MemoryOrder, std::memory_order_relaxed));
		}
		const bool PrevFlagState = RawFlagmark & ((EFlagmarkType)1 << RawFlag);
		return MakeOutcome<Paradigm, bool>(PrevFlagState == bState ? EApparatusStatus::Noop : EApparatusStatus::Success,
										   PrevFlagState);
	}

	/**
	 * Set a flagmark bit to a specific state.
	 * Status version.
	 *
	 * The method is atomic and thread-safe.
	 *
	 * @param Flag The flag to set or clear.
	 * @param bState The state to set the flag to.
	 * @return The status of the operation.
	 * 
	 * @see AddToFlagmark()
	 */
	[[deprecated("The method is now deprecated. Use 'SetFlag' in a polite paradigm, please.")]]
	FORCEINLINE EApparatusStatus
	SetFlag_Status(const EFlagmarkBit Flag,
				   const bool         bState = true)
	{
		return (SetFlag(Flag, bState) == bState) ? EApparatusStatus::Noop : EApparatusStatus::Success;
	}

	/**
	 * Toggle the state of a flagmark bit.
	 *
	 * The method is atomic and thread-safe.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flag The flag to toggle.
	 * @param MemoryOrder The memory order to use for the 'load' and 'compare exchange' operation.
	 * @return The new state of the flag.
	 * 
	 * @see HasFlag(), SetFlag()
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, bool>
	ToggleFlag(const EFlagmarkBit      Flag,
			   const std::memory_order MemoryOrder = std::memory_order_release)
	{
		const auto RawFlag = (std::underlying_type_t<EFlagmarkBit>)Flag;
		auto RawFlagmark = RealFlagmark.load(std::memory_order_relaxed);
		while (!RealFlagmark.compare_exchange_weak(RawFlagmark,
												   RawFlagmark ^ ((EFlagmarkType)1 << RawFlag),
												   MemoryOrder, std::memory_order_relaxed));
		const bool PrevFlagState = RawFlagmark & ((EFlagmarkType)1 << RawFlag);
		return MakeOutcome<Paradigm, bool>(EApparatusStatus::Success, !PrevFlagState);
	}

	/**
	 * Set the new active boot state.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param bState The boot state to set.
	 * @param MemoryOrder The memory order to use for the 'load' and 'compare exchange' operation.
	 * @return The previous state of the boot flag.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, bool>
	SetBooted(bool                    bState      = true,
			  const std::memory_order MemoryOrder = std::memory_order_release)
	{
		return SetFlag<Paradigm>(EFlagmarkBit::Booted, bState, MemoryOrder);
	}

	/**
	 * Check if the fingerprint corresponds to a stale entity.
	 *
	 * This examination is atomic and thread-safe.
	 *
	 * @param MemoryOrder The memory order to use for the 'load' operation.
	 * @return The state of being stale.
	 */
	FORCEINLINE bool
	IsStale(const std::memory_order MemoryOrder = std::memory_order_acquire) const
	{
		return HasFlag(EFlagmarkBit::Stale, MemoryOrder);
	}

	/**
	 * Check if the fingerprint corresponds to a booted entity.
	 *
	 * This examination is atomic and thread-safe.
	 *
	 * @param MemoryOrder The memory order to use for the 'load' operation.
	 * @return The state of being booted.
	 */
	FORCEINLINE bool
	IsBooted(const std::memory_order MemoryOrder = std::memory_order_acquire) const
	{
		return HasFlag(EFlagmarkBit::Booted, MemoryOrder);
	}

	/// @}
#pragma endregion Flagmark

	/**
	 * Get the traits of the fingerprint.
	 */
	FORCEINLINE const TraitsType& 
	GetTraits() const
	{
		return Traitmark.GetTraits();
	}

	/**
	 * Get the details of the fingerprint.
	 */
	FORCEINLINE const DetailsType& 
	GetDetails() const
	{
		return Detailmark.GetDetails();
	}

	/**
	 * The number of traits in the fingerprint.
	 */
	FORCEINLINE int32
	TraitsNum() const
	{
		return Traitmark.TraitsNum();
	}

	/**
	 * The number of details in the fingerprint.
	 */
	FORCEINLINE int32
	DetailsNum() const 
	{
		return Detailmark.DetailsNum();
	}

	/**
	 * Get the traitmark part of the fingerprint.
	 */
	FORCEINLINE const FTraitmark&
	GetTraitmark() const
	{
		return Traitmark;
	}

	/**
	 * Get the detailmark part of the fingerprint.
	 */
	FORCEINLINE const FDetailmark&
	GetDetailmark() const
	{
		return Detailmark;
	}

	/**
	 * Convert a fingerprint to a traitmark.
	 */
	FORCEINLINE operator const FTraitmark&() const
	{
		return GetTraitmark();
	}

	/**
	 * Convert a fingerprint to a detailmark.
	 */
	FORCEINLINE operator const FDetailmark&() const
	{
		return GetDetailmark();
	}

	/**
	 * Get the traits mask of the fingerprint.
	 */
	FORCEINLINE const FBitMask&
	GetTraitsMask() const
	{
		return Traitmark.GetTraitsMask();
	}

	/**
	 * Get the details mask of the fingerprint.
	 */
	FORCEINLINE const FBitMask&
	GetDetailsMask() const
	{
		return Detailmark.GetDetailsMask();
	}

	/**
	 * Get a trait by its index.
	 */
	FORCEINLINE UScriptStruct*
	TraitAt(const int32 Index) const
	{
		return Traitmark.TraitAt(Index);
	}

	/**
	 * Get a detail by its index.
	 */
	FORCEINLINE TSubclassOf<UDetail>
	DetailAt(const int32 Index) const
	{
		return Detailmark.DetailAt(Index);
	}

	/**
	 * Convert to an array of trait types.
	 */
	FORCEINLINE explicit operator TraitsType() const
	{
		return (TraitsType)Traitmark;
	}

	/**
	 * Convert to an array of detail classes.
	 */
	FORCEINLINE explicit operator DetailsType() const
	{
		return (DetailsType)Detailmark;
	}

	/**
	 * Check if the fingerprint is completely empty.
	 */
	FORCEINLINE bool
	IsEmpty() const
	{
		return (RealFlagmark.load() == (int32)FM_None) &&
			   (Traitmark.IsEmpty()) &&
			   (Detailmark.IsEmpty());
	}

	/**
	 * Check if a fingerprint is viable and has any effect.
	 */
	FORCEINLINE operator bool() const
	{
		return RealFlagmark.load() || Traitmark || Detailmark;
	}

#pragma region Mapping

	/**
	 * Get an indexing mapping from another fingerprint defined by an array of details.
	 *
	 * @param InDetailClasses A fingerprint to get a mapping from.
	 * @param OutMapping The resulting mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindDetailsMappingFrom(const TArray<TSubclassOf<UDetail>>& InDetailClasses,
						   TArray<int32>&                      OutMapping) const
	{
		return Detailmark.FindMappingFrom(InDetailClasses, OutMapping);
	}

	/**
	 * Get an indexing mapping from another traitmark defined by an array of traits.
	 *
	 * @param InTraitmark A traitmark to get a mapping from.
	 * @param OutMapping The resulting mapping.
	 */
	FORCEINLINE void
	FindTraitsMappingFrom(const TArray<UScriptStruct*>& InTraitmark,
						 TArray<int32>&                 OutMapping) const
	{
		Traitmark.FindMappingFrom(InTraitmark, OutMapping);
	}

	/**
	 * Get an indexing multi-mapping from another fingerprint defined by an array of details.
	 *
	 * @param InDetailClasses A fingerprint to get a mapping from.
	 * @param OutMapping The resulting two-dimensional multi-mapping.
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	FindDetailsMappingFrom(const TArray<TSubclassOf<UDetail>>& InDetailClasses,
						   TArray<TArray<int32>>&              OutMapping) const
	{
		return Detailmark.FindMappingFrom(InDetailClasses, OutMapping);
	}

	/**
	 * Get an indexing mapping from another fingerprint.
	 *
	 * @param Fingerprint A fingerprint to get a mapping from.
	 * @param OutMapping The resulting mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindDetailsMappingFrom(const FFingerprint& Fingerprint,
						   TArray<int32>&      OutMapping) const
	{
		return Detailmark.FindMappingFrom(Fingerprint.Detailmark, OutMapping);
	}

	/**
	 * Get an indexing mapping from another fingerprint's traits.
	 *
	 * @param Fingerprint A fingerprint to get a mapping from.
	 * @param OutMapping The resulting traits mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindTraitsMappingFrom(const FFingerprint& Fingerprint,
						  TArray<int32>&      OutMapping) const
	{
		return Traitmark.FindMappingFrom(Fingerprint.Traitmark, OutMapping);
	}

	/**
	 * Get an indexing mapping from another traitmark's traits.
	 *
	 * @param InTraitmark A traitmark to get a mapping from.
	 * @param OutMapping  The resulting traits mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindTraitsMappingFrom(const FTraitmark& InTraitmark,
						  TArray<int32>&    OutMapping) const
	{
		return Traitmark.FindMappingFrom(InTraitmark, OutMapping);
	}

	/**
	 * Get an indexing details mapping to another fingerprint.
	 *
	 * @param Fingerprint A fingerprint to get the details mapping to.
	 * @param OutMapping The resulting mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindDetailsMappingTo(const FFingerprint& Fingerprint,
						 TArray<int32>&      OutMapping) const
	{
		return Fingerprint.FindDetailsMappingFrom(GetDetails(), OutMapping);
	}

	/**
	 * Get an indexing traits mapping to another traitmark.
	 *
	 * @param InTraitmark A traitmark to get the traits mapping to.
	 * @param OutMapping  The resulting mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindTraitsMappingTo(const FTraitmark& InTraitmark,
						TArray<int32>&    OutMapping) const
	{
		return Traitmark.FindMappingTo(InTraitmark, OutMapping);
	}

	/**
	 * Get an indexing traits mapping to another traitmark.
	 *
	 * @param InFingerprint A fingerprint to get the traits mapping to.
	 * @param OutMapping The resulting mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindTraitsMappingTo(const FFingerprint& InFingerprint,
						TArray<int32>&      OutMapping) const
	{
		return Traitmark.FindMappingTo(InFingerprint.Traitmark, OutMapping);
	}

	/**
	 * Get a detail indexing multi-mapping from another fingerprint.
	 *
	 * @param Fingerprint A fingerprint to get a mapping from.
	 * @param OutMapping The resulting mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindDetailsMappingFrom(const FFingerprint&    Fingerprint,
						   TArray<TArray<int32>>& OutMapping) const
	{
		return Detailmark.FindMappingFrom(Fingerprint.Detailmark, OutMapping);
	}

	/**
	 * Get an indexing multi-mapping to another fingerprint.
	 *
	 * @param Fingerprint A fingerprint to get a mapping to.
	 * @param OutMapping The resulting mapping.
	 */
	FORCEINLINE EApparatusStatus
	FindDetailsMappingTo(const FFingerprint&    Fingerprint,
						 TArray<TArray<int32>>& OutMapping) const
	{
		return Detailmark.FindMappingTo(Fingerprint.Detailmark, OutMapping);
	}

#pragma endregion Mapping

#pragma region Matching

	/**
	 * Check if the fingerprint's flagmark matches a filter.
	 */
	bool
	FlagmarkMatches(const FFilter& Filter) const;

	/**
	 * Check if the fingerprint's traits part match a filter.
	 */
	bool
	TraitsMatch(const FFilter& Filter) const;

	/**
	 * Check if the fingerprint's details part match a filter.
	 */
	bool
	DetailsMatch(const FFilter& Filter) const;

	/**
	 * Check if the fingerprint matches
	 * the flagmarks used as filters.
	 */
	bool
	Matches(const EFlagmark IncludingFlagmark,
			const EFlagmark ExcludingFlagmark = FM_None) const;

	/**
	 * Check if the fingerprint matches a filter.
	 */
	bool
	Matches(const FFilter& Filter) const;

	/**
	 * Check if the fingerprint matches a filter
	 * with flagmarks filtering overriden.
	 */
	bool
	Matches(const FFilter&  Filter,
			const EFlagmark IncludingFlagmarkOverride,
			const EFlagmark ExcludingFlagmarkOverride) const;

	/**
	 * Check if the fingerprint matches another fingerprint acting as a filter.
	 */
	FORCEINLINE bool
	Matches(const FFingerprint& InFingerprint) const
	{
		return 
			EnumHasAllFlags(GetFlagmark(), InFingerprint.GetFlagmark()) &&
			GetTraitmark().Matches(InFingerprint.GetTraitmark()) &&
			GetDetailmark().Matches(InFingerprint.GetDetailmark());
	}

	/**
	 * Check if the fingerprint matches a traitmark acting as a filter.
	 */
	FORCEINLINE bool
	Matches(const FTraitmark& InTraitmark) const
	{
		return Traitmark.Matches(InTraitmark);
	}

	/**
	 * Check if the fingerprint matches a detailmark acting as a filter.
	 */
	FORCEINLINE bool
	Matches(const FDetailmark& InDetailmark) const
	{
		return Detailmark.Matches(InDetailmark);
	}

#pragma endregion Matching

#pragma region Comparison

	/**
	 * Compare two fingerprints for equality.
	 */
	bool
	operator==(const FFingerprint& Other) const
	{
		if (UNLIKELY(this == std::addressof(Other)))
			return true;

		return (CalcHash() == Other.CalcHash()) && 
			   (GetFlagmark() == Other.GetFlagmark()) &&
			   (GetTraitmark() == Other.GetTraitmark()) &&
			   (GetDetailmark() == Other.GetDetailmark());
	}

	/**
	 * Compare two fingerprint for inequality.
	 */
	bool
	operator!=(const FFingerprint& Other) const
	{
		if (UNLIKELY(this == std::addressof(Other)))
			return false;

		return (CalcHash() != Other.CalcHash()) ||
			   (GetFlagmark() != Other.GetFlagmark()) ||
			   (GetTraitmark() != Other.GetTraitmark()) ||
			   (GetDetailmark() != Other.GetDetailmark());
	}

	/**
	 * Compare two fingerprints for equality. Editor-friendly method.
	 * 
	 * @param Other The other fingerprint to compare to.
	 * @param PortFlags The contextual port flags.
	 * @return The state of examination.
	 */
	bool
	Identical(const FFingerprint* Other, uint32 PortFlags) const
	{
		if (UNLIKELY(this == Other))
		{
			return true;
		}
		return (CalcHash() == Other->CalcHash()) &&
			   (GetFlagmark() == Other->GetFlagmark()) &&
			   (GetTraitmark().Identical(&Other->Traitmark, PortFlags)) &&
			   (GetDetailmark().Identical(&Other->Detailmark, PortFlags));
	}

#pragma endregion Comparison

#pragma region Search

	/**
	 * Get the index of a specific trait type.
	 */
	FORCEINLINE int32
	IndexOf(UScriptStruct* const TraitType) const
	{
		return Traitmark.IndexOf(TraitType);
	}

	/**
	 * Get the index of a specific detail class.
	 * 
	 * Also supports parent classes.
	 * 
	 * @param DetailClass The detail class to look for.
	 */
	FORCEINLINE int32
	IndexOf(const TSubclassOf<UDetail> DetailClass) const
	{
		return Detailmark.IndexOf(DetailClass);
	}

	/**
	 * Get the indices of a specific detail class.
	 * 
	 * Also supports searches by the parental classes.
	 * 
	 * @param DetailClass The detail class to search for.
	 * May also be a base class.
	 * @param OutIndices The indices array to output to.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop If there are no such details.
	 */
	EApparatusStatus
	IndicesOf(const TSubclassOf<UDetail> DetailClass,
			  TArray<int32>&             OutIndices) const
	{
		return Detailmark.IndicesOf(DetailClass, OutIndices);
	}

#pragma endregion Search

#pragma region Assignment

	/**
	 * Move a traitmark to the fingerprint's traitmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmark The traitmark to move.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitmark(FTraitmark&& InTraitmark)
	{
		const auto Outcome = Traitmark.template Set<MakePolite(Paradigm)>(InTraitmark);
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Set the fingerprint's traitmark equal to a traitmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmark The traitmark to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitmark(const FTraitmark& InTraitmark)
	{
		const auto Outcome = Traitmark.template Set<MakePolite(Paradigm)>(InTraitmark);
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Move a detailmark to the fingerprint's detailmark.
	 * 
	 * The traitmark and the flagmark are preserved.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The detailmark to move.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetDetailmark(FDetailmark&& InDetailmark)
	{
		const auto Outcome = Detailmark.template Set<MakePolite(Paradigm)>(InDetailmark);
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Set the fingerprint's detailmark equal to a detailmark.
	 * 
	 * The traitmark is preserved, excluding the boot state.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The detailmark to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetDetailmark(const FDetailmark& InDetailmark)
	{
		const auto Outcome = Detailmark.template Set<MakePolite(Paradigm)>(InDetailmark);
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Move a traitmark to the fingerprint.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmark The traitmark to set to.
	 * @param bPreserveFlagmark Should existing flagmark be preserved.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually performed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Set(FTraitmark&& InTraitmark,
		const bool   bPreserveFlagmark = true)
	{ 
		auto Status = bPreserveFlagmark ? EApparatusStatus::Noop : ToStatus(SetFlagmark<Paradigm>(FM_None));
		const auto ComponentsOutcome = OutcomeCombine(SetTraitmark<MakePolite(Paradigm)>(InTraitmark),
													  Detailmark.template Reset<MakePolite(Paradigm)>());
		if (LIKELY(!IsNoop(ComponentsOutcome)))
		{
			HashCache = 0;
			return OutcomeCombine(Status, ComponentsOutcome);
		}
		return Status;
	}

	/**
	 * Set the fingerprint equal to a traitmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmark The traitmark to set to.
	 * @param bPreserveFlagmark Should existing flagmark be preserved.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually performed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Set(const FTraitmark& InTraitmark,
		const bool        bPreserveFlagmark = true)
	{
		auto Status = bPreserveFlagmark ? EApparatusStatus::Noop : ToStatus(SetFlagmark<Paradigm>(FM_None));
		const auto ComponentsOutcome = OutcomeCombine(SetTraitmark<MakePolite(Paradigm)>(InTraitmark),
													  Detailmark.template Reset<MakePolite(Paradigm)>());
		if (LIKELY(!IsNoop(ComponentsOutcome)))
		{
			HashCache = 0;
			return OutcomeCombine(Status, ComponentsOutcome);
		}
		return Status;
	}

	/**
	 * Move a detailmark to the fingerprint.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The detailmark to set to.
	 * @param bPreserveFlagmark Should existing flagmark be preserved.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually performed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Set(FDetailmark&& InDetailmark,
		const bool    bPreserveFlagmark = true)
	{
		auto Status = bPreserveFlagmark ? EApparatusStatus::Noop : ToStatus(SetFlagmark<Paradigm>(FM_None));
		const auto OtherStatus = StatusCombine(Traitmark.template Reset<MakePolite(Paradigm)>(), SetDetailmark<MakePolite(Paradigm)>(InDetailmark));
		if (LIKELY(OtherStatus != EApparatusStatus::Noop))
		{
			HashCache = 0;
			Status += OtherStatus;
		}
		return Status;
	}

	/**
	 * Set the fingerprint equal to a detailmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The detailmark to set to.
	 * @param bPreserveFlagmark Should existing flagmark be preserved.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually performed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Set(const FDetailmark& InDetailmark,
		const bool         bPreserveFlagmark = true)
	{
		auto Status = bPreserveFlagmark ? EApparatusStatus::Noop : ToStatus(SetFlagmark<Paradigm>(FM_None));
		const auto OtherStatus = StatusCombine(Traitmark.template Reset<MakePolite(Paradigm)>(), SetDetailmark<MakePolite(Paradigm)>(InDetailmark));
		if (LIKELY(OtherStatus != EApparatusStatus::Noop))
		{
			HashCache = 0;
			Status += OtherStatus;
		}
		return Status;
	}

	/**
	 * Move another fingerprint to this one.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFingerprint The fingerprint to move.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Set(FFingerprint&& InFingerprint)
	{
		HashCache = InFingerprint.HashCache.load();
		InFingerprint.HashCache = 0;
		if (IsHarsh(Paradigm)) // Compile-time branch.
		{
			SetFlagmark<Paradigm>(InFingerprint.GetFlagmark());
			Traitmark.template Set<Paradigm>(MoveTemp(InFingerprint.Traitmark));
			Detailmark.template Set<Paradigm>(MoveTemp(InFingerprint.Detailmark));
			return EApparatusStatus::Success;
		}
		else
		{
			return
				StatusCombine(SetFlagmark<MakePolite(Paradigm)>(InFingerprint.GetFlagmark()),
							  Traitmark.template Set<Paradigm>(MoveTemp(InFingerprint.Traitmark)),
							  Detailmark.template Set<Paradigm>(MoveTemp(InFingerprint.Detailmark)));
		}
	}

	/**
	 * Set the fingerprint equal to another one.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Set(const FFingerprint& InFingerprint)
	{
		if (UNLIKELY(std::addressof(InFingerprint) == this))
		{
			return EApparatusStatus::Noop;
		}
		if (IsHarsh(Paradigm)) // Compile-time branch.
		{
#if WITH_EDITORONLY_DATA
			Flagmark     = InFingerprint.Flagmark;
#endif
			RealFlagmark = InFingerprint.RealFlagmark.load();
			if (LIKELY(HashCache != InFingerprint.HashCache))
			{
				Traitmark.template Set<Paradigm>(InFingerprint.Traitmark);
				Detailmark.template Set<Paradigm>(InFingerprint.Detailmark);
				HashCache = InFingerprint.HashCache.load(); // Safer to set as zero since thread-safety...
			}
			else
			{
				const auto ComponentsStatus =
					StatusCombine(Traitmark.template Set<MakePolite(Paradigm)>(InFingerprint.Traitmark),
								  Detailmark.template Set<MakePolite(Paradigm)>(InFingerprint.Detailmark));
				if (LIKELY(ComponentsStatus != EApparatusStatus::Noop))
				{
					HashCache = 0;
				}
			}
			return EApparatusStatus::Success;
		}
		else
		{
			if (LIKELY(HashCache != InFingerprint.HashCache))
			{
#if WITH_EDITORONLY_DATA
				Flagmark     = InFingerprint.Flagmark;
#endif
				RealFlagmark = InFingerprint.RealFlagmark.load();
				Traitmark.template Set<Paradigm>(InFingerprint.Traitmark);
				Detailmark.template Set<Paradigm>(InFingerprint.Detailmark);
				HashCache = InFingerprint.HashCache.load();
				return EApparatusStatus::Success;
			}
			else
			{
				EApparatusStatus Status = SetFlagmark<MakePolite(Paradigm)>(InFingerprint.GetFlagmark());
				const auto ComponentsStatus =
					StatusCombine(Traitmark.template Set<MakePolite(Paradigm)>(InFingerprint.Traitmark),
								  Detailmark.template Set<MakePolite(Paradigm)>(InFingerprint.Detailmark));
				if (LIKELY(ComponentsStatus != EApparatusStatus::Noop))
				{
#if WITH_EDITORONLY_DATA
					Flagmark = InFingerprint.Flagmark;
#endif
					HashCache = 0;
					Status += ComponentsStatus;
				}
				return Status;
			}
		}
	}

	/**
	 * Set the fingerprint to an array of traits and a flagmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The allocator of the array container.
	 * @param InTraits The traits to set to.
	 * @param InFlagmark The flagmark to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	TOutcome<Paradigm>
	Set(const TArray<UScriptStruct*, AllocatorT>& InTraits,
		const EFlagmark                           InFlagmark)
	{
		TOutcome<Paradigm> Status = SetFlagmark<Paradigm>(InFlagmark);
		const auto ComponentsStatus =
			StatusCombine(Traitmark.template Set<MakePolite(Paradigm)>(InTraits),
						  Detailmark.template Reset<MakePolite(Paradigm)>());
		if (LIKELY(!IsNoop(ComponentsStatus)))
		{
			HashCache = 0;
		}
		return Status;
	}

	/**
	 * Set a fingerprint to an array of detail classes and a flagmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The allocator of the array container.
	 * @param InDetailClasses The details to set to.
	 * @param InFlagmark The flagmark to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	TOutcome<Paradigm>
	Set(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses,
		const EFlagmark                                 InFlagmark)
	{
		EApparatusStatus Status = SetFlagmark<Paradigm>(InFlagmark);
		const auto ComponentsStatus =
			StatusCombine(Traitmark.template Reset<MakePolite(Paradigm)>(),
						  Detailmark.template Set<MakePolite(Paradigm)>(InDetailClasses));
		if (LIKELY(ComponentsStatus != EApparatusStatus::Noop))
		{
			HashCache = 0;
			Status += ComponentsStatus;
		}
		return Status;
	}

	/**
	 * Set a fingerprint to an array of active-tested details and a flagmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The allocator of the array container.
	 * @param InDetails The details to set to.
	 * @param InFlagmark The flagmark to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	TOutcome<Paradigm>
	Set(const TArray<UDetail*, AllocatorT>& InDetails,
		const EFlagmark                     InFlagmark)
	{
		EApparatusStatus Status = SetFlagmark<Paradigm>(InFlagmark);
		const auto ComponentsStatus = 
			StatusCombine(Traitmark.template Reset<MakePolite(Paradigm)>(),
						  Detailmark.template Set<MakePolite(Paradigm)>(InDetails));
		if (LIKELY(ComponentsStatus != EApparatusStatus::Noop))
		{
			HashCache = 0;
			Status += ComponentsStatus;
		}
		return Status;
	}

	/**
	 * Move a traitmark to the fingerprint.
	 */
	FORCEINLINE FFingerprint&
	operator=(FTraitmark&& InTraitmark)
	{
		Set(InTraitmark);
		return *this;
	}

	/**
	 * Set the fingerprint equal to a traitmark.
	 */
	FORCEINLINE FFingerprint&
	operator=(const FTraitmark& InTraitmark)
	{
		Set(InTraitmark);
		return *this;
	}

	/**
	 * Move a detailmark to the fingerprint.
	 */
	FORCEINLINE FFingerprint&
	operator=(FDetailmark&& InDetailmark)
	{
		Set(InDetailmark);
		return *this;
	}

	/**
	 * Set the fingerprint equal to a detailmark.
	 */
	FORCEINLINE FFingerprint&
	operator=(const FDetailmark& InDetailmark)
	{
		Set(InDetailmark);
		return *this;
	}

	/**
	 * Move a fingerprint to this one.
	 */
	FORCEINLINE FFingerprint&
	operator=(FFingerprint&& Fingerprint)
	{
		Set(Fingerprint);
		return *this;
	}

	/**
	 * Set the fingerprint equal to another fingerprint.
	 */
	FORCEINLINE FFingerprint&
	operator=(const FFingerprint& Fingerprint)
	{
		Set(Fingerprint);
		return *this;
	}

	/**
	 * Set a fingerprint equal to an array of trait types.
	 * 
	 * The flagmark is preserved.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFingerprint&
	operator=(const TArray<UScriptStruct*, AllocatorT>& InTraitTypes)
	{
		Set(InTraitTypes, GetFlagmark());
		return *this;
	}

	/**
	 * Set a fingerprint equal to an array of detail classes.
	 * 
	 * The flagmark is preserved.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFingerprint&
	operator=(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses)
	{
		Set(InDetailClasses, GetFlagmark());
		return *this;
	}

	/**
	 * Set a fingerprint equal to an array of details.
	 * 
	 * The flagmark is preserved.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFingerprint&
	operator=(const TArray<UDetail*, AllocatorT>& InDetails)
	{
		Set(InDetails, GetFlagmark());
		return *this;
	}

#pragma endregion Assignment

#pragma region Component Operations

  private:

	template < typename C,
			   bool bTrait  = IsTraitType<C>(),
			   bool bDetail = IsDetailClass<C>() >
	struct TComponentOps
	{
		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Add(FFingerprint* const InFingerprint)
		{
			return TOutcome<Paradigm>::Noop();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Remove(FFingerprint* const InFingerprint)
		{
			return TOutcome<Paradigm>::Noop();
		}

		static constexpr FORCEINLINE bool
		Contains(const FFingerprint* const InFingerprint)
		{
			return false;
		}
	};
	
	// Trait version.
	template < typename T >
	struct TComponentOps<T, true, false>
	{
		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Add(FFingerprint* const InFingerprint)
		{
			return InFingerprint->template AddTrait<Paradigm, T>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Remove(FFingerprint* const InFingerprint)
		{
			return InFingerprint->template RemoveTrait<Paradigm, T>();
		}

		static FORCEINLINE bool
		Contains(const FFingerprint* const InFingerprint)
		{
			return InFingerprint->template ContainsTrait<T>();
		}
	};

	// Detail version.
	template < typename D >
	struct TComponentOps<D, false, true>
	{
		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Add(FFingerprint* const InFingerprint)
		{
			return InFingerprint->template AddDetail<Paradigm, D>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Remove(FFingerprint* const InFingerprint)
		{
			return InFingerprint->template RemoveDetail<Paradigm, D>();
		}

		static FORCEINLINE bool
		Contains(const FFingerprint* const InFingerprint)
		{
			return InFingerprint->template ContainsDetail<D>();
		}
	};

	template < EParadigm Paradigm, class T >
	FORCEINLINE auto
	DoAdd()
	{
		return TComponentOps<T>::template Add<Paradigm>(this);
	}


	template < EParadigm Paradigm, class C >
	FORCEINLINE auto
	DoRemove()
	{
		return TComponentOps<C>::template Remove<Paradigm>(this);
	}

	template < typename C >
	FORCEINLINE bool
	DoesContain() const
	{
		return TComponentOps<C>::Contains(this);
	}

  public:

#pragma endregion Component Operations

#pragma region Addition
	/// @name Addition
	/// @{

	/**
	 * Add trait types to the fingerprint.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitTypes The trait types to add.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Add(std::initializer_list<UScriptStruct*> TraitTypes)
	{
		const auto Outcome = Traitmark.template Add<MakePolite(Paradigm)>(TraitTypes);
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Add detail classes to the fingerprint.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClasses The detail classes to add.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Add(std::initializer_list<TSubclassOf<UDetail>> DetailClasses)
	{
		const auto Outcome = Detailmark.template Add<MakePolite(Paradigm)>(DetailClasses);
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Add a fingerprint.
	 * 
	 * @param InFingerprint The fingerprint to add.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Add(const FFingerprint& InFingerprint)
	{
		const auto Outcome = AddToFlagmark<Paradigm>(InFingerprint.GetFlagmark());
		const auto ComponentsOutcome=
			OutcomeCombine(Traitmark.template Add<MakePolite(Paradigm)>(InFingerprint.Traitmark),
						   Detailmark.template Add<MakePolite(Paradigm)>(InFingerprint.Detailmark));
		if (LIKELY(!IsNoop(ComponentsOutcome)))
		{
			HashCache = 0;
			return OutcomeCombine(Outcome, ComponentsOutcome);
		}
		return Outcome;
	}

	/**
	 * Add a fingerprint with a flagmark override.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFingerprint The fingerprint to add.
	 * @param InFlagmark The flagmark to add.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Add(const FFingerprint& InFingerprint,
		const EFlagmark     InFlagmark)
	{
		const auto Outcome = AddToFlagmark<Paradigm>(InFlagmark);
		const auto ComponentsOutcome = 
				StatusCombine(Traitmark.template Add<MakePolite(Paradigm)>(InFingerprint.Traitmark),
							  Detailmark.template Add<MakePolite(Paradigm)>(InFingerprint.Detailmark));
		if (LIKELY(!IsNoop(ComponentsOutcome)))
		{
			HashCache = 0;
			return OutcomeCombine(Outcome, ComponentsOutcome);
		}
		return Outcome;
	}

	/**
	 * Add a trait type and a flagmark to the fingerprint.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to add.
	 * @param InFlagmark The flagmark to add.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Add(UScriptStruct* const TraitType,
		const EFlagmark      InFlagmark = FM_None)
	{
		auto Outcome = ToStatus(AddToFlagmark<Paradigm>(InFlagmark));
		const auto TraitmarkOutcome = Traitmark.template Add<MakePolite(Paradigm)>(TraitType);
		if (LIKELY(!IsNoop(TraitmarkOutcome)))
		{
			HashCache = 0;
			return OutcomeCombine(Outcome, TraitmarkOutcome);
		}
		return Outcome;
	}

	/**
	 * Add a detail class and a flagmark to the fingerprint.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The class of the detail to add.
	 * @param InFlagmark The flagmark to add.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Add(const TSubclassOf<UDetail> DetailClass,
		const EFlagmark            InFlagmark = FM_None)
	{
		const auto Outcome = AddToFlagmark<Paradigm>(InFlagmark);
		if (AvoidError(Paradigm, Outcome))
		{
			return Outcome;
		}
		const auto DetailmarkOutcome = Detailmark.template Add<MakePolite(Paradigm)>(DetailClass);
		if (LIKELY(!IsNoop(DetailmarkOutcome)))
		{
			HashCache = 0;
			return OutcomeCombine(Outcome, DetailmarkOutcome);
		}
		return Outcome;
	}

	/**
	 * Add generic traits and a boot state specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam TraitsExtractorT The traits extractor container type.
	 * @param InTraits The extracted traits to add.
	 * @param InFlagmark The flagmark to add.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename TraitsExtractorT = void >
	FORCEINLINE TOutcome<Paradigm>
	Add(const TraitsExtractorT& InTraits,
		const EFlagmark         InFlagmark = FM_None)
	{
		const auto Outcome = AddToFlagmark<Paradigm>(InFlagmark);
		if (AvoidError(Paradigm, Outcome))
		{
			return Outcome;
		}
		const auto TraitmarkOutcome = Traitmark.template Add<MakePolite(Paradigm)>(InTraits);
		if (LIKELY(TraitmarkOutcome != EApparatusStatus::Noop))
		{
			HashCache = 0;
			return OutcomeCombine(Outcome, TraitmarkOutcome);
		}
		return Outcome;
	}

	/**
	 * Add an array of traits and a flagmark specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the array allocator.
	 * @param InTraitTypes The traits to add.
	 * @param InFlagmark The flagmark to add.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	FORCEINLINE TOutcome<Paradigm>
	Add(const TArray<UScriptStruct*, AllocatorT>& InTraitTypes,
		const EFlagmark                           InFlagmark = FM_None)
	{
		auto Outcome = AddToFlagmark<Paradigm>(InFlagmark);
		const auto TraitmarkOutcome = Traitmark.template Add<MakePolite(Paradigm)>(InTraitTypes);
		if (LIKELY(TraitmarkOutcome != EApparatusStatus::Noop))
		{
			HashCache = 0;
			return OutcomeCombine(Outcome, TraitmarkOutcome);
		}
		return Outcome;
	}

	/**
	 * Add an arrary of detail classes and an optional flagmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of array allocator to use.
	 * @param InDetailClasses The detail classes to add.
	 * @param InFlagmark The flagmark to add.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	FORCEINLINE TOutcome<Paradigm>
	Add(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses,
		const EFlagmark                                 InFlagmark = FM_None)
	{
		const auto Outcome = AddToFlagmark<Paradigm>(InFlagmark);
		const auto DetailmarkOutcome = Detailmark.template Add(InDetailClasses);
		if (LIKELY(!IsNoop(DetailmarkOutcome)))
		{
			HashCache = 0;
			return OutcomeCombine(Outcome, DetailmarkOutcome);
		}
		return Outcome;
	}

	/**
	 * Add an array of details and an optional flagmark.
	 * 
	 * Only the enabled details get actually added.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of array allocator.
	 * @param InDetails The details to add.
	 * Only the active ones get actually added.
	 * @param InFlagmark The flagmark to add.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop Nothing was actually performed.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	FORCEINLINE TOutcome<Paradigm>
	Add(const TArray<UDetail*, AllocatorT>& InDetails,
		const EFlagmark                     InFlagmark = FM_None)
	{
		auto Outcome = AddToFlagmark<Paradigm>(InFlagmark);
		const auto DetailmarkOutcome = Detailmark.template Add<MakePolite(Paradigm)>(InDetails);
		if (LIKELY(!IsNoop(DetailmarkOutcome)))
		{
			HashCache = 0;
			return OutcomeCombine(Outcome, DetailmarkOutcome);
		}
		return Outcome;
	}

	/**
	 * Add a trait type. Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The trait type to add.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm>
	AddTrait()
	{
		const auto Outcome = Traitmark.template Add<MakePolite(Paradigm), T>();
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Add a trait type. Templated version.
	 * 
	 * @tparam T The trait type to add.
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE TOutcome<EParadigm::Default>
	AddTrait()
	{
		return AddTrait<EParadigm::Default, T>();
	}

	/**
	 * Add a detail class. Templated version with a paradigm.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The detail class to add.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcome<Paradigm>
	AddDetail()
	{
		const auto Outcome = Detailmark.template Add<MakePolite(Paradigm), D>();
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Add a detail class. Templated version.
	 * 
	 * @tparam D The detail class to add.
	 * @return The status of the operation.
	 */
	template < class D >
	FORCEINLINE auto
	AddDetail()
	{
		return AddDetail<EParadigm::Default, D>();
	}

	/**
	 * Add component(s) to the fingerprint.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The components to add to the fingerprint.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm>
	Add()
	{
		return OutcomeCombine(DoAdd<Paradigm, Cs>()...);
	}

	/**
	 * Add component(s) to the fingerprint.
	 * Templated version.
	 * 
	 * @tparam Cs The components to add to the fingerprint.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < class... Cs, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Add()
	{
		return Add<Paradigm, Cs...>();
	}

	/**
	 * Add variadic trait types.
	 */
	FORCEINLINE FFingerprint&
	operator+=(std::initializer_list<UScriptStruct*> TraitTypes)
	{
		Add(TraitTypes);
		return *this;
	}

	/**
	 * Add variadic detail classes.
	 */
	FORCEINLINE FFingerprint&
	operator+=(std::initializer_list<TSubclassOf<UDetail>> DetailClasses)
	{
		Add(DetailClasses);
		return *this;
	}

	/**
	 * Add a fingerprint specification.
	 */
	FORCEINLINE FFingerprint&
	operator+=(const FFingerprint& Fingerprint)
	{
		Add(Fingerprint);
		return *this;
	}

	/**
	 * Add an array of trait types.
	 */
	template <typename AllocatorT>
	FORCEINLINE FFingerprint&
	operator+=(const TArray<UScriptStruct*, AllocatorT>& TraitTypes)
	{
		Add(TraitTypes);
		return *this;
	}

	/**
	 * Add an array of detail classes.
	 */
	template <typename AllocatorT>
	FORCEINLINE FFingerprint&
	operator+=(const TArray<TSubclassOf<UDetail>, AllocatorT>& DetailClasses)
	{
		Add(DetailClasses);
		return *this;
	}

	/**
	 * Add active details from an array.
	 */
	template <typename AllocatorT>
	FORCEINLINE FFingerprint&
	operator+=(const TArray<UDetail*, AllocatorT>& InDetails)
	{
		Add(InDetails);
		return *this;
	}

	/**
	 * Add a single trait type.
	 */
	FORCEINLINE FFingerprint&
	operator+=(UScriptStruct* TraitType)
	{
		Add(TraitType);
		return *this;
	}

	/**
	 * Add a single detail class.
	 */
	FORCEINLINE FFingerprint&
	operator+=(TSubclassOf<UDetail> DetailClass)
	{
		Add(DetailClass);
		return *this;
	}

	/// @}
#pragma endregion Addition

#pragma region Removal
	/// @name Removal
	/// @{

	/**
	 * Remove a trait type from the fingerprint specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The trait type to remove.
	 * If @c nullptr, nothing is actually performed.
	 * @returns The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Remove(UScriptStruct* const TraitType)
	{
		const auto Outcome = Traitmark.template Remove<MakePolite(Paradigm)>(TraitType);
		if (LIKELY(Outcome != EApparatusStatus::Noop))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Remove a detail class from the fingerprint specification.
	 *
	 * Even the base-class-matching details get removed. The
	 * result is such that the fingerprint won't contain
	 * that passed detail class anymore.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass A detail class to remove. May be a base class.
	 * If @c nullptr, nothing is actually performed.
	 * @returns The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Remove(const TSubclassOf<UDetail> DetailClass)
	{
		const auto Outcome = Detailmark.template Remove<MakePolite(Paradigm)>(DetailClass);
		if (LIKELY(Outcome != EApparatusStatus::Noop))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Remove a trait type from a fingerprint.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The trait type to remove.
	 * @returns The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTrait()
	{
		const auto Outcome = Traitmark.template Remove<MakePolite(Paradigm), T>();
		if (LIKELY(Outcome != EApparatusStatus::Noop))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Remove a trait type from a fingerprint.
	 * Templated version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The trait type to remove.
	 * @returns The outcome of the operation.
	 */
	template < typename T >
	FORCEINLINE auto
	RemoveTrait()
	{
		return RemoveTrait<EParadigm::Default, T>();
	}

	/**
	 * Remove a detail class from a fingerprint.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The detail class to remove.
	 * @returns The status of the operation.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcome<Paradigm>
	RemoveDetail()
	{
		const auto Outcome = Detailmark.template Remove<MakePolite(Paradigm), D>();
		if (LIKELY(Outcome != EApparatusStatus::Noop))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Remove a detail class from a fingerprint.
	 * Templated version.
	 *
	 * @tparam D The detail class to remove.
	 * @returns The status of the operation.
	 * @see Remove<D>()
	 */
	template < class D >
	FORCEINLINE auto
	RemoveDetail()
	{
		return RemoveDetail<EParadigm::Default, D>();
	}

	/**
	 * Remove component(s) from the fingerprint.
	 * Paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The types/classes of the components to remove.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm>
	Remove()
	{
		return OutcomeCombine(DoRemove<Paradigm, Cs>()...);
	}

	/**
	 * Remove component(s) from the fingerprint.
	 * 
	 * @tparam Cs The types/classes of the components to remove.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < class... Cs, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Remove()
	{
		return Remove<Paradigm, Cs...>();
	}

	/**
	 * Remove all of the traits (if any).
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If there is already no traits in the fingerprint.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveAllTraits()
	{
		const auto Outcome = Traitmark.template Reset<MakePolite(Paradigm)>();
		if (LIKELY(Outcome != EApparatusStatus::Noop))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Remove all of the details (if any).
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If there is already no details in the fingerprint.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveAllDetails()
	{
		const auto Outcome = Detailmark.template Reset<MakePolite(Paradigm)>();
		if (LIKELY(Outcome != EApparatusStatus::Noop))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Remove a trait type from the fingerprint.
	 */
	FORCEINLINE FFingerprint&
	operator-=(UScriptStruct* const TraitType)
	{
		Remove(TraitType);
		return *this;
	}

	/**
	 * Remove a detail class from a fingerprint.
	 */
	FORCEINLINE FFingerprint&
	operator-=(const TSubclassOf<UDetail> DetailClass)
	{
		Remove(DetailClass);
		return *this;
	}

	/// @}
#pragma endregion Removal

#pragma region Search
	/// @name Search
	/// @{

	/**
	 * Check whether a fingerprint contains a trait specification.
	 */
	FORCEINLINE bool
	Contains(const UScriptStruct* const TraitType) const
	{
		return Traitmark.Contains(TraitType);
	}

	/**
	 * Check whether a fingerprint contains a detail specification.
	 */
	FORCEINLINE bool
	Contains(const TSubclassOf<UDetail> DetailClass) const
	{
		return Detailmark.Contains(DetailClass);
	}

	/**
	 * Check whether a fingerprint contains a trait specification.
	 * 
	 * @deprecated since 1.23 The method was renamed. Just use the 'Contains' variant, please.
	 */
	[[deprecated("The method was renamed. Just use the 'Contains' variant, please.")]]
	FORCEINLINE bool
	ContainsTrait(const UScriptStruct* const TraitType) const
	{
		return Traitmark.Contains(TraitType);
	}

	/**
	 * Check whether a fingerprint contains a detail specification.
	 * 
	 * @deprecated since 1.23 The method was renamed. Just use the 'Contains' variant, please.
	 */
	[[deprecated("The method was renamed. Just use the 'Contains' variant, please.")]]
	FORCEINLINE bool
	ContainsDetail(const TSubclassOf<UDetail> DetailClass) const
	{
		return Detailmark.Contains(DetailClass);
	}

	/**
	 * Check whether a fingerprint contains a trait specification. Templated version.
	 * 
	 * @see Contains(const UScriptStruct* const), Contains<T>()
	 */
	template < typename T >
	FORCEINLINE bool
	ContainsTrait() const
	{
		return Traitmark.template Contains<T>();
	}

	/**
	 * Check whether a fingerprint contains a detail specification. Templated version.
	 * 
	 * @see Contains(const TSubclassOf<UDetail>), Contains<D>()
	 */
	template < class D >
	FORCEINLINE bool
	ContainsDetail() const
	{
		return Detailmark.template Contains<D>();
	}

	/**
	 * Check whether a fingerprint contains a component specification.
	 * 
	 * @tparam Cs The types/classes of components to check for.
	 * @return Returns @c true if all of the components are within the
	 * fingerprint.
	 */
	template < typename ...Cs >
	FORCEINLINE bool
	Contains() const
	{
		static_assert(sizeof...(Cs) > 0, "At least one component must be supplied for the testing.");
		const bool Checks[] = { DoesContain<Cs>() ... };
		for (int32 i = 0; i < sizeof...(Cs); ++i)
		{
			if (!Checks[i]) return false;
		}
		return true;
	}

	/// @}
#pragma endregion Search

	/**
	 * Clear the fingerprint without any deallocations.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flagmark to re-apply.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop is nothing was actually changed at all.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Reset(const EFlagmark InFlagmark = FM_None)
	{
		if (IsHarsh(Paradigm)) // Compile-time branch.
		{
			SetFlagmark(InFlagmark);
			RemoveAllTraits();
			RemoveAllDetails();
			return EApparatusStatus::Success;
		}
		return StatusCombine(SetFlagmark<MakePolite(Paradigm)>(InFlagmark),
							 RemoveAllTraits<MakePolite(Paradigm)>(),
							 RemoveAllDetails<MakePolite(Paradigm)>());
	}

	/**
	 * Convert a fingerprint to a string.
	 */
	FString
	ToString() const;

	/**
	 * Calculate the hash sum of the fingerprint.
	 * 
	 * The results are cached internally.
	 * 
	 * This method is thread-safe for solid state
	 * subjects, since their hash won't actually
	 * change.
	 */
	FORCEINLINE uint32
	CalcHash() const
	{
		auto Hash = HashCache.load();
		if (LIKELY(Hash))
		{
			return Hash;
		}
		// The flagmark hashing is not performed here,
		// fo the flagmark operations to be atomic and thread-safe.
		Hash = HashCombine(GetTypeHash(Traitmark),
						   GetTypeHash(Detailmark));
		HashCache.store(Hash);
		return Hash;
	}

#pragma region Serialization

	/**
	 * Serialization operator.
	 */
	friend FArchive&
	operator<<(FArchive& Ar, FFingerprint& InFingerprint)
	{
		if (Ar.IsLoading())
		{
			int32 LoadedFlagmark;
			Ar << LoadedFlagmark;
			InFingerprint.RealFlagmark.store(LoadedFlagmark);
#if WITH_EDITORONLY_DATA
			InFingerprint.Flagmark = LoadedFlagmark;
#endif
		}
		else
		{
			auto SavedFlagmark = InFingerprint.RealFlagmark.load();
#if WITH_EDITORONLY_DATA
			if (SavedFlagmark != InFingerprint.Flagmark)
			{
				if (Ar.IsTransacting())
				{
					SavedFlagmark = InFingerprint.Flagmark; // TODO: Handle the editor-only flagmark manually.
					InFingerprint.RealFlagmark = SavedFlagmark;
				}
			}
#endif
			Ar << SavedFlagmark;
		}

		Ar << InFingerprint.Traitmark;
		Ar << InFingerprint.Detailmark;

		return Ar;
	}

	/**
	 * Serialize the fingerprint to the archive.
	 */
	bool
	Serialize(FArchive& Archive)
	{
		Archive.UsingCustomVersion(FApparatusCustomVersion::GUID);
		const int32 Version = Archive.CustomVer(FApparatusCustomVersion::GUID);
		if (Version < FApparatusCustomVersion::AtomicFlagmarks)
		{
			return false;
		}

		Archive << (*this);

		return true;
	}

	/**
	 * Post-serialize the fingerprint updating its caches.
	 */
	FORCEINLINE void
	PostSerialize(const FArchive& Archive)
	{
		if (Archive.IsLoading())
		{
#if WITH_EDITORONLY_DATA
			// Fetch the real flagmark...
			RealFlagmark = Flagmark;
#endif
			// Reset the cache in case of loading...
			HashCache = 0;
		}
	};

#pragma endregion Serialization

#pragma region Constructors

	/**
	 * Make a fingerprint with components and an optional boot filter.
	 * 
	 * @tparam Ts The types of components to construct with.
	 * @param InFlagmark The flagmark to apply.
	 * @return The resulting fingerprint.
	 */
	template < typename ...Ts >
	static FORCEINLINE FFingerprint
	Make(const EFlagmark InFlagmark = FM_None)
	{
		FFingerprint Fingerprint(InFlagmark);
		Fingerprint.Add<Ts...>();
		return MoveTemp(Fingerprint);
	}

	/**
	 * Construct an empty fingerprint with an optional boot state.
	 */
	FORCEINLINE
	FFingerprint(const int32 InFlagmark = FM_None)
	  :
#if WITH_EDITORONLY_DATA 
	   Flagmark(InFlagmark),
#endif
	   RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a new fingerprint from a single detail class and an optional boot state.
	 */
	FORCEINLINE
	FFingerprint(const TSubclassOf<UDetail> DetailClass,
				 const int32                InFlagmark = FM_None)
	  : Detailmark(DetailClass)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a new fingerprint from a single trait type
	 * and an optional boot state.
	 */
	FORCEINLINE
	FFingerprint(UScriptStruct* const TraitType,
				 const int32          InFlagmark = FM_None)
	  : Traitmark(TraitType)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a fingerprint from an array of details
	 * and an optional boot state.
	 */
	template < typename AllocatorT >
	FORCEINLINE
	FFingerprint(const TArray<UDetail*, AllocatorT>& InDetailClasses,
				 const int32                         InFlagmark = FM_None)
	  : Detailmark(InDetailClasses)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a fingerprint from an array of detail classes
	 * and an optional boot state.
	 */
	template < typename AllocatorT >
	FORCEINLINE
	FFingerprint(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses,
				 const int32                                     InFlagmark = FM_None)
	  : Detailmark(InDetailClasses)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a fingerprint from arrays of detail classes and trait types
	 * and also an optional boot state.
	 */
	template < typename AllocatorTA, typename AllocatorTB >
	FORCEINLINE
	FFingerprint(const TArray<UScriptStruct*, AllocatorTA>&       InTraitTypes,
				 const TArray<TSubclassOf<UDetail>, AllocatorTB>& InDetailClasses,
				 const int32                                      InFlagmark = FM_None)
	  : Traitmark(InTraitTypes)
	  , Detailmark(InDetailClasses)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a fingerprint from initializer lists of trait types and detail classes
	 * and also an optional boot state.
	 */
	FORCEINLINE
	FFingerprint(std::initializer_list<UScriptStruct*>       InTraitTypes,
				 std::initializer_list<TSubclassOf<UDetail>> InDetailClasses,
				 const int32                                 InFlagmark = FM_None)
	  : Traitmark(InTraitTypes)
	  , Detailmark(InDetailClasses)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Move-construct a new fingerprint.
	 */
	FORCEINLINE
	FFingerprint(FFingerprint&& InFingerprint)
	  : Traitmark(MoveTemp(InFingerprint.Traitmark))
	  , Detailmark(MoveTemp(InFingerprint.Detailmark))
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFingerprint.Flagmark)
#endif
	{
		RealFlagmark = InFingerprint.RealFlagmark.load();
	}

	/**
	 * Construct a fingerprint as a copy of another one.
	 */
	FORCEINLINE
	FFingerprint(const FFingerprint& InFingerprint)
	  : Traitmark(InFingerprint.Traitmark)
	  , Detailmark(InFingerprint.Detailmark)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFingerprint.Flagmark)
#endif
	{
		RealFlagmark = InFingerprint.RealFlagmark.load();
	}

	/**
	 * Move-construct from a templated fingerprint.
	 */
	template < typename ...Ts >
	FFingerprint(TFingerprint<Ts...>&& InFingerprint);

	/**
	 * Construct a new fingerprint while moving a traitmark and a flagmark.
	 */
	FORCEINLINE
	FFingerprint(FTraitmark&& InTraitmark,
				 const int32  InFlagmark = FM_None)
	  : Traitmark(InTraitmark)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a new fingerprint while moving a detailmark and a flagmark.
	 */
	FORCEINLINE
	FFingerprint(FDetailmark&& InDetailmark,
				 const int32   InFlagmark = FM_None)
	  : Detailmark(InDetailmark)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a new fingerprint while moving a traitmark and a detailmark.
	 */
	FORCEINLINE
	FFingerprint(FTraitmark&&  InTraitmark,
				 FDetailmark&& InDetailmark,
				 const int32   InFlagmark = FM_None)
	  : Traitmark(InTraitmark)
	  , Detailmark(InDetailmark)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a new fingerprint with a traitmark and a flagmark.
	 */
	FORCEINLINE
	FFingerprint(const FTraitmark& InTraitmark,
				 const int32       InFlagmark = FM_None)
	  : Traitmark(InTraitmark)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a new fingerprint with a detailmark and a flagmark.
	 */
	FORCEINLINE
	FFingerprint(const FDetailmark& InDetailmark,
				 const int32        InFlagmark = FM_None)
	  : Detailmark(InDetailmark)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

	/**
	 * Construct a new fingerprint with a traitmark and a detailmark.
	 */
	FORCEINLINE
	FFingerprint(const FTraitmark&  InTraitmark,
				 const FDetailmark& InDetailmark,
				 const int32        InFlagmark = FM_None)
	  : Traitmark(InTraitmark)
	  , Detailmark(InDetailmark)
#if WITH_EDITORONLY_DATA
	  , Flagmark(InFlagmark)
#endif
	  , RealFlagmark(InFlagmark)
	{}

#pragma endregion Constructors

}; //-struct FFingerprint

FORCEINLINE uint32
GetTypeHash(const FFingerprint& Fingerprint)
{
	return Fingerprint.CalcHash();
}

/**
 * The compile-time traits/details fingerprint.
 * 
 * @tparam Ts The types of components to use.
 */
template < typename ...Ts >
struct TFingerprint : private FFingerprint
{
  private:

	friend struct FFingerprint;

  public:

	/**
	 * Construct the fingerprint with an optional boot state.
	 */
	FORCEINLINE
	TFingerprint(const EFlagmark InFlagmark = FM_None)
	  : FFingerprint(InFlagmark)
	{
		FFingerprint::Add<Ts...>();
	}
};

template < typename ...Ts >
FORCEINLINE
FFingerprint::FFingerprint(TFingerprint<Ts...>&& InFingerprint)
  : Traitmark(MoveTemp(InFingerprint.Traitmark))
  , Detailmark(MoveTemp(InFingerprint.Detailmark))
#if WITH_EDITORONLY_DATA
  , Flagmark(InFingerprint.Flagmark)
#endif
  , RealFlagmark(InFingerprint.RealFlagmark.load())
{}

template<>
struct TStructOpsTypeTraits<FFingerprint> : public TStructOpsTypeTraitsBase2<FFingerprint>
{
	enum 
	{
		WithCopy = true,
		WithIdentical = true,
		WithSerializer = true,
		WithPostSerialize = true
	}; 
};
