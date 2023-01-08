/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Traitmark.h
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

#include <array>
#include "More/type_traits"

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Misc/App.h"
#include "Templates/Casts.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define TRAITMARK_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"
#include "BitMask.h"
#include "TraitsExtractor.h"

#ifdef TRAITMARK_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Traitmark.generated.h"


// Forward declarations:
struct FFlagmarkIndicator;
struct FExclusionIndicator;
struct FFingerprint;
struct FFilter;
struct FCommonSubjectHandle;
struct FScriptStructArray;
struct FTraitRecord;
struct FSubjectRecord;
class ISubjective;
class ISolidSubjective;
class UScriptStruct;
struct FCommonChain;

/**
 * Checks if a type can be considered to be actually a trait.
 */
template< typename T >
class TTraitCandidateChecker
{
	template<typename P, P>
	struct Wrapper;

	template<typename A>
	static constexpr bool Do(Wrapper<UScriptStruct* (*)(void), &A::StaticStruct>*)
	{
		return true;
	}

	template<typename>
	static constexpr bool Do(...)
	{
		return false;
	}

  public:

	static constexpr bool Value = Do<T>(0);
}; //-class TTraitCandidateChecker

/**
 * Check if the supplied type is actually a trait.
 * 
 * @tparam T The type to examine.
 * @return The state of examination.
 */
template < typename T >
constexpr bool IsTraitType()
{
	return (TTraitCandidateChecker<T>::Value) &&
		   (!std::is_base_of<FFingerprint, T>::value) &&
		   (!std::is_base_of<FFilter, T>::value) &&
		   (!std::is_base_of<FTraitRecord, T>::value) &&
		   (!std::is_base_of<FSubjectRecord, T>::value) &&
		   (!std::is_base_of<FCommonSubjectHandle, T>::value) &&
		   (!std::is_base_of<FScriptStructArray, T>::value);
}

/**
 * Secure the trait type.
 * 
 * @tparam T The type to secure as a valid trait.
 */
template < typename T >
using TTraitTypeSecurity = more::enable_if_t<IsTraitType<T>(), bool>;

/**
 * Check if there are any directly references
 * within the list.
 * 
 * This template is used to detect the need
 * for solid operating.
 * 
 * @tparam List The list of types to examine.
 */
template < typename... List >
struct THasDirectTraitAccess;

// Recursive examination:
template < typename T, typename... Tail >
struct THasDirectTraitAccess<T, Tail...>
{
	enum { Value = (IsTraitType<typename more::flatten<T>::type>() 
					&& (std::is_reference<T>::value || std::is_pointer<T>::value))
				|| THasDirectTraitAccess<Tail...>::Value };
};

// Empty list version:
template < >
struct THasDirectTraitAccess<>
{
	enum { Value = false };
};

/**
 * The trait-only fingerprint part.
 */
USTRUCT(BlueprintType, Meta = (HasNativeMake = "ApparatusRuntime.ApparatusFunctionLibrary.MakeTraitmark"))
struct APPARATUSRUNTIME_API FTraitmark
{
	GENERATED_BODY()

  public:

	enum
	{
		/**
		 * Invalid trait identifier.
		 */
		InvalidTraitId = -1,
	};

	/**
	 * The type of the traits array container.
	 */
	typedef TArray<UScriptStruct*> TraitsType;

  private:

	/**
	 * A list of traits.
	 * 
	 * Doesn't contain neither nulls nor duplicates during the runtime.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Traitmark",
			  Meta = (AllowPrivateAccess = "true"))
	TArray<UScriptStruct*> Traits;

	/**
	 * The current traits mask.
	 * 
	 * This is updated according to the traits list.
	 */
	mutable FBitMask TraitsMask;

	friend struct FFingerprint;
	friend struct FFilter;
	friend class UChunk;
	friend struct FSubjectHandle;

	friend class UBelt;
	friend class AMechanism;
	friend class UApparatusFunctionLibrary;

	/**
	 * Get the traits mask of the traitmark.
	 */
	FORCEINLINE FBitMask&
	GetTraitsMaskRef() { return TraitsMask; }

  public:

	/**
	 * Get the total number of registered trait types.
	 */
	static int32
	RegisteredTraitsNum();

	/**
	 * Get a trait's unique identifier.
	 */
	static int32
	GetTraitId(const UScriptStruct* TraitType);

	/**
	 * Get the cached mask for a trait type.
	 */
	static const FBitMask&
	GetTraitMask(const UScriptStruct* TraitType);

	/**
	 * Get the mask of a trait.
	 */
	template <class T>
	FORCEINLINE static const FBitMask&
	GetTraitMask()
	{
		return GetTraitMask(T::StaticStruct());
	}

	/**
	 * Get the traits of the traitmark.
	 */
	FORCEINLINE const TraitsType&
	GetTraits() const
	{
		return Traits;
	}

	/**
	 * Check if the traitmark is empty.
	 */
	FORCEINLINE bool
	IsEmpty() const
	{
		return Traits.Num() == 0;
	}

	/**
	 * The number of traits in the traitmark.
	 */
	FORCEINLINE int32
	TraitsNum() const { return Traits.Num(); }

	/**
	 * Get the traits mask of the traitmark.
	 * Constant version.
	 */
	FORCEINLINE const FBitMask&
	GetTraitsMask() const { return TraitsMask; }

	/**
	 * Get a trait type by its index.
	 */
	FORCEINLINE UScriptStruct*
	TraitAt(const int32 Index) const
	{
		return Traits[Index];
	}

	/**
	 * Get a trait type by its index.
	 */
	FORCEINLINE UScriptStruct*
	operator[](const int32 Index) const
	{
		return TraitAt(Index);
	}

	/**
	 * Check if a traitmark is viable and has any actual effect.
	 */
	FORCEINLINE operator bool() const
	{
		return Traits.Num() > 0;
	}

	/**
	 * Convert to a traits array.
	 */
	FORCEINLINE explicit operator const TraitsType&() const
	{
		return Traits;
	}

  private:

	/**
	 * Generic component adapter
	 * supporting both traits and details compile-time.
	 */
	template < typename T, bool bIsTrait = IsTraitType<T>() >
	struct TComponentOps;
	
	// Non-trait version.
	template < typename T >
	struct TComponentOps<T, /*bIsTrait=*/false>
	{

		static FORCEINLINE constexpr int32
		IndexOf(const FTraitmark* const)
		{
			return INDEX_NONE;
		}

		static FORCEINLINE constexpr bool
		Contains(const FTraitmark* const)
		{
			return false;
		}

	}; //-struct TComponentOps<T, bIsTrait=false>

	// Trait version.
	template < typename T >
	struct TComponentOps<T, /*bIsTrait=*/true>
	{

		static FORCEINLINE int32
		IndexOf(const FTraitmark* const InTraitmark)
		{
			return InTraitmark->IndexOf(T::StaticStruct());
		}

		static FORCEINLINE bool
		Contains(const FTraitmark* const InTraitmark)
		{
			return InTraitmark->Contains(T::StaticStruct());
		}

	}; //-struct TComponentOps<T, bIsTrait=true>

  public:

	/**
	 * Get the index of a specific trait type.
	 * 
	 * @param TraitType The type of the trait to get an index of.
	 * @return The index of the trait.
	 * @return @c INDEX_NONE If there is no such trait within or
	 * @p TraitType is @c nullptr.
	 */
	int32
	IndexOf(const UScriptStruct* TraitType) const
	{
		if (UNLIKELY(TraitType == nullptr))
		{
			return INDEX_NONE;
		}
		const FBitMask& Mask = GetTraitMask(TraitType);
		if (UNLIKELY(!GetTraitsMask().Includes(Mask)))
		{
			return INDEX_NONE;
		}

		for (int32 i = 0; i < Traits.Num(); ++i)
		{
			if (Traits[i] == TraitType) return i;
		}

		checkf(false, TEXT("Trait was not found in the list: %s"),
			   *TraitType->GetName());
		return INDEX_NONE;
	}

	/**
	 * Get the index of a specific trait type. Templated version.
	 * 
	 * @note This method actually supports non-trait types - @c INDEX_NONE
	 * will be returned in such case.
	 *
	 * @tparam T The type of the trait to find.
	 * @return The index of the trait within the traitmark.
	 */
	template < typename T >
	FORCEINLINE constexpr int32
	IndexOf() const
	{
		return TComponentOps<T>::IndexOf(this);
	}

	/**
	 * Get an indexing mapping from another traitmark defined by an array of traits.
	 *
	 * @param[in] InTraitmark A traitmark to get a mapping from as an array of trait types.
	 * @param[out] OutMapping The resulting mapping.
	 */
	template<typename AllocatorTA, typename AllocatorTB>
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const TArray<UScriptStruct*, AllocatorTA>& InTraitmark,
					TArray<int32, AllocatorTB>&                OutMapping) const
	{
		OutMapping.Reset(InTraitmark.Num());
		for (int32 i = 0; i < InTraitmark.Num(); ++i)
		{
			OutMapping.Add(IndexOf(InTraitmark[i]));
		}
		return OutMapping.Num() > 0 ? EApparatusStatus::Success : EApparatusStatus::NoItems;
	}

	/**
	 * Get an indexing mapping from another traitmark defined by a standard array of traits.
	 *
	 * @param[in] InTraitmark A traitmark to get a mapping from as an array of trait types.
	 * @param[out] OutMapping The resulting mapping.
	 */
	template < typename AllocatorTA, int32 Size >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const TArray<UScriptStruct*, AllocatorTA>& InTraitmark,
					std::array<int32, Size>&                   OutMapping) const
	{
		checkf(Size >= InTraitmark.Num(),
			   TEXT("The size of the destination array must be enough to store the mapping."));
		for (int32 i = 0; i < InTraitmark.Num(); ++i)
		{
			OutMapping[i] = IndexOf(InTraitmark[i]);
		}
		return InTraitmark.Num() > 0 ? EApparatusStatus::Success : EApparatusStatus::NoItems;
	}

	/**
	 * Get an indexing mapping from another traitmark's traits.
	 *
	 * @param[in] InTraitmark A traitmark to get a mapping from.
	 * @param[out] OutMapping The resulting traits mapping.
	 */
	template < typename AllocatorT >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const FTraitmark&          InTraitmark,
					TArray<int32, AllocatorT>& OutMapping) const
	{
		return FindMappingFrom(InTraitmark.Traits, OutMapping);
	}

	/**
	 * Get an indexing mapping from another traitmark's traits.
	 *
	 * @tparam Size The size of the output buffer.
	 * @param[in] InTraitmark A traitmark to get a mapping from.
	 * @param[out] OutMapping The resulting traits mapping.
	 */
	template < int32 Size >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const FTraitmark&        InTraitmark,
					std::array<int32, Size>& OutMapping) const
	{
		return FindMappingFrom(InTraitmark.Traits, OutMapping);
	}

	/**
	 * Get an indexing mapping to an another traitmark defined by an array of traits.
	 *
	 * @tparam AllocatorT The allocator of the output mapping.
	 * @param[in] InTraitmark A traitmark to get a mapping to.
	 * @param[out] OutMapping The resulting mapping.
	 */
	template < typename AllocatorT >
	FORCEINLINE EApparatusStatus
	FindMappingTo(const TArray<UScriptStruct*>& InTraitmark,
				  TArray<int32, AllocatorT>&    OutMapping) const
	{
		OutMapping.Reset(Traits.Num());
		for (auto Trait : Traits)
		{
			OutMapping.Add(InTraitmark.IndexOfByKey(Trait));
		}
		return OutMapping.Num() > 0 ? EApparatusStatus::Success : EApparatusStatus::NoItems;
	}

	/**
	 * Get an indexing traits mapping to another traitmark.
	 *
	 * @tparam AllocatorT The type of the output array allocator.
	 * @param[in] InTraitmark A traitmark to get the traits mapping to.
	 * @param[out] OutMapping The resulting mapping.
	 */
	template < typename AllocatorT >
	FORCEINLINE EApparatusStatus
	FindMappingTo(const FTraitmark&          InTraitmark,
				  TArray<int32, AllocatorT>& OutMapping) const
	{
		return InTraitmark.FindMappingFrom(GetTraits(), OutMapping);
	}

	/**
	 * Get an indexing traits mapping to another traitmark.
	 * Standard array version.
	 *
	 * @tparam Size The size of the output array buffer.
	 * @param[in] InTraitmark A traitmark to get the traits mapping to.
	 * @param[out] OutMapping The resulting mapping.
	 */
	template < int32 Size >
	FORCEINLINE EApparatusStatus
	FindMappingTo(const FTraitmark&        InTraitmark,
				  std::array<int32, Size>& OutMapping) const
	{
		return InTraitmark.FindMappingFrom(GetTraits(), OutMapping);
	}

#pragma region Comparison

	/**
	 * Compare two traitmarks for equality.
	 * 
	 * Two traitmarks are considered to be equal
	 * if their traits composition is equal (regardless of the ordering).
	 */
	FORCEINLINE bool
	operator==(const FTraitmark& Other) const
	{
		// Traits bit-masks already have address comparison.
		return GetTraitsMask() == Other.GetTraitsMask();
	}

	/**
	 * Compare two traitmarks for inequality.
	 * 
	 * Two traitmarks are considered to be unequal
	 * if their traits composition is different (regardless of the ordering).
	 * 
	 * @param Other The other traitmark to compare to.
	 * @return The state of inequality.
	 */
	FORCEINLINE bool
	operator!=(const FTraitmark& Other) const
	{
		// Traits bit-masks already have address comparison.
		return GetTraitsMask() != Other.GetTraitsMask();
	}

	/**
	 * Compare two traitmarks for equality. Editor-friendly method.
	 * 
	 * @param Other The other traitmark to compare to.
	 * @param PortFlags The contextual port flags.
	 * @return The state of examination.
	 */
	bool
	Identical(const FTraitmark* Other, uint32 PortFlags) const
	{
		if (this == Other)
		{
			return true;
		}
#if WITH_EDITOR
		if (!FApp::IsGame())
		{
			// Correct support for property editing:
			return Traits == Other->Traits;
		}
#endif
		return (*this) == (*Other);
	}

#pragma endregion Comparison

#pragma region Search

	/**
	 * Check if the traitmark includes a trait type.
	 * 
	 * @param TraitType The trait type to check for.
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	Contains(const UScriptStruct* const TraitType) const
	{
		checkf(TraitType, TEXT("The trait type must be provided for trait checks."));
		const FBitMask& Mask = GetTraitMask(TraitType);
		return GetTraitsMask().Includes(Mask);
	}

	/**
	 * Check if the traitmark includes a trait type.
	 * 
	 * @tparam T The trait type to check for.
	 * @return The state of examination.
	 */
	template < typename T >
	FORCEINLINE bool
	Contains() const
	{
		return TComponentOps<T>::Contains(this);
	}

#pragma endregion Search

#pragma region Matching

	/**
	 * Check if the traitmark matches a filter.
	 */
	bool Matches(const FFilter& Filter) const;

	/**
	 * Check if the traitmark matches another traitmark acting as a filter.
	 */
	FORCEINLINE bool
	Matches(const FTraitmark& Traitmark) const
	{
		return GetTraitsMask().Includes(Traitmark.GetTraitsMask());
	}

#pragma endregion Matching

#pragma region Assignment
	/// @name Assignment
	/// @{

	/**
	 * Set a traitmark to an array of traits.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraits The traits to add.
	 * May contain @c nullptr values which will be ignored
	 * silently.
	 * @return The resulting outcome.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Set(const TArray<UScriptStruct*>& InTraits);

	/**
	 * Move a traitmark to this traitmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmark The traitmark to move.
	 * @return The resulting outcome.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Set(FTraitmark&& InTraitmark)
	{
		if (UNLIKELY(GetTraitsMask() == InTraitmark.GetTraitsMask()))
		{
			return EApparatusStatus::Noop;
		}

		Traits     = MoveTemp(InTraitmark.Traits);
		TraitsMask = MoveTemp(InTraitmark.TraitsMask);		
		return EApparatusStatus::Success;
	}

	/**
	 * Set a traitmark equal to another traitmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmark The traitmark to copy.
	 * @return The resulting outcome.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Set(const FTraitmark& InTraitmark)
	{
		if (UNLIKELY(std::addressof(InTraitmark) == this))
		{
			return EApparatusStatus::Noop;
		}

		const auto Outcome = GetTraitsMaskRef().template Set<MakePolite(Paradigm)>(InTraitmark.GetTraitsMask());
		if (LIKELY(Outcome == EApparatusStatus::Success))
		{
			Traits = InTraitmark.Traits;
		}
		return Outcome;
	}

	/**
	 * Move-assign a traitmark.
	 * 
	 * @param InTraitmark The traitmark to move.
	 * @return Returns itself.
	 */
	FORCEINLINE FTraitmark&
	operator=(FTraitmark&& InTraitmark)
	{
		Set(InTraitmark);
		return *this;
	}

	/**
	 * Set the traitmark equal to another traitmark.
	 * 
	 * @param InTraitmark The traitmark to copy.
	 * @return Returns itself.
	 */
	FORCEINLINE FTraitmark&
	operator=(const FTraitmark& InTraitmark)
	{
		Set(InTraitmark);
		return *this;
	}

	/// @}
#pragma endregion Assignment

#pragma region Addition
	/// @name Addition
	/// @{

	/**
	 * Add trait types to a fingerprint
	 * through an initialization list.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Add(std::initializer_list<UScriptStruct*> InTraits)
	{
		auto Status = EApparatusStatus::Noop;
		for (auto InTrait : InTraits)
		{
			if (UNLIKELY(InTrait == nullptr))
			{
				continue;
			}

			const FBitMask& TraitMask = GetTraitMask(InTrait);
			if (LIKELY(TraitsMask.template Include<MakePolite(Paradigm)>(TraitMask) == EApparatusStatus::Success))
			{
				Traits.Add(InTrait);
				Status = EApparatusStatus::Success;
			}
		}
		return Status;
	}

	/**
	 * Add a traitmark to the traitmark.
	 * 
	 * This is the same as adding a traitmark's
	 * list of traits, but performs additional
	 * optimizing checks.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmark The traitmark to add.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Add(const FTraitmark& InTraitmark)
	{
		if (UNLIKELY(std::addressof(InTraitmark) == this))
		{
			return EApparatusStatus::Noop;
		}
		if (UNLIKELY(GetTraitsMask().Includes(InTraitmark.GetTraitsMask())))
		{
			return EApparatusStatus::Noop;
		}

		return Add<Paradigm>(InTraitmark.Traits);
	}

	/**
	 * Add a trait type.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to add.
	 * May be a @c nullptr.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Add(UScriptStruct* const TraitType)
	{
		if (UNLIKELY(TraitType == nullptr))
		{
			return EApparatusStatus::Noop;
		}

		const FBitMask& Mask = GetTraitMask(TraitType);
		// Check if already is included:
		if (UNLIKELY(TraitsMask.Includes(Mask)))
		{
			return EApparatusStatus::Noop;
		}

		Traits.Add(TraitType);
		TraitsMask.Include(Mask);
		
		return EApparatusStatus::Success;
	}

	/**
	 * Add an array of traits and a boot state specification.
	 * Paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam TraitsExtractorT The traits extractor type to use.
	 * @param InTraits The generic traits provider.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   typename TraitsExtractorT = void >
	TOutcome<Paradigm>
	Add(const TraitsExtractorT& InTraits)
	{
		auto Status = EApparatusStatus::Noop;
		for (int32 i = 0; i < InTraits.Num(); ++i)
		{
			const auto TraitType = InTraits.TypeAt(i);
			if (UNLIKELY(!TraitType))
			{
				continue;
			}

			const FBitMask& TraitMask = GetTraitMask(TraitType);
			// Check if already is included:
			if (LIKELY(TraitsMask.template Include<MakePolite(Paradigm)>(TraitMask) == EApparatusStatus::Success))
			{
				Traits.Add(TraitType);
				Status = EApparatusStatus::Success;
			}
		}
		return Status;
	}

	/**
	 * Add an array of traits and a boot state specification.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The allocator type to use.
	 * @param InTraits The generic traits provider.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   typename AllocatorT = void >
	FORCEINLINE auto
	Add(const TArray<UScriptStruct*, AllocatorT>& InTraits)
	{
		return Add<Paradigm>(TTraitsExtractor<TArray<UScriptStruct*, AllocatorT>>(InTraits));
	}

  private:

	/**
	 * Add a trait type. Trait-compatible version.
	 * 
	 * This is an internal helper method.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to add.
	 * The references are automatically stripped.
	 * @return The status of the operation.
	 * @see Add()
	 */
	template < EParadigm Paradigm, typename T,
			   typename std::enable_if<IsTraitType<typename more::flatten<T>::type>(), int>::type = 0 >
	FORCEINLINE auto
	DoAdd()
	{
		return Add<Paradigm>(more::flatten_t<T>::StaticStruct());
	}

	/**
	 * Add a trait type. Non-trait version.
	 * 
	 * This is an internal helper method.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to add.
	 * The references are automatically stripped.
	 * @return The status of the operation.
	 * @see Add()
	 */
	template < EParadigm Paradigm, typename T,
			   typename std::enable_if<!IsTraitType<typename more::flatten<T>::type>(), int>::type = 0 >
	FORCEINLINE auto
	DoAdd()
	{
		return TOutcome<Paradigm>::Noop();
	}

  public:

	/**
	 * Add trait(s) to the traitmark.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The type(s) of the trait(s) to add.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename... Ts >
	FORCEINLINE TOutcome<Paradigm>
	Add()
	{
		if (sizeof...(Ts) == 0) // Compile-time branch.
		{
			return EApparatusStatus::Noop;
		}
		Traits.Reserve(Traits.Num() + sizeof...(Ts));
		return OutcomeCombine(DoAdd<Paradigm, Ts>()...);
	}

	/**
	 * Add trait(s) to the traitmark.
	 * Templated version.
	 * 
	 * @tparam Ts The type(s) of the trait(s) to add.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < typename... Ts, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Add()
	{
		return Add<Paradigm, Ts...>();
	}

	/**
	 * Add variadic trait types.
	 */
	FORCEINLINE FTraitmark&
	operator+=(std::initializer_list<UScriptStruct*> TraitTypes)
	{
		Add(TraitTypes);
		return *this;
	}

	/**
	 * Add an array of trait types.
	 */
	FORCEINLINE FTraitmark&
	operator+=(const TArray<UScriptStruct*>& TraitTypes)
	{
		Add(TraitTypes);
		return *this;
	}

	/**
	 * Add a single trait type.
	 */
	FORCEINLINE FTraitmark&
	operator+=(UScriptStruct* const TraitType)
	{
		Add(TraitType);
		return *this;
	}

	/// @}
#pragma endregion Addition

#pragma region Removal
	/// @name Removal
	/// @{

	/**
	 * Remove a trait type from the traitmark specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The trait type to remove.
	 * May be a @c nullptr and will be ignored in that case.
	 * @returns The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Remove(UScriptStruct* const TraitType)
	{
		if (UNLIKELY(TraitType == nullptr))
		{
			return EApparatusStatus::Noop;
		}

		// Check if there is actually such trait in the traitmark...
		const FBitMask& TraitMask = GetTraitMask(TraitType);
		if (UNLIKELY(!GetTraitsMask().Includes(TraitMask)))
		{
			return EApparatusStatus::Noop;
		}

		// We can use swapping here,
		// since fingerprints are never reduced within chunks/belts.
		Traits.RemoveSwap(TraitType);
		TraitsMask.SetMasked(TraitMask, false);

		return EApparatusStatus::Success;
	}

  private:

	/**
	 * Remove a type from the traitmark. Internal trait-compatible version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to remove.
	 * @returns The status of the operation.
	 */
	template < EParadigm Paradigm, typename T,
			   typename std::enable_if<IsTraitType<typename more::flatten<T>::type>(), int>::type = 0 >
	FORCEINLINE auto
	DoRemove()
	{		
		return Remove<Paradigm>(more::flatten<T>::type::StaticStruct());
	}

	/**
	 * Remove a type from the traitmark. Internal non-trait version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to remove.
	 * @returns The status of the operation.
	 */
	template < EParadigm Paradigm, typename T,
			   typename std::enable_if<!IsTraitType<typename more::flatten<T>::type>(), int>::type = 0>
	FORCEINLINE auto
	DoRemove()
	{
		return TOutcome<Paradigm>::Noop();
	}

  public:

	/**
	 * Remove trait(s) from the traitmark. Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The type(s) of the trait(s) to remove.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm,
			   typename... Ts >
	FORCEINLINE TOutcome<Paradigm>
	Remove()
	{
		if (sizeof...(Ts) == 0) // Compile-time branch.
		{
			return EApparatusStatus::Noop;
		}
		return OutcomeCombine(DoRemove<Paradigm, Ts>()...);
	}

	/**
	 * Remove trait(s) from the traitmark. Templated version.
	 * 
	 * @tparam Ts The type of the trait(s) to remove.
	 * @return The outcome of the operation.
	 */
	template < typename... Ts,
			   EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Remove()
	{
		return Remove<Paradigm, Ts...>();
	}

	/**
	 * Remove a trait type from the traitmark.
	 */
	FORCEINLINE FTraitmark&
	operator-=(UScriptStruct* const TraitType)
	{
		Remove(TraitType);
		return *this;
	}

	/**
	 * Clear the traitmark without any deallocations.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The resulting outcome.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Reset()
	{
		if (UNLIKELY(Traits.Num() == 0))
		{
			return EApparatusStatus::Noop;
		}
		Traits.Reset();
		TraitsMask.Reset();
		return EApparatusStatus::Success;
	}

	/// @}
#pragma endregion Removal

	/**
	 * Convert a traitmark to a string.
	 */
	FString ToString() const;

	/**
	 * An empty traitmark constant.
	 */
	static const FTraitmark Zero;

#pragma region Serialization

	/**
	 * Serialization operator.
	 */
	friend FORCEINLINE FArchive&
	operator<<(FArchive& Ar, FTraitmark& Traitmark)
	{
		Ar << Traitmark.Traits;
		return Ar;
	}

	/**
	 * Serialize the traitmark to the archive.
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
	 * Post-serialize the traitmark updating the mask.
	 */
	void
	PostSerialize(const FArchive& Archive);

#pragma endregion Serialization

#pragma region Initialization

	/**
	 * Initialize a new traitmark with a list of traits.
	 */
	FORCEINLINE
	FTraitmark()
	  : TraitsMask(RegisteredTraitsNum())
	{}

	/**
	 * Move to a new traitmark.
	 */
	FORCEINLINE
	FTraitmark(FTraitmark&& Traitmark)
	  : Traits(MoveTemp(Traitmark.Traits))
	  , TraitsMask(MoveTemp(Traitmark.TraitsMask))
	{}

	/**
	 * Initialize a new traitmark as a copy of another one.
	 */
	FORCEINLINE
	FTraitmark(const FTraitmark& Traitmark)
	  : Traits(Traitmark.Traits)
	  , TraitsMask(Traitmark.TraitsMask)
	{}

	/**
	 * Initialize a new traitmark with a single trait type.
	 * 
	 * @param TraitType The trait type to initialize with.
	 */
	FTraitmark(UScriptStruct* const TraitType);

	/**
	 * Initialize a new traitmark with an initializer list of traits.
	 */
	FTraitmark(std::initializer_list<UScriptStruct*> InTraits);

	/**
	 * Initialize a new traitmark with an array of traits.
	 */
	template < typename AllocatorT >
	FTraitmark(const TArray<UScriptStruct*, AllocatorT>& InTraits);

	/**
	 * Make a new traitmark with a list of traits types.
	 * 
	 * @tparam Ts The types of traits to fill with.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The resulting traitmark.
	 */
	template < typename... Ts, EParadigm Paradigm = EParadigm::Default >
	static FORCEINLINE FTraitmark
	Make()
	{
		FTraitmark Traitmark;
		verify(OK(Traitmark.template Add<Paradigm, Ts...>()));
		return MoveTemp(Traitmark);
	}

	/**
	 * Make a new traitmark with a list of traits types.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The types of traits to fill with.
	 * @return The resulting traitmark.
	 */
	template < EParadigm Paradigm, typename... Ts >
	static FORCEINLINE FTraitmark
	Make()
	{
		return Make<Ts..., Paradigm>();
	}

#pragma endregion Initialization

}; //-FTraitmark

FORCEINLINE uint32
GetTypeHash(const FTraitmark& Traitmark)
{
	return GetTypeHash(Traitmark.GetTraitsMask());
}

template<>
struct TStructOpsTypeTraits<FTraitmark> : public TStructOpsTypeTraitsBase2<FTraitmark>
{
	enum 
	{
		WithCopy = true,
		WithIdentical = true,
		WithSerializer = true,
		WithPostSerialize = true
	}; 
};
