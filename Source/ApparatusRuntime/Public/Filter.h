/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Filter.h
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

#include "CoreMinimal.h"
#include "UObject/Class.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define FILTER_H_SKIPPED_MACHINE_H
#endif

#include "Fingerprint.h"

#ifdef FILTER_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Filter.generated.h"


// Forward declarations...
template<class ...Cs>
struct TFilter;

/**
 * The base filter class used for indication.
 */
USTRUCT()
struct APPARATUSRUNTIME_API FFilterIndicator
{
	GENERATED_BODY()
};

/**
 * Check if a type is a filter.
 * 
 * @tparam T The type to check.
 * @return The state of examination. 
 */
template < typename T >
constexpr FORCEINLINE bool
IsFilterType()
{
	return std::is_base_of<FFilterIndicator, T>();
}

/**
 * Secure a filter type.
 * 
 * @tparam T The type to secure as a filter.
 */
template < typename T >
using TFilterTypeSecurity = more::enable_if_t<IsFilterType<T>(), bool>;

/**
 * A subject filtering specification.
 * 
 * Example:
 * @code {.cpp}
 * FFilter Filter = FFilter::Make<FHealth, FArmor>();
 * @endcode
 * 
 */
USTRUCT(BlueprintType,
		Meta = (HasNativeMake = "ApparatusRuntime.ApparatusFunctionLibrary.MakeFilter"))
struct APPARATUSRUNTIME_API FFilter : public FFilterIndicator
{
	GENERATED_BODY()

  public:

	/**
	 * The type of traits array.
	 */
	typedef FFingerprint::TraitsType TraitsType;

	/**
	 * The type of details array.
	 */
	typedef FFingerprint::DetailsType DetailsType;

	/**
	 * The default flagmark used in a filter.
	 */
	static constexpr EFlagmark DefaultFlagmark = FM_Booted;

	/**
	 * The default excluding flagmark used in a filter.
	 */
	static constexpr EFlagmark DefaultExcludingFlagmark = FM_Stale;

  private:

	template<class ...Cs>
	friend struct TFilter;

	/**
	 * The included fingerprint.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Filter",
			  Meta = (AllowPrivateAccess = "true"))
	FFingerprint Fingerprint{DefaultFlagmark};

	/**
	 * A list of excluded traits.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Filter",
			  Meta = (AllowPrivateAccess = "true", DisplayAfter="ExcludingFlagmark"))
	TArray<UScriptStruct*> ExcludedTraits;

	/**
	 * The current excluded traits mask.
	 */
	mutable FBitMask ExcludedTraitsMask;

	/**
	 * A list of excluded details.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Filter",
			  Meta = (AllowPrivateAccess = "true", DisplayAfter="ExcludedTraits"))
	TArray<TSubclassOf<UDetail>> ExcludedDetails;

	/**
	 * The current excluded details mask.
	 */
	mutable FBitMask ExcludedDetailsMask;

	/**
	 * The current negative flagmark.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Filter",
			  Meta = (AllowPrivateAccess = "true"))
	int32 ExcludingFlagmark = DefaultExcludingFlagmark;

	/**
	 * The cached hash sum. If 0, needs to be updated.
	 */
	mutable uint32 HashCache = 0;

	/**
	 * Get the hash of a filter.
	 */
	friend uint32 GetTypeHash(const FFilter& Filter);

	friend class UBelt;
	friend class UMachine;
	friend class UApparatusFunctionLibrary;

  public:

#pragma region Flagmarking
	/// @name Flagmarking
	/// @{

	/**
	 * Get the current positive flagmark of the filter.
	 *
	 * @param MemoryOrder The memory order to use for the load
	 * of the filter's flagmark.
	 * @return The current including flagmark of the filter.
	 */
	FORCEINLINE EFlagmark
	GetFlagmark(const std::memory_order MemoryOrder = std::memory_order_relaxed) const
	{
		return Fingerprint.GetFlagmark(MemoryOrder);
	}

	/**
	 * Set the new positive, including flagmark.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The including flagmark to set.
	 * @param MemoryOrder The memory order to use for the store
	 * of the filter's flagmark.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetFlagmark(const EFlagmark         InFlagmark,
				const std::memory_order MemoryOrder = std::memory_order_relaxed)
	{
		const auto Outcome = Fingerprint.template SetFlagmark<MakePolite(Paradigm)>(InFlagmark, MemoryOrder);
		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/// @}
#pragma endregion Flagmarking

	/**
	 * Get the inclusive traitmark of the filter.
	 */
	FORCEINLINE const FTraitmark&
	GetTraitmark() const
	{
		return Fingerprint.GetTraitmark();
	}

	/**
	 * Get the inclusive detailmark of the filter.
	 */
	FORCEINLINE const FDetailmark&
	GetDetailmark() const
	{
		return Fingerprint.GetDetailmark();
	}

	/**
	 * Get the inclusive fingerprint of the filter.
	 */
	FORCEINLINE const FFingerprint&
	GetFingerprint() const
	{
		return Fingerprint;
	}

	/**
	 * Get the included traits of the filter.
	 */
	FORCEINLINE const TraitsType&
	GetTraits() const
	{
		return Fingerprint.GetTraits();
	}

	/**
	 * Get the included details of the filter.
	 */
	FORCEINLINE const DetailsType&
	GetDetails() const
	{
		return Fingerprint.GetDetails();
	}

	/**
	 * Get the excluded traits of the filter.
	 */
	FORCEINLINE const TraitsType&
	GetExcludedTraits() const
	{
		return ExcludedTraits;
	}

	/**
	 * Get the excluded details of the filter.
	 */
	FORCEINLINE const DetailsType&
	GetExcludedDetails() const
	{
		return ExcludedDetails;
	}

	/**
	 * Get the current negative flagmark of the filter.
	 */
	FORCEINLINE EFlagmark
	GetExcludingFlagmark() const
	{
		return (EFlagmark)ExcludingFlagmark;
	}

	/**
	 * Set the new active negative flagmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InExcludingFlagmark The excluding flagmark to set.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetExcludingFlagmark(EFlagmark InExcludingFlagmark)
	{
		if (UNLIKELY(ExcludingFlagmark == (EFlagmarkType)InExcludingFlagmark))
		{
			return EApparatusStatus::Noop;
		}
		ExcludingFlagmark = (EFlagmarkType)InExcludingFlagmark;
		HashCache = 0;
		return EApparatusStatus::Success;
	}

	/**
	 * The number of included traits in the filter.
	 * 
	 * @return The number of traits currently being
	 * included within the filter.
	 */
	FORCEINLINE int32
	TraitsNum() const
	{
		return Fingerprint.TraitsNum();
	}

	/**
	 * The number of included details in the filter.
	 */
	FORCEINLINE int32
	DetailsNum() const { return Fingerprint.DetailsNum(); }

	/**
	 * The number of excluded traits in the filter.
	 */
	FORCEINLINE int32
	ExcludedTraitsNum() const { return ExcludedTraits.Num(); }

	/**
	 * The number of excluded details in the filter.
	 */
	FORCEINLINE int32
	ExcludedDetailsNum() const { return ExcludedDetails.Num(); }

	/**
	 * Check if there is some kind of trait-wise filtering.
	 */
	FORCEINLINE bool
	IsTraitWise() const
	{
		return (TraitsNum() > 0) || (ExcludedTraitsNum() > 0);
	}

	/**
	 * Check if there is some kind of detail-wise filtering.
	 */
	FORCEINLINE bool
	IsDetailWise() const
	{
		return (DetailsNum() > 0) || (ExcludedDetailsNum() > 0);
	}

	/**
	 * Check if the filter requires a belt-wise iterating process.
	 * 
	 * Returns @c true, if there are any details included within the filter.
	 */
	FORCEINLINE bool
	IsBeltBased() const
	{
		return DetailsNum() > 0;
	}

	/**
	 * Get the included traits mask of the filter.
	 */
	FORCEINLINE const FBitMask&
	GetTraitsMask() const { return Fingerprint.GetTraitsMask(); }

	/**
	 * Get the included details mask of the filter.
	 */
	FORCEINLINE const FBitMask&
	GetDetailsMask() const { return Fingerprint.GetDetailsMask(); }

	/**
	 * Get the excluded traits mask of the filter.
	 */
	FORCEINLINE const FBitMask&
	GetExcludedTraitsMask() const
	{
		return ExcludedTraitsMask;
	}

	/**
	 * Get the excluded details mask of the filter.
	 */
	FORCEINLINE const FBitMask&
	GetExcludedDetailsMask() const
	{
		return ExcludedDetailsMask;
	}

	/**
	 * Get an included trait by its index.
	 */
	FORCEINLINE UScriptStruct*
	TraitAt(const int32 Index) const
	{
		return Fingerprint.TraitAt(Index);
	}

	/**
	 * Get an included detail by its index.
	 */
	FORCEINLINE TSubclassOf<UDetail>
	DetailAt(const int32 Index) const
	{
		return Fingerprint.DetailAt(Index);
	}

	/**
	 * Get an excluded trait by its index.
	 */
	FORCEINLINE UScriptStruct*
	ExcludedTraitAt(const int32 Index) const
	{
		return ExcludedTraits[Index];
	}

	/**
	 * Get an excluded detail by its index.
	 */
	FORCEINLINE TSubclassOf<UDetail>
	ExcludedDetailAt(const int32 Index) const
	{
		return ExcludedDetails[Index];
	}

	/**
	 * Find the index of a specific trait type.
	 */
	FORCEINLINE int32
	IndexOf(UScriptStruct* const TraitType) const
	{
		return Fingerprint.IndexOf(TraitType);
	}

	/**
	 * Find the index of a specific detail class.
	 * 
	 * Supports parent classes.
	 */
	FORCEINLINE int32
	IndexOf(const TSubclassOf<UDetail> DetailClass) const
	{
		return Fingerprint.IndexOf(DetailClass);
	}

	/**
	 * Find the indices of a specific detail class.
	 * Supports parent classes.
	 */
	template < typename AllocatorT >
	FORCEINLINE EApparatusStatus
	IndicesOf(const TSubclassOf<UDetail> DetailClass,
			  TArray<int32, AllocatorT>& OutIndices) const
	{
		return Fingerprint.IndicesOf(DetailClass, OutIndices);
	}

#pragma region Assignment

	/**
	 * Move a filter to this one.
	 */
	void Set(FFilter&& Filter);

	/**
	 * Set a filter equal to another filter.
	 */
	void Set(const FFilter& Filter);

	/**
	 * Set a filter to an array of trait types.
	 * 
	 * Preserves the current boot filter.
	 */
	template < typename AllocatorT >
	void Set(const TArray<UScriptStruct*, AllocatorT>& TraitTypes);

	/**
	 * Set a filter to an array of detail classes.
	 * 
	 * Preserves the current boot filter.
	 */
	template < typename AllocatorT >
	void Set(const TArray<TSubclassOf<UDetail>, AllocatorT>& DetailClasses);

	/**
	 * Set a filter to an array of details.
	 * 
	 * Preserves the current boot filter.
	 */
	template < typename AllocatorT >
	void Set(const TArray<UDetail*, AllocatorT>& Details);

	/**
	 * Set a filter equal to another filter.
	 */
	FORCEINLINE FFilter&
	operator=(const FFilter& Filter)
	{
		Set(Filter);
		return *this;
	}

	/**
	 * Move a filter to this one.
	 */
	FORCEINLINE FFilter&
	operator=(FFilter&& Filter)
	{
		Set(Filter);
		return *this;
	}

	/**
	 * Set a filter to an array of trait types.
	 * 
	 * Preserves the current boot filter.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFilter&
	operator=(const TArray<UScriptStruct*, AllocatorT>& TraitTypes)
	{
		Set(TraitTypes);
		return *this;
	}

	/**
	 * Set the filter equal to an array of detail classes.
	 * 
	 * Preserves the current boot filter.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFilter&
	operator=(const TArray<TSubclassOf<UDetail>, AllocatorT>& DetailClasses)
	{
		Set(DetailClasses);
		return *this;
	}

	/**
	 * Set the filter equal to an array of active details.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFilter&
	operator=(const TArray<UDetail*, AllocatorT>& InDetails)
	{
		Set(InDetails);
		return *this;
	}

#pragma endregion Assignment

#pragma region Comparison

	/**
	 * Compare filters for equality.
	 */
	bool
	operator==(const FFilter& Other) const
	{
		if (UNLIKELY(this == std::addressof(Other)))
			return true;

		return (CalcHash() == Other.CalcHash()) &&
			   (Fingerprint == Other.Fingerprint) &&
			   (ExcludingFlagmark == Other.ExcludingFlagmark) &&
			   (GetExcludedTraitsMask() == Other.GetExcludedTraitsMask()) &&
			   (GetExcludedDetailsMask() == Other.GetExcludedDetailsMask());
	}

	/**
	 * Compare filters for inequality.
	 */
	bool
	operator!=(const FFilter& Other) const
	{
		if (UNLIKELY(this == std::addressof(Other)))
			return false;

		return (CalcHash() != Other.CalcHash()) ||
			   (Fingerprint != Other.Fingerprint) ||
			   (ExcludingFlagmark != Other.ExcludingFlagmark) ||
			   (GetExcludedTraitsMask() != Other.GetExcludedTraitsMask()) ||
			   (GetExcludedDetailsMask() != Other.GetExcludedDetailsMask());
	}

	/**
	 * Compare two filters for equality. Editor-friendly method.
	 * 
	 * @param Other The other filter to compare to.
	 * @param PortFlags The contextual port flags.
	 * @return The state of examination.
	 */
	bool
	Identical(const FFilter* Other, uint32 PortFlags) const
	{
		if (UNLIKELY(this == Other))
		{
			return true;
		}
#if WITH_EDITOR
		if (!FApp::IsGame())
		{
			// Correct support for the property editing
			// requires direct arrays comparisons...
			if (ExcludedTraits != Other->ExcludedTraits)
			{
				return false;
			}
			if (ExcludedDetails != Other->ExcludedDetails)
			{
				return false;
			}
		}
#endif
		return (CalcHash() == Other->CalcHash()) &&
			   (Fingerprint.Identical(&Other->Fingerprint, PortFlags)) &&
			   (ExcludingFlagmark == Other->ExcludingFlagmark) && 
			   (GetExcludedTraitsMask() == Other->GetExcludedTraitsMask()) &&
			   (GetExcludedDetailsMask() == Other->GetExcludedDetailsMask());
	}

#pragma endregion Comparison

#pragma region Generic Operations

  private:

	/**
	 * Generic component adapter
	 * supporting both traits and details compile-time.
	 */
	template < typename C,
			   bool IsTrait     = ::IsTraitType<C>(),
			   bool IsDetail    = ::IsDetailClass<C>(),
			   bool IsFlagmark  = std::is_base_of<FFlagmarkIndicator, C>::value,
			   bool IsExclusion = std::is_base_of<FExclusionIndicator, C>::value >
	struct TComponentOps
	{
		template < EParadigm Paradigm >
		static FORCEINLINE constexpr TOutcome<Paradigm>
		Include(FFilter* const)
		{
			return EApparatusStatus::Noop;
		}

		template < EParadigm Paradigm >
		static FORCEINLINE constexpr TOutcome<Paradigm>
		RemoveInclusion(FFilter* const)
		{
			return EApparatusStatus::Noop;
		}

		template < EParadigm Paradigm >
		static FORCEINLINE constexpr TOutcome<Paradigm>
		Exclude(FFilter* const)
		{
			return EApparatusStatus::Noop;
		}

		template < EParadigm Paradigm >
		static FORCEINLINE constexpr EApparatusStatus
		RemoveExclusion(FFilter* const)
		{
			return EApparatusStatus::Noop;
		}

		static FORCEINLINE constexpr bool
		Includes(const FFilter* const)
		{
			return false;
		}

		static FORCEINLINE constexpr bool
		Excludes(const FFilter* const)
		{
			return false;
		}
	};
	
	// Trait version.
	template < typename T >
	struct TComponentOps<T, true, false, false, false>
	{
		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Include(FFilter* const InFilter)
		{
			return InFilter->template IncludeTrait<Paradigm, T>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		RemoveInclusion(FFilter* const InFilter)
		{
			return InFilter->template RemoveTraitInclusion<Paradigm, T>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Exclude(FFilter* const InFilter)
		{
			return InFilter->template ExcludeTrait<Paradigm, T>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		RemoveExclusion(FFilter* const InFilter)
		{
			return InFilter->template RemoveTraitExclusion<Paradigm, T>();
		}

		static FORCEINLINE bool
		Includes(const FFilter* const InFilter)
		{
			return InFilter->template IncludesTrait<T>();
		}

		static FORCEINLINE bool
		Excludes(const FFilter* const InFilter)
		{
			return InFilter->template ExcludesTrait<T>();
		}

	}; //-struct TComponentOps<T, true, false, false>

	// Detail version.
	template < class D >
	struct TComponentOps<D, false, true, false, false>
	{
		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Include(FFilter* const InFilter)
		{
			return InFilter->template IncludeDetail<Paradigm, D>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		RemoveInclusion(FFilter* const InFilter)
		{
			return InFilter->template RemoveDetailInclusion<Paradigm, D>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Exclude(FFilter* const InFilter)
		{
			return InFilter->template ExcludeDetail<Paradigm, D>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		RemoveExclusion(FFilter* const InFilter)
		{
			return InFilter->template RemoveDetailExclusion<Paradigm, D>();
		}

		static FORCEINLINE bool
		Includes(const FFilter* const InFilter)
		{
			return InFilter->template IncludesDetail<D>();
		}

		static FORCEINLINE bool
		Excludes(const FFilter* const InFilter)
		{
			return InFilter->template ExcludesDetail<D>();
		}

	}; //-struct TComponentOps<D, false, true, false>

	// Flagmark version.
	template < class F >
	struct TComponentOps<F, false, false, true, false>
	{
		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Include(FFilter* const InFilter)
		{
			return InFilter->template Include<Paradigm>(F::Value);
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		RemoveInclusion(FFilter* const InFilter)
		{
			return InFilter->template RemoveInclusion<Paradigm>(F::Value);
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Exclude(FFilter* const InFilter)
		{
			return InFilter->template Exclude<Paradigm>(F::Value);
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		RemoveExclusion(FFilter* const InFilter)
		{
			return InFilter->template RemoveExclusion<Paradigm>(F::Value);
		}

		static FORCEINLINE bool
		Includes(const FFilter* const InFilter)
		{
			return InFilter->Includes(F::Value);
		}

		static FORCEINLINE bool
		Excludes(const FFilter* const InFilter)
		{
			return InFilter->Excludes(F::Value);
		}

	}; //-struct TComponentOps<F, false, false, true, false>

	// Excluding version.
	template < class E >
	struct TComponentOps<E, false, false, false, true>
	{
		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Include(FFilter* const InFilter)
		{
			return InFilter->template Exclude<Paradigm, typename E::Type>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		RemoveInclusion(FFilter* const InFilter)
		{
			return InFilter->template RemoveExclusion<Paradigm, typename E::Type>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		Exclude(FFilter* const InFilter)
		{
			return InFilter->template Include<Paradigm, typename E::Type>();
		}

		template < EParadigm Paradigm >
		static FORCEINLINE TOutcome<Paradigm>
		RemoveExclusion(FFilter* const InFilter)
		{
			return InFilter->template RemoveInclusion<Paradigm, typename E::Type>();
		}

		static FORCEINLINE bool
		Includes(const FFilter* const InFilter)
		{
			return InFilter->Excludes<typename E::Type>();
		}

		static FORCEINLINE bool
		Excludes(const FFilter* const InFilter)
		{
			return InFilter->Includes<typename E::Type>();
		}

	}; //-struct TComponentOps<F, false, false, false, true>

	/**
	 * @internal Include a single component
	 * to the filter. Internal method.
	 */
	template < EParadigm Paradigm, class C >
	FORCEINLINE TOutcome<Paradigm>
	DoInclude()
	{
		return TComponentOps<C>::Include<Paradigm>(this);
	}

	/**
	 * @internal Remove a single component inclusion
	 * from the filter. Internal method.
	 */
	template <EParadigm Paradigm,  class C >
	FORCEINLINE TOutcome<Paradigm>
	DoRemoveInclusion()
	{
		return TComponentOps<C>::RemoveInclusion<Paradigm>(this);
	}

	/**
	 * @internal Exclude a single component
	 * within the filter. Internal method.
	 */
	template < EParadigm Paradigm, class C >
	FORCEINLINE TOutcome<Paradigm>
	DoExclude()
	{
		return TComponentOps<C>::Exclude<Paradigm>(this);
	}

	/**
	 * @internal Remove a single component exclusion
	 * from the filter. Internal method.
	 */
	template <EParadigm Paradigm,  class C >
	FORCEINLINE TOutcome<Paradigm>
	DoRemoveExclusion()
	{
		return TComponentOps<C>::RemoveExclusion<Paradigm>(this);
	}

	/**
	 * @internal Check if the filter includes a single component.
	 * Internal method.
	 */
	template < class C >
	FORCEINLINE bool
	DoesInclude() const
	{
		return TComponentOps<C>::Includes(this);
	}

	/**
	 * @internal Check if the filter excludes a single component.
	 * Internal method.
	 */
	template < class C >
	FORCEINLINE bool
	DoesExclude() const
	{
		return TComponentOps<C>::Excludes(this);
	}

#pragma endregion Generic Operations

  public:

#pragma region Inclusion
	/// @name Inclusion
	/// @{

	/**
	 * Add a flag as an including one.
	 * 
	 * Removes the flag from the excluding one as necessary.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InBit The flagmark bit to include in to the filtering.
	 * @return Returns itself for chaining.
	 *
	 * @see Include(const EFlagmarkBit)
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	IncludeFlag(const EFlagmarkBit InBit)
	{
		return MakeOutcome<Paradigm, FFilter&>(
			OutcomeCombine(
				SetExcludingFlagmark<Paradigm>(GetExcludingFlagmark() - InBit),
				SetFlagmark<Paradigm>(GetFlagmark() + InBit)),
			*this);
	}

	/**
	 * Add a flagmark as an including one.
	 * 
	 * Removes the flagmark from the excluding filtering as necessary.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flagmark to include in to the filtering.
	 * @return Returns itself for chaining.
	 *
	 * @see Include(const EFlagmark)
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	IncludeFlagmark(const EFlagmark InFlagmark)
	{
		return MakeOutcome<Paradigm, FFilter&>(
			OutcomeCombine(
				SetExcludingFlagmark<Paradigm>(GetExcludingFlagmark() - InFlagmark),
				SetFlagmark<Paradigm>(GetFlagmark() | InFlagmark)),
			*this);
	}

	/**
	 * Add an array of trait types as inclusions.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The allocator to use for the traits types array.
	 * @param TraitTypes The trait types to include.
	 * @return Returns itself for the purpose of chaining.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(const TArray<UScriptStruct*, AllocatorT>& TraitTypes)
	{
		const auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(TraitTypes);
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			HashCache = 0;
			check(!Fingerprint.GetTraitsMask().IncludesPartially(GetExcludedTraitsMask()));
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Add an arrary of detail classes as inclusions.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The allocator to use for the details classes array.
	 * @param DetailClasses The detail classes to include.
	 * @return Returns itself for the purpose of chaining.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(const TArray<TSubclassOf<UDetail>, AllocatorT>& DetailClasses)
	{
		const auto Status =  Fingerprint.template Add<MakePolite(Paradigm)>(DetailClasses);
		if (Status == EApparatusStatus::Success)
		{
			HashCache = 0;
			check(!Fingerprint.GetDetailsMask().IncludesPartially(GetExcludedDetailsMask()));
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Add a variadic list of trait types as inclusions.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitTypes The trait types to add
	 * as a variadic list.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(std::initializer_list<UScriptStruct*> InTraitTypes)
	{
		const auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(InTraitTypes);
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			HashCache = 0;
			check(!Fingerprint.GetTraitsMask().IncludesPartially(GetExcludedTraitsMask()));
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Add a variadic list of detail classes as inclusions.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailClasses The initializer list of detail classes to include.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(std::initializer_list<TSubclassOf<UDetail>> InDetailClasses)
	{
		const auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(InDetailClasses);
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			HashCache = 0;
			check(!Fingerprint.GetDetailsMask().IncludesPartially(GetExcludedDetailsMask()));
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Add an array of active details as inclusions.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The allocator of the array.
	 * @param InDetails The details to include.
	 * Only the active details get included.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(const TArray<UDetail*, AllocatorT>& InDetails)
	{
		const auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(InDetails);
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			HashCache = 0;
			check(!Fingerprint.GetDetailsMask().IncludesPartially(GetExcludedDetailsMask()));
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Include a fingerprint within the filter.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFingerprint The fingerprint to include.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(const FFingerprint& InFingerprint)
	{
		const auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(InFingerprint);
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			HashCache = 0;
			check(!Fingerprint.GetTraitsMask().IncludesPartially(GetExcludedTraitsMask()));
			check(!Fingerprint.GetDetailsMask().IncludesPartially(GetExcludedDetailsMask()));
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Add a trait type.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType A type of the trait to add.
	 * May be a @c nullptr and is skipped in such case.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(UScriptStruct* const TraitType)
	{
		const auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(TraitType);
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			HashCache = 0;
			check(!Fingerprint.GetTraitsMask().IncludesPartially(GetExcludedTraitsMask()));
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Add a detail class.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass A class of the detail to add.
	 * May be a @c nullptr and is skipped in such case.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(const TSubclassOf<UDetail> DetailClass)
	{
		const auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(DetailClass);
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			HashCache = 0;
			check(!Fingerprint.GetDetailsMask().IncludesPartially(GetExcludedDetailsMask()));
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Include a trait type within the filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to include.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	IncludeTrait()
	{
		return Include<Paradigm>(T::StaticStruct());
	}

	/**
	 * Include a trait type within the filter.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to include.
	 * @return The outcome of the operation.
	 */
	template < typename T >
	FORCEINLINE auto
	IncludeTrait()
	{
		return Include<EParadigm::Default, T>();
	}

	/**
	 * Include a detail class within the filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to include.
	 * @return The outcome of the operation.
	 * 
	 * @see Include()
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	IncludeDetail()
	{
		return Include<Paradigm>(D::StaticClass());
	}

	/**
	 * Include a detail class within the filter.
	 * Templated version.
	 * 
	 * @tparam D The class of the detail to include.
	 * @return The outcome of the operation.
	 * 
	 * @see Include()
	 */
	template < class D >
	FORCEINLINE auto
	IncludeDetail()
	{
		return Include<EParadigm::Default, D>();
	}

	/**
	 * Include component(s) within a filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The components to include.
	 * @return Returns itself for the purpose of chaining.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include()
	{
		if (sizeof...(Cs) == 0) // Compile-time branch.
		{
			return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Noop, *this);
		}
		return MakeOutcome<Paradigm, FFilter&>(OutcomeCombine(DoInclude<Paradigm, Cs>()...), *this);
	}

	/**
	 * Include component(s) within a filter.
	 * Templated version.
	 * 
	 * @tparam Cs The components to include.
	 * @return The outcome of the operation.
	 */
	template < class... Cs >
	FORCEINLINE auto
	Include()
	{
		return Include<EParadigm::Default, Cs...>();
	}

	/**
	 * Include component(s) alongside a flag within the filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The components to include.
	 * @param Flag The flagmark bit to also include
	 * within the filter.
	 * @return Returns itself for chaining.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(const EFlagmarkBit Flag)
	{
		return MakeOutcome<Paradigm, FFilter&>(
			OutcomeCombine(
				IncludeFlag<Paradigm>(Flag),
				Include<Paradigm, Cs...>()),
			*this);
	}

	/**
	 * Include component(s) alongside a flag within the filter.
	 * Templated version.
	 * 
	 * @tparam Cs The components to include.
	 * @param Flag The flagmark bit to also include
	 * within the filter.
	 * @return Returns itself for chaining.
	 */
	template < class... Cs >
	FORCEINLINE auto
	Include(const EFlagmarkBit Flag)
	{
		return Include<EParadigm::Default, Cs...>(Flag);
	}

	/**
	 * Include component(s) alongside a flagmark within the filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The components to include.
	 * @param Flagmark The flagmark bit to also include
	 * within the filter.
	 * @return Returns itself for chaining.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Include(const EFlagmark Flagmark)
	{
		return MakeOutcome<Paradigm, FFilter&>(
			OutcomeCombine(
				IncludeFlagmark<Paradigm>(Flagmark),
				Include<Paradigm, Cs...>()),
			*this);
	}

	/**
	 * Include component(s) alongside a flagmark within the filter.
	 * Templated version.
	 * 
	 * @tparam Cs The components to include.
	 * @param Flagmark The flagmark bit to also include
	 * within the filter.
	 * @return Returns itself for chaining.
	 */
	template < class... Cs >
	FORCEINLINE auto
	Include(const EFlagmark Flagmark)
	{
		return Include<EParadigm::Default, Cs...>(Flagmark);
	}

	/**
	 * Remove a flagmark inclusion from the filter specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark A flagmark to remove from being included.
	 * @returns The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveInclusion(const EFlagmark InFlagmark)
	{
		const auto Status = Fingerprint.template RemoveFromFlagmark<MakePolite(Paradigm)>(InFlagmark);
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			HashCache = 0;
		}
		return Status;
	}

	/**
	 * Remove a trait type from the filter specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType A trait type to remove.
	 * If @c nullptr, nothing is performed.
	 * @returns The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveInclusion(UScriptStruct* const TraitType)
	{
		const auto Outcome = Fingerprint.template Remove<MakePolite(Paradigm)>(TraitType);
		if (LIKELY(Outcome == EApparatusStatus::Success))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Remove a detail class from the filter specification.
	 *
	 * @note Even the base matching details will be removed. The
	 * result is that the resulting filter won't contain
	 * that passed detail class.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass A detail class to remove. May be a base class.
	 * If @c nullptr, nothing is performed.
	 * @returns The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveInclusion(const TSubclassOf<UDetail> DetailClass)
	{
		const auto Outcome = Fingerprint.template Remove<MakePolite(Paradigm)>(DetailClass);
		if (LIKELY(Outcome == EApparatusStatus::Success))
		{
			HashCache = 0;
		}
		return Outcome;
	}

	/**
	 * Remove an included trait from the filter.
	 * Template paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait inclusion to remove.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTraitInclusion()
	{
		return RemoveInclusion<Paradigm>(T::StaticStruct());
	}

	/**
	 * Remove an included trait from the filter.
	 * Template version.
	 *
	 * @tparam T The type of the trait inclusion to remove.
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE auto
	RemoveTraitInclusion()
	{
		return RemoveInclusion<EParadigm::Default, T>();
	}

	/**
	 * Remove an included detail from the filter.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail inclusion to remove.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcome<Paradigm>
	RemoveDetailInclusion()
	{
		return RemoveInclusion<Paradigm>(D::StaticClass());
	}

	/**
	 * Remove an included detail from the filter.
	 * Templated version.
	 *
	 * @tparam D The class of the detail inclusion to remove.
	 * @return The status of the operation.
	 */
	template < class D >
	FORCEINLINE auto
	RemoveDetailInclusion()
	{
		return RemoveInclusion<EParadigm::Default, D>();
	}

	/**
	 * Remove the component(s) from the filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The types or classes of the components to remove.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm>
	RemoveInclusion()
	{
		return OutcomeCombine(DoRemoveInclusion<Paradigm, Cs>()...);
	}

	/**
	 * Remove the component(s) from the filter.
	 * Templated version.
	 * 
	 * @tparam Cs The types or classes of the components to remove.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < class... Cs >
	FORCEINLINE auto
	RemoveInclusion()
	{
		return RemoveInclusion<EParadigm::Default, Cs...>();
	}

	/**
	 * Include variadic trait types.
	 */
	FORCEINLINE FFilter&
	operator+=(std::initializer_list<UScriptStruct*> InTraitTypes)
	{
		Include(InTraitTypes);
		return *this;
	}

	/**
	 * Include variadic detail classes.
	 */
	FORCEINLINE FFilter&
	operator+=(std::initializer_list<TSubclassOf<UDetail>> InDetailClasses)
	{
		Include(InDetailClasses);
		return *this;
	}

	/**
	 * Include a fingerprint within the filter.
	 */
	FORCEINLINE FFilter&
	operator+=(const FFingerprint& InFingerprint)
	{
		Include(InFingerprint);
		return *this;
	}

	/**
	 * Add an array of trait types.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFilter&
	operator+=(const TArray<UScriptStruct*, AllocatorT>& InTraitTypes)
	{
		Include(InTraitTypes);
		return *this;
	}

	/**
	 * Add an array of detail classes.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFilter&
	operator+=(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses)
	{
		Include(InDetailClasses);
		return *this;
	}

	/**
	 * Add active details from an array.
	 */
	template < typename AllocatorT >
	FORCEINLINE FFilter&
	operator+=(const TArray<UDetail*, AllocatorT>& InDetails)
	{
		Include(InDetails);
		return *this;
	}

	/**
	 * Add a single trait type to the list of included traits.
	 */
	FORCEINLINE FFilter&
	operator+=(UScriptStruct* const TraitType)
	{
		Include(TraitType);
		return *this;
	}

	/**
	 * Add a single detail class of included details.
	 */
	FORCEINLINE FFilter&
	operator+=(const TSubclassOf<UDetail> DetailClass)
	{
		Include(DetailClass);
		return *this;
	}

	/// @}
#pragma endregion Inclusion

#pragma region Exclusion
	/// @name Exclusion
	/// @{

	/**
	 * Add a flag as an excluding one.
	 * Status version.
	 * 
	 * Removes the flag from the positive flagmark accordingly.
	 * 
	 * @param[in] InBit The bit to exclude.
	 * @param[out] OutStatus The status of the operation.
	 * @return Returns itself to be used in a one-liner.
	 */
	[[deprecated("The status version is deprecated. Use the normal one within a polite paradigm.")]]
	FORCEINLINE FFilter&
	Exclude(const EFlagmarkBit InBit,
			EApparatusStatus&  OutStatus)
	{
		StatusAccumulate(
			OutStatus,
			SetFlagmark<EParadigm::Polite>(GetFlagmark() - InBit),
			SetExcludingFlagmark<EParadigm::Polite>(GetExcludingFlagmark() + InBit));
		return *this;
	}

	/**
	 * Add a flag as an excluding one.
	 * 
	 * Removes the flag from the positive flagmark accordingly.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return Returns itself to be used in a one-liner.
	 *
	 * @see Exclude(const EFlagmarkBit)
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	ExcludeFlag(const EFlagmarkBit InBit)
	{
		return MakeOutcome<Paradigm, FFilter&>(
			OutcomeCombine(
				SetFlagmark<Paradigm>(GetFlagmark() - InBit),
				SetExcludingFlagmark<Paradigm>(GetExcludingFlagmark() + InBit)),
			*this);
	}

	/**
	 * Add a flagmark as an excluding one.
	 * Status version.
	 * 
	 * Removes the flagmark from the positive one accordingly.
	 * 
	 * @param InExcludingFlagmark The flagmark to exclude.
	 * @param OutStatus The status of the operation.
	 * @return Returns itself to be used in a one-liner.
	 */
	[[deprecated("The status version is deprecated. Use the normal one within a polite paradigm.")]]
	FORCEINLINE FFilter&
	Exclude(const EFlagmark   InExcludingFlagmark,
			EApparatusStatus& OutStatus)
	{
		StatusAccumulate(
			OutStatus,
			SetFlagmark<EParadigm::Polite>(GetFlagmark() - InExcludingFlagmark),
			SetExcludingFlagmark<EParadigm::Polite>(GetExcludingFlagmark() | InExcludingFlagmark));
		return *this;
	}

	/**
	 * Add a flagmark as an excluding one.
	 * 
	 * Removes the flagmark from the positive one accordingly.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InExcludingFlagmark The flagmark to exclude.
	 * @return Returns itself to be used in a one-liner.
	 * 
	 * @see Exclude(const EFlagmark)
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	ExcludeFlagmark(const EFlagmark InExcludingFlagmark)
	{
		return MakeOutcome<Paradigm, FFilter&>(
			OutcomeCombine(
				SetFlagmark<Paradigm>(GetFlagmark() - InExcludingFlagmark),
				SetExcludingFlagmark<Paradigm>(GetExcludingFlagmark() | InExcludingFlagmark)),
			*this);
	}

	/**
	 * Exclude a trait type from being matched.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InExcludedTraitType The trait type to exclude.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Exclude(UScriptStruct* const InExcludedTraitType)
	{
		if (UNLIKELY(InExcludedTraitType == nullptr))
		{
			return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Noop, *this);
		}

		const FBitMask& Mask = FTraitmark::GetExcludingTraitMask(InExcludedTraitType);
		if (UNLIKELY(Fingerprint.GetTraitsMask().Includes(Mask)))
		{
			return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Conflict, *this);
		}
		if (UNLIKELY(ExcludedTraitsMask.Includes(Mask)))
		{
			return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Noop, *this);
		}
		ExcludedTraitsMask.Include(Mask);
		ExcludedTraits.Add(InExcludedTraitType);
		HashCache = 0;

		return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Success, *this);
	}

	/**
	 * Exclude a detail class from being matched.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InExcludedDetailClass The detail class to exclude.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Exclude(const TSubclassOf<UDetail> InExcludedDetailClass)
	{
		if (UNLIKELY(InExcludedDetailClass == nullptr))
		{
			return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Noop, *this);
		}

		const FBitMask& Mask = FDetailmark::GetExcludingDetailMask(InExcludedDetailClass);
		if (UNLIKELY(Fingerprint.GetDetailsMask().Includes(Mask)))
		{
			return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Conflict, *this);
		}
		if (UNLIKELY(ExcludedDetailsMask.Includes(Mask)))
		{
			return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Noop, *this);
		}
		ExcludedDetailsMask.Include(Mask);
		ExcludedDetails.Add(InExcludedDetailClass);
		HashCache = 0;

		return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Success, *this);
	}

	/**
	 * Exclude a trait type from being matched.
	 * Template paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The trait type to exclude.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	ExcludeTrait()
	{
		return Exclude<Paradigm>(T::StaticStruct());
	}

	/**
	 * Exclude a trait type from being matched.
	 * Template version.
	 * 
	 * @tparam T The trait type to exclude.
	 * @return The outcome of the operation.
	 */
	template < typename T >
	FORCEINLINE auto
	ExcludeTrait()
	{
		return Exclude<EParadigm::Default, T>();
	}

	/**
	 * Exclude a detail class from filtering.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to exclude.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	ExcludeDetail()
	{
		return Exclude<Paradigm>(D::StaticClass());
	}

	/**
	 * Exclude a detail class from filtering.
	 * Templated version.
	 * 
	 * @tparam D The class of the detail to exclude.
	 * @return The outcome of the operation.
	 */
	template < class D >
	FORCEINLINE auto
	ExcludeDetail()
	{
		return Exclude<EParadigm::Default, D>();
	}

	/**
	 * Exclude component(s) within the filter.
	 * Templated status version.
	 *
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @param OutStatus The status of the operation.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < class... Cs >
	[[deprecated("The status version is deprecated. Use the normal one within a polite paradigm.")]]
	FORCEINLINE FFilter&
	Exclude(EApparatusStatus& OutStatus)
	{
		const EApparatusStatus Statuses[] = { EApparatusStatus::Noop, DoExclude<Cs>() ... };
		StatusAccumulate(OutStatus, Statuses);
		return *this;
	}

	/**
	 * Exclude component(s) within the filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Exclude()
	{
		if (sizeof...(Cs) == 0) // Compile-time branch.
		{
			return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Noop, *this);
		}
		return MakeOutcome<Paradigm, FFilter&>(OutcomeCombine(DoExclude<Paradigm, Cs>()...), *this);
	}

	/**
	 * Exclude component(s) and a flagmark within the filter.
	 * Templated default paradigm version.
	 *
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @tparam Paradigm The paradigm to work under.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < class... Cs, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Exclude()
	{
		return Exclude<Paradigm, Cs...>();
	}

	/**
	 * Exclude component(s) and a flagmark within the filter.
	 * Templated status version.
	 *
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @param InExcludingFlagmark The flagmark to exclude.
	 * @param OutStatus The status of the operation.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < class... Cs >
	[[deprecated("The status version is deprecated. Use the normal one within a polite paradigm.")]]
	FORCEINLINE FFilter&
	Exclude(const EFlagmark   InExcludingFlagmark,
			EApparatusStatus& OutStatus)
	{
		OutStatus = Exclude<EParadigm::SafePolite, Cs...>(OutStatus);
		StatusAccumulate(OutStatus, Exclude<EParadigm::SafePolite>(InExcludingFlagmark));
		return *this;
	}

	/**
	 * Exclude component(s) and a flagmark within the filter.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @param InExcludingFlagmark The flagmark to exclude.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Exclude(const EFlagmark InExcludingFlagmark)
	{
		return MakeOutcome<Paradigm, FFilter&>(
			OutcomeCombine(
				Exclude<Paradigm, Cs...>(),
				ExcludeFlagmark<Paradigm>(InExcludingFlagmark)),
			*this);
	}

	/**
	 * Exclude component(s) and a flagmark within the filter.
	 * Templated default paradigm version.
	 *
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @tparam Paradigm The paradigm to work under.
	 * @param InExcludingFlagmark The flagmark to exclude.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < class... Cs, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Exclude(const EFlagmark InExcludingFlagmark)
	{
		return Exclude<Paradigm, Cs...>(InExcludingFlagmark);
	}

	/**
	 * Exclude component(s) and a flagmark bit within the filter.
	 * Templated status version.
	 *
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @param InExcludedFlagmarkBit The flagmark bit to exclude.
	 * @param OutStatus The status of the operation.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < class... Cs >
	[[deprecated("The status version is deprecated. Use the normal one within a polite paradigm.")]]
	FORCEINLINE FFilter&
	Exclude(const EFlagmarkBit InExcludedFlagmarkBit,
			EApparatusStatus&  OutStatus)
	{
		OutStatus = Exclude<EParadigm::SafePolite, Cs...>();
		StatusAccumulate(OutStatus, Exclude<EParadigm::SafePolite>(InExcludedFlagmarkBit));
		return *this;
	}

	/**
	 * Exclude component(s) and a flagmark bit within the filter.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @param InExcludedFlagmarkBit The flagmark bit to exclude.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	Exclude(const EFlagmarkBit InExcludedFlagmarkBit)
	{
		return MakeOutcome<Paradigm, FFilter&>(
			OutcomeCombine(
				Exclude<Paradigm, Cs...>(),
				ExcludeFlag<Paradigm>(InExcludedFlagmarkBit)),
			*this);
	}

	/**
	 * Exclude component(s) and a flagmark bit within the filter.
	 * Templated version.
	 *
	 * @tparam Cs The components to exclude.
	 * May be both traits and details.
	 * @param InExcludedFlagmarkBit The flagmark bit to exclude.
	 * @return Returns itself to be used in a one-liner.
	 */
	template < class... Cs, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Exclude(const EFlagmarkBit InExcludedFlagmarkBit)
	{
		return Exclude<Paradigm, Cs...>(InExcludedFlagmarkBit);
	}

	/**
	 * Add an array of traits exclusions to the filter.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator of the traits array.
	 * @param InExcludedTraitTypes The traits to exclude.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	TOutcome<Paradigm, FFilter&>
	Exclude(const TArray<UScriptStruct*, AllocatorT>& InExcludedTraitTypes)
	{
		EApparatusStatus Status = EApparatusStatus::Noop;
		for (auto InExcludedTraitType : InExcludedTraitTypes)
		{
			if (UNLIKELY(!InExcludedTraitType))
				continue;

			const FBitMask& Mask = FTraitmark::GetTraitMask(InExcludedTraitType);
			if (UNLIKELY(Fingerprint.GetTraitsMask().Includes(Mask)))
			{
				ExcludedTraitsMask.Reset();
				ExcludedTraits.Reset();
				return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Conflict, *this);
			}
			if (UNLIKELY(ExcludedTraitsMask.Includes(Mask)))
				continue;
			ExcludedTraitsMask.Include(Mask);
			ExcludedTraits.Add(InExcludedTraitType);
			Status = EApparatusStatus::Success;
			HashCache = 0;
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Add an array of traits exclusions to the filter.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator of the traits array.
	 * @param InExcludedTraitTypes The traits to exclude.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	[[deprecated("The method is renamed. Use the plain 'Exclude' overload, please.")]]
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	ExcludeTraits(const TArray<UScriptStruct*, AllocatorT>& InExcludedTraitTypes)
	{
		return Exclude<Paradigm>(InExcludedTraitTypes);
	}

	/**
	 * Add an array of details exclusions to the filter.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator of the details array.
	 * @param InExcludedDetailClasses The details to exclude.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT >
	TOutcome<Paradigm, FFilter&>
	Exclude(const TArray<TSubclassOf<UDetail>, AllocatorT>& InExcludedDetailClasses)
	{
		EApparatusStatus Status = EApparatusStatus::Noop;
		for (auto InExcludedDetailClass : InExcludedDetailClasses)
		{
			if (UNLIKELY(!InExcludedDetailClass))
				continue;

			const FBitMask& Mask = FDetailmark::GetExcludingDetailMask(InExcludedDetailClass);
			if (UNLIKELY(Fingerprint.GetDetailsMask().Includes(Mask)))
			{
				ExcludedDetailsMask.Reset();
				ExcludedDetails.Reset();
				return MakeOutcome<Paradigm, FFilter&>(EApparatusStatus::Conflict, *this);
			}
			if (UNLIKELY(ExcludedDetailsMask.Includes(Mask)))
				continue;
			ExcludedDetailsMask.Include(Mask);
			ExcludedDetails.Add(InExcludedDetailClass);
			Status = EApparatusStatus::Success;
			HashCache = 0;
		}
		return MakeOutcome<Paradigm, FFilter&>(Status, *this);
	}

	/**
	 * Add an array of details exclusions to the filter.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator of the details array.
	 * @param InExcludedDetailClasses The details to exclude.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT >
	[[deprecated("The method is renamed. Use the plain 'Exclude' overload, please.")]]
	FORCEINLINE TOutcome<Paradigm, FFilter&>
	ExcludeDetails(const TArray<TSubclassOf<UDetail>, AllocatorT>& InExcludedDetailClasses)
	{
		return Exclude<Paradigm>(InExcludedDetailClasses);
	}

	/**
	 * Remove an excluding flagmark from the filter specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The excluding flagmark to remove.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually performed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveExclusion(const EFlagmark InFlagmark)
	{
		if (LIKELY(ExcludingFlagmark & (EFlagmarkType)InFlagmark))
		{
			ExcludingFlagmark &= ~((EFlagmarkType)InFlagmark);
			HashCache = 0;
			return EApparatusStatus::Success;
		}
		return EApparatusStatus::Noop;
	}

	/**
	 * Remove an excluding trait type from the filter specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The excluding trait type to remove.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually performed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveExclusion(UScriptStruct* const TraitType)
	{
		if (UNLIKELY(!Excludes(TraitType)))
		{
			return EApparatusStatus::Noop;
		}
		verify(ExcludedTraits.RemoveSingle(TraitType) == 1);
		HashCache = 0;
		return EApparatusStatus::Success;
	}

	/**
	 * Remove an excluding detail class from the filter specification.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The excluding detail class to remove.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually performed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveExclusion(const TSubclassOf<UDetail> DetailClass)
	{
		if (UNLIKELY(!Excludes(DetailClass)))
		{
			return EApparatusStatus::Noop;
		}
		verify(ExcludedDetails.RemoveSingle(DetailClass) == 1);
		HashCache = 0;
		return EApparatusStatus::Success;
	}

	/**
	 * Remove an excluded trait from the filter.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait inclusion to remove.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTraitExclusion()
	{
		return RemoveExclusion(T::StaticStruct());
	}

	/**
	 * Remove an excluded trait from the filter.
	 * Templated version.
	 *
	 * @tparam T The type of the trait inclusion to remove.
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE auto
	RemoveTraitExclusion()
	{
		return RemoveExclusion<EParadigm::Default, T>();
	}

	/**
	 * Remove an excluded detail from the filter.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail inclusion to remove.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcome<Paradigm>
	RemoveDetailExclusion()
	{
		return RemoveExclusion<Paradigm>(D::StaticClass());
	}

	/**
	 * Remove an excluded detail from the filter.
	 * Templated version.
	 *
	 * @tparam D The class of the detail inclusion to remove.
	 * @return The status of the operation.
	 */
	template < class D >
	FORCEINLINE auto
	RemoveDetailExclusion()
	{
		return RemoveExclusion<EParadigm::Default, D>();
	}

	/**
	 * Remove the excluding component(s) from the filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Cs The excluding components to remove.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, class... Cs >
	FORCEINLINE TOutcome<Paradigm>
	RemoveExclusion()
	{
		const TOutcome<Paradigm> Statuses[] = { DoRemoveExclusion<Paradigm, Cs>() ... };
		return OutcomeCombine(Statuses, TOutcome<Paradigm>::Noop());
	}

	/**
	 * Remove the excluding component(s) from the filter.
	 * Templated paradigm version.
	 * 
	 * @tparam Cs The excluding components to remove.
	 * @return The outcome of the operation.
	 */
	template < class... Cs >
	FORCEINLINE auto
	RemoveExclusion()
	{
		return RemoveExclusion<EParadigm::Default, Cs...>();
	}

	/// @}
#pragma endregion Exclusion

#pragma region Examination
	/// @name Examination
	/// @{

	/**
	 * Check whether a filter includes a certain flag.
	 *
	 * @see Includes(const InFlagmark)
	 */
	FORCEINLINE bool
	Includes(const EFlagmarkBit InBit) const
	{
		return Fingerprint.HasFlag(InBit);
	}

	/**
	 * Check whether a filter includes all of the passed in flagmark flags.
	 * 
	 * @see Includes(const EFlagmarkBit)
	 */
	FORCEINLINE bool
	Includes(const EFlagmark InFlagmark) const
	{
		return EnumHasAllFlags(GetFlagmark(), InFlagmark);
	}

	/**
	 * Check whether a filter contains a trait specification.
	 */
	FORCEINLINE bool
	Includes(UScriptStruct* const TraitType) const
	{
		return Fingerprint.Contains(TraitType);
	}

	/**
	 * Check whether a filter contains a detail specification.
	 */
	FORCEINLINE bool
	Includes(const TSubclassOf<UDetail> DetailClass) const
	{
		return Fingerprint.Contains(DetailClass);
	}

	/**
	 * Check whether a filter contains a trait specification.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to search for.
	 */
	template < typename T >
	FORCEINLINE bool
	IncludesTrait() const
	{
		return Includes(T::StaticStruct());
	}

	/**
	 * Check whether a filter contains a detail specification.
	 * Templated version.
	 * 
	 * @tparam D The class of the detail to search for.
	 */
	template < class D >
	FORCEINLINE bool
	IncludesDetail() const
	{
		return Includes(D::StaticClass());
	}

	/**
	 * Check if the filter includes all of the passed in components.
	 * Templated version.
	 * 
	 * @tparam Cs The excluding components to check for.
	 * All must be satisfied.
	 * @return The state of examination.
	 */
	template < class... Cs >
	FORCEINLINE bool
	Includes() const
	{
		const bool Checks[] = { DoesInclude<Cs>() ... };
		for (int32 i = 0; i < sizeof...(Cs); ++i)
		{
			if (!Checks[i]) return false;
		}
		return true;
	}

	/**
	 * Check whether a filter excludes a flag.
	 */
	FORCEINLINE bool
	Excludes(const EFlagmarkBit InBit) const
	{
		return EnumHasFlag(GetExcludingFlagmark(), InBit);
	}

	/**
	 * Check whether a filter excludes all of the flagmark bits passed in.
	 */
	FORCEINLINE bool
	Excludes(const EFlagmark InFlagmark) const
	{
		return EnumHasAllFlags(GetExcludingFlagmark(), InFlagmark);
	}

	/**
	 * Check if the filter excludes a specific trait type.
	 */
	FORCEINLINE bool
	Excludes(UScriptStruct* const TraitType) const
	{
		check(TraitType);
		const FBitMask& Mask = FTraitmark::GetTraitMask(TraitType);
		return ExcludedTraitsMask.Includes(Mask);
	}

	/**
	 * Check if the filter excludes a specific detail class.
	 */
	FORCEINLINE bool
	Excludes(const TSubclassOf<UDetail> DetailClass) const
	{
		check(DetailClass);
		const FBitMask& Mask = FDetailmark::GetExcludingDetailMask(DetailClass);
		return ExcludedDetailsMask.Includes(Mask);
	}

	/**
	 * Check if the filter excludes a specific trait type.
	 * Templated version.
	 *
	 * @tparam T The type of the excluding trait to search for.
	 */
	template < typename T >
	FORCEINLINE bool
	ExcludesTrait() const
	{
		return Excludes(T::StaticStruct());
	}

	/**
	 * Check if the filter excludes a specific detail class.
	 * Templated version.
	 * 
	 * @tparam D The class of the excluding detail to search for.
	 */
	template < class D >
	FORCEINLINE bool
	ExcludesDetail() const
	{
		return Excludes(D::StaticClass());
	}

	/**
	 * Check if the filter excludes all of the passed in components.
	 * Templated version.
	 * 
	 * @tparam Cs The excluding components to check for.
	 * All must be satisfied.
	 * @return The state of examination.
	 */
	template < class... Cs >
	FORCEINLINE bool
	Excludes() const
	{
		const bool Checks[] = { DoesExclude<Cs>() ... };
		for (int32 i = 0; i < sizeof...(Cs); ++i)
		{
			if (!Checks[i]) return false;
		}
		return true;
	}

	/**
	 * Check if there are any conflicts in the filter.
	 */
	bool
	HasConflicts() const
	{
		if (EnumHasAnyFlags(GetExcludingFlagmark(), GetFlagmark()))
		{
			return true;
		}
		if (EnumHasAnyFlags(GetFlagmark(), GetExcludingFlagmark()))
		{
			return true;
		}
		for (auto ExcludedTraitType : ExcludedTraits)
		{
			if (UNLIKELY(!ExcludedTraitType))
				continue;

			const FBitMask& Mask = FTraitmark::GetTraitMask(ExcludedTraitType);
			if (UNLIKELY(Fingerprint.GetTraitsMask().Includes(Mask)))
			{
				return true;
			}
		}
		for (auto ExcludedDetailClass : ExcludedDetails)
		{
			if (UNLIKELY(!ExcludedDetailClass))
				continue;

			const FBitMask& Mask = FDetailmark::GetExcludingDetailMask(ExcludedDetailClass);
			if (UNLIKELY(Fingerprint.GetDetailsMask().Includes(Mask)))
			{
				return true;
			}
		}
		return false;
	}

	/// @}
#pragma endregion Examination

	/**
	 * Clear the fingerprint without any deallocations.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flagmark to reset to.
	 * @param InExcludingFlagmark The negative flagmark to reset to.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Reset(const EFlagmark InFlagmark = DefaultFlagmark,
		  const EFlagmark InExcludingFlagmark = DefaultExcludingFlagmark)
	{
		auto Outcome = SetExcludingFlagmark<MakePolite(Paradigm)>(InExcludingFlagmark);

		if (ExcludedTraits.Num() > 0)
		{
			ExcludedTraits.Reset();
			ExcludedTraitsMask.Reset();
			Outcome += EApparatusStatus::Success;
		}
		if (ExcludedDetails.Num() > 0)
		{
			ExcludedDetails.Reset();
			ExcludedDetailsMask.Reset();
			Outcome += EApparatusStatus::Success;
		}

		Outcome += Fingerprint.template Reset<MakePolite(Paradigm)>(InFlagmark);

		if (LIKELY(!IsNoop(Outcome)))
		{
			HashCache = 0;
		}

		return Outcome;
	}

	/**
	 * Check if the filter matches an another filter.
	 */
	bool Matches(const FFilter& Filter) const;

	/**
	 * Convert the filter to a string representation.
	 * 
	 * @return The string representation of the filter.
	 */
	FString
	ToString() const;

	/**
	 * An empty filter specification.
	 * 
	 * Will match all subjects (both booted and halted and stale).
	 */
	static const FFilter Zero;

	/**
	 * A default filter specification.
	 * 
	 * Will match the booted non-stale subjects only.
	 */
	static const FFilter Default;

	/**
	 * Calculate the hash sum of the filter.
	 * 
	 * The sum is actually cached internally,
	 * for a faster calculation time. This is in
	 * turn needed for faster iterables lookup.
	 * 
	 * @return The hashing sum of the filter.
	 */
	FORCEINLINE uint32
	CalcHash() const
	{
		if (LIKELY(HashCache != 0))
		{
			return HashCache;
		}
		return HashCache = 
			MoreHashCombine(GetTypeHash(Fingerprint),
							GetTypeHash(ExcludingFlagmark),
							GetTypeHash(GetExcludedTraitsMask()),
							GetTypeHash(GetExcludedDetailsMask()));
	}

#pragma region Serialization

	/**
	 * Post-serialize the filter updating its caches.
	 */
	void
	PostSerialize(const FArchive& Archive);

#pragma endregion Serialization

#pragma region Initialization
	/// @name Initialization
	/// @{

	// TODO: Remove this deprecated constructor.
	/**
	 * Construct a new filter with a boot filter. Deprecated.
	 * 
	 * The method is deprecated as of 1.10. Do not use it, please.
	 * 
	 * @see FFilter(EFlagmar, EFlagmark)
	 */
	[[deprecated("The boot filtering is deprecated as of 1.10. Use the flagmark version instead, please.")]]	
	FORCEINLINE
	FFilter(const EBootFilter InBootFilter)
	{
		check(InBootFilter != EBootFilter::None);
		if (InBootFilter == EBootFilter::All)
		{
			Fingerprint.SetFlag(EFlagmarkBit::Booted, false);
		}
		else if (InBootFilter == EBootFilter::Halted)
		{
			Exclude(EFlagmarkBit::Booted);
		}
		// Booted by default so do nothing here...
	}

	/**
	 * Construct a new filter with optional flagmark specifications.
	 */
	FORCEINLINE
	FFilter(const EFlagmark InFlagmark          = DefaultFlagmark,
			const EFlagmark InExcludingFlagmark = DefaultExcludingFlagmark)
	  : Fingerprint(InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{
		Exclude(InExcludingFlagmark);
	}

	/**
	 * Construct a new filter with a flagmark bit set.
	 * 
	 * @param InFlagmarkBit The flagmark bit to set.
	 */
	FORCEINLINE
	FFilter(const EFlagmarkBit InFlagmarkBit)
	  : FFilter(FlagToEnum(InFlagmarkBit))
	{}

	/**
	 * Construct a new filter with a flagmark bit set.
	 * 
	 * @param InFlagmarkBit The flagmark bit to set.
	 * @param InExcludingFlagmarkBit The flagmark bit to use as an excluding one.
	 */
	FORCEINLINE
	FFilter(const EFlagmarkBit InFlagmarkBit,
			const EFlagmarkBit InExcludingFlagmarkBit)
	  : FFilter(FlagToEnum(InFlagmarkBit), FlagToEnum(InExcludingFlagmarkBit))
	{}

	/**
	 * Construct a new filter while moving a fingerprint.
	 */
	FORCEINLINE
	FFilter(FFingerprint&& InFingerprint)
	  : Fingerprint(InFingerprint)
	{}

	/**
	 * Construct a new filter based on a fingerprint.
	 */
	FORCEINLINE
	FFilter(const FFingerprint& InFingerprint)
	  : Fingerprint(InFingerprint)
	{}

	/**
	 * Construct a new filter from a single trait type
	 * and an optional boot filter.
	 */
	FORCEINLINE
	FFilter(UScriptStruct* const InTraitType,
			const EFlagmark      InFlagmark = DefaultFlagmark)
	  : Fingerprint(InTraitType,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{}

	/**
	 * Construct a new filter from a single detail class
	 * and an optional boot filter.
	 */
	FORCEINLINE
	FFilter(const TSubclassOf<UDetail> InDetailClass,
			const EFlagmark            InFlagmark = DefaultFlagmark)
	  : Fingerprint(InDetailClass,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{}

	/**
	 * Construct a filter from an array of types.
	 */
	template < typename AllocatorT >
	FORCEINLINE
	FFilter(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses,
			const EFlagmark                                 InFlagmark = DefaultFlagmark)
	  : Fingerprint(InDetailClasses,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{}

	/**
	 * Construct a filter from an array of details.
	 */
	template < typename AllocatorT >
	FORCEINLINE
	FFilter(const TArray<UDetail*, AllocatorT>& InDetailClasses,
			const EFlagmark                     InFlagmark = DefaultFlagmark)
	  : Fingerprint(InDetailClasses,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{}

	/**
	 * Construct a filter from an array of types and excluded types.
	 */
	template < typename AllocatorT >
	FORCEINLINE
	FFilter(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses,
			const TArray<TSubclassOf<UDetail>, AllocatorT>& InExcludedDetailClasses,
			const EFlagmark                                 InFlagmark = DefaultFlagmark,
			const EFlagmark                                 InExcludingFlagmark = DefaultExcludingFlagmark)
	  : Fingerprint(InDetailClasses,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{
		ExcludeDetails(InExcludedDetailClasses);
		Exclude(InExcludingFlagmark);
		checkSlow(!HasConflicts());
	}

	/**
	 * Construct a filter from arrays of traits and details.
	 */
	template < typename AllocatorT >
	FORCEINLINE
	FFilter(const TArray<UScriptStruct*, AllocatorT>&       InTraitTypes,
			const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses,
			const EFlagmark                                 InFlagmark = DefaultFlagmark,
			const EFlagmark                                 InExcludingFlagmark = DefaultExcludingFlagmark)
	  : Fingerprint(InTraitTypes,
	  				InDetailClasses,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{
		Exclude(InExcludingFlagmark);
		checkSlow(!HasConflicts());
	}

	/**
	 * Construct a filter from initializer lists of traits and details.
	 */
	FORCEINLINE
	FFilter(std::initializer_list<UScriptStruct*>       InTraitTypes,
			std::initializer_list<TSubclassOf<UDetail>> InDetailClasses,
			const EFlagmark                             InFlagmark = DefaultFlagmark)
	  : Fingerprint(InTraitTypes,
					InDetailClasses,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{
		checkSlow(!HasConflicts());
	}

	/**
	 * Construct a filter from arrays of details and traits and exclusions.
	 */
	template < typename AllocatorT >
	FORCEINLINE
	FFilter(const TArray<UScriptStruct*, AllocatorT>&       InTraitTypes,
			const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses,
			const TArray<TSubclassOf<UDetail>, AllocatorT>& InExcludedDetailClasses,
			const EFlagmark                                 InFlagmark = DefaultFlagmark,
			const EFlagmark                                 InExcludingFlagmark = DefaultExcludingFlagmark)
	  : Fingerprint(InTraitTypes,
					InDetailClasses,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{
		ExcludeDetails(InExcludedDetailClasses);
		Exclude(InExcludingFlagmark);
		checkSlow(!HasConflicts());
	}

	/**
	 * Construct a filter from arrays of details and traits and exclusions.
	 */
	template < typename AllocatorT >
	FFilter(const TArray<UScriptStruct*, AllocatorT>&       InTraitTypes,
			const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses,
			const TArray<UScriptStruct*, AllocatorT>&       InExcludedTraitTypes,
			const TArray<TSubclassOf<UDetail>, AllocatorT>& InExcludedDetailClasses,
			const EFlagmark                                 InFlagmark = DefaultFlagmark,
			const EFlagmark                                 InExcludingFlagmark = DefaultExcludingFlagmark)
	  : Fingerprint(InTraitTypes,
	  				InDetailClasses,
					InFlagmark - EFlagmarkBit::Stale) // Filtering stale slots is not supported.
	{
		Exclude(InExcludedTraitTypes);
		Exclude(InExcludedDetailClasses);
		Exclude(InExcludingFlagmark);
		checkSlow(!HasConflicts());
	}

	/**
	 * Moves an existing filter.
	 */
	FORCEINLINE FFilter(FFilter&& InFilter)
	  : Fingerprint(MoveTemp(InFilter.Fingerprint))
	  , ExcludedTraits(MoveTemp(InFilter.ExcludedTraits))
	  , ExcludedTraitsMask(MoveTemp(InFilter.ExcludedTraitsMask))
	  , ExcludedDetails(MoveTemp(InFilter.ExcludedDetails))
	  , ExcludedDetailsMask(MoveTemp(InFilter.ExcludedDetailsMask))
	  , ExcludingFlagmark(InFilter.ExcludingFlagmark)
	  , HashCache(InFilter.HashCache)
	{
		checkSlow(!HasConflicts());
	}

	/**
	 * Initializes the filter as a copy of another one.
	 */
	FORCEINLINE
	FFilter(const FFilter& InFilter)
	  : Fingerprint(InFilter.Fingerprint)
	  , ExcludedTraits(InFilter.ExcludedTraits)
	  , ExcludedTraitsMask(InFilter.ExcludedTraitsMask)
	  , ExcludedDetails(InFilter.ExcludedDetails)
	  , ExcludedDetailsMask(InFilter.ExcludedDetailsMask)
	  , ExcludingFlagmark(InFilter.ExcludingFlagmark)
	  , HashCache(InFilter.HashCache)
	{
		checkSlow(!InFilter.HasConflicts());
	}

	/**
	 * Convert from a generic filter.
	 * 
	 * @tparam Cs The components of the filter.
	 */
	template < typename ...Cs >
	FFilter(const TFilter<Cs...>& InFilter);

	/**
	 * Make a new filter with a list of components.
	 * 
	 * @tparam Ts The types and classes of traits and details to fill with.
	 * @param InFlagmark The flagmark to use.
	 * @param InExcludingFlagmark The excluding flagmark to use.
	 * @return The resulting traitmark.
	 */
	template < typename ...Ts >
	static FORCEINLINE FFilter
	Make(const EFlagmark InFlagmark          = DefaultFlagmark,
		 const EFlagmark InExcludingFlagmark = DefaultExcludingFlagmark)
	{
		FFilter Filter(InFlagmark, InExcludingFlagmark);
		verify(OK(Filter.template Include<Ts...>()));
		return MoveTemp(Filter);
	}

	/**
	 * Make a new filter with a list of components.
	 * 
	 * @tparam Ts The types and classes of traits and details to fill with.
	 * @param InFlagmarkBit The flagmark bit to use.
	 * @return The resulting traitmark.
	 */
	template < typename... Ts >
	static FORCEINLINE FFilter
	Make(const EFlagmarkBit InFlagmarkBit)
	{
		FFilter Filter(InFlagmarkBit);
		verify(OK(Filter.template Include<Ts...>()));
		return MoveTemp(Filter);
	}

	/**
	 * Make a new filter with a list of components.
	 * 
	 * @tparam Ts The types and classes of traits and details to fill with.
	 * @param InFlagmarkBit The flagmark bit to use.
	 * @param InExcludingFlagmarkBit The flagmark bit to use as an excluding one.
	 * @return The resulting traitmark.
	 */
	template < typename... Ts >
	static FORCEINLINE FFilter
	Make(const EFlagmarkBit InFlagmarkBit,
		 const EFlagmarkBit InExcludingFlagmarkBit)
	{
		FFilter Filter(InFlagmarkBit, InExcludingFlagmarkBit);
		verify(OK(Filter.template Include<Ts...>()));
		return MoveTemp(Filter);
	}

	/// @}
#pragma endregion Initialization

}; //-struct FFilter

FORCEINLINE uint32
GetTypeHash(const FFilter& Filter)
{
	return Filter.CalcHash();
}

FORCEINLINE bool
FTraitmark::Matches(const FFilter& Filter) const
{
	return Matches(Filter.GetTraitmark()) &&
		   !GetTraitsMask().IncludesPartially(Filter.GetExcludedTraitsMask());
}

FORCEINLINE bool
FDetailmark::Matches(const FFilter& Filter) const
{
	return Matches(Filter.GetDetailmark()) &&
		   !GetDetailsMask().IncludesPartially(Filter.GetExcludedDetailsMask());
}

#pragma region Fingerprint

FORCEINLINE bool
FFingerprint::FlagmarkMatches(const FFilter& Filter) const
{
	return ::Matches(GetFlagmark(),
					 Filter.GetFlagmark(),
					 Filter.GetExcludingFlagmark());
}

FORCEINLINE bool
FFingerprint::TraitsMatch(const FFilter& Filter) const
{
	return GetTraitsMask().Includes(Filter.GetTraitsMask()) &&
		   (!GetTraitsMask().IncludesPartially(Filter.GetExcludedTraitsMask()));
}

FORCEINLINE bool
FFingerprint::DetailsMatch(const FFilter& Filter) const
{
	return GetDetailsMask().Includes(Filter.GetDetailsMask()) &&
		   (!GetDetailsMask().IncludesPartially(Filter.GetExcludedDetailsMask()));
}

FORCEINLINE bool
FFingerprint::Matches(const EFlagmark IncludingFlagmark,
					  const EFlagmark ExcludingFlagmark/*=FM_None*/) const
{
	return ::Matches(GetFlagmark(),
					 IncludingFlagmark,
					 ExcludingFlagmark);
}

FORCEINLINE bool
FFingerprint::Matches(const FFilter& Filter) const
{
	return FlagmarkMatches(Filter) &&
		   TraitsMatch(Filter) &&
		   DetailsMatch(Filter);
}

FORCEINLINE bool
FFingerprint::Matches(const FFilter&  Filter,
					  const EFlagmark IncludingFlagmarkOverride,
					  const EFlagmark ExcludingFlagmarkOverride) const
{
	return Matches(IncludingFlagmarkOverride, ExcludingFlagmarkOverride) &&
		   TraitsMatch(Filter) &&
		   DetailsMatch(Filter);
}

#pragma endregion Fingerprint

FORCEINLINE bool
FFilter::Matches(const FFilter& Filter) const
{
	return Fingerprint.Matches(Filter) &&
		   !ExcludedTraitsMask.IncludesPartially(Filter.GetTraitsMask()) &&
		   !ExcludedDetailsMask.IncludesPartially(Filter.GetDetailsMask());
}

FORCEINLINE void
FFilter::Set(FFilter&& InFilter)
{
	Reset();

	Fingerprint = MoveTemp(InFilter.Fingerprint);
	
	ExcludedTraits      = MoveTemp(InFilter.ExcludedTraits);
	ExcludedTraitsMask  = MoveTemp(InFilter.ExcludedTraitsMask);
	ExcludedDetails     = MoveTemp(InFilter.ExcludedDetails);
	ExcludedDetailsMask = MoveTemp(InFilter.ExcludedDetailsMask);
	ExcludingFlagmark   = InFilter.ExcludingFlagmark;

	HashCache = InFilter.HashCache;
	InFilter.HashCache = 0; // Invalidate just in case...
}

FORCEINLINE void
FFilter::Set(const FFilter& InFilter)
{
	Reset();

	Fingerprint = InFilter.Fingerprint;
	
	ExcludedTraits      = InFilter.ExcludedTraits;
	ExcludedTraitsMask  = InFilter.ExcludedTraitsMask;
	ExcludedDetails     = InFilter.ExcludedDetails;
	ExcludedDetailsMask = InFilter.ExcludedDetailsMask;
	ExcludingFlagmark   = InFilter.ExcludingFlagmark;

	HashCache = InFilter.HashCache;
}

template< typename AllocatorT >
FORCEINLINE void
FFilter::Set(const TArray<UScriptStruct*, AllocatorT>& TraitTypes)
{
	Reset(GetFlagmark());

	Include(TraitTypes);
}

template< typename AllocatorT >
FORCEINLINE void
FFilter::Set(const TArray<TSubclassOf<UDetail>, AllocatorT>& DetailClasses)
{
	Reset(GetFlagmark());

	Include(DetailClasses);
}

template< typename AllocatorT >
FORCEINLINE void
FFilter::Set(const TArray<UDetail*, AllocatorT>& Details)
{
	Reset(GetFlagmark());

	Include(Details);
}

/**
 * The templated shortcut class
 * for constructing a filter.
 * 
 * @tparam Cs The components of the filter.
 * May feature traits, details and
 * flagmark indicators.
 */
template < typename ...Cs >
struct TFilter : public FFilterIndicator
{
  public:

	static constexpr EFlagmark DefaultFlagmark          = FFilter::DefaultFlagmark;

	static constexpr EFlagmark DefaultExcludingFlagmark = FFilter::DefaultExcludingFlagmark;

	static constexpr bool HasFlagmarkSpec          = TFlagmarkAccumulator<Cs...>::Count > 0;

	static constexpr bool HasExcludingFlagmarkSpec = TExcludingFlagmarkAccumulator<Cs...>::Count > 0;

  private:

	friend struct FFilter;

	/**
	 * Generic component adapter
	 * supporting both traits and details compile-time.
	 */
	template < typename C,
			   bool IsTrait     = IsTraitType<C>(),
			   bool IsDetail    = IsDetailClass<C>(),
			   bool IsFlagmark  = std::is_base_of<FFlagmarkIndicator, C>::value,
			   bool IsExclusion = std::is_base_of<FExclusionIndicator, C>::value >
	struct TComponentOps
	{
		static constexpr bool
		Includes()
		{
			return false;
		}

		static constexpr bool
		Excludes()
		{
			return false;
		}
	}; //-struct TComponentOps

	// Trait version.
	template < typename T >
	struct TComponentOps<T, true, false, false, false>
	{
		static constexpr bool
		Includes()
		{
			return more::is_base_contained<T, Cs...>::value;
		}

		static constexpr bool
		Excludes()
		{
			return more::is_base_contained<TExclude<T>, Cs...>::value;
		}
	}; //-struct TComponentOps<F, true, false, false, false>

	// Detail version.
	template < typename D >
	struct TComponentOps<D, false, true, false, false>
	{
		static constexpr bool
		Includes()
		{
			return more::is_base_contained<D, Cs...>::value;
		}

		static constexpr bool
		Excludes()
		{
			return more::is_base_contained<TExclude<D>, Cs...>::value;
		}
	}; //-struct TComponentOps<F, false, true, false, false>

	// Flagmark version.
	template < class F >
	struct TComponentOps<F, false, false, true, false>
	{
		static constexpr bool
		Includes()
		{
			return IncludesAtCompileTime(F::Value);
		}

		static constexpr bool
		Excludes()
		{
			return ExcludesAtCompileTime(F::Value);
		}
	}; //-struct TComponentOps<F, false, false, true, false>

	// Excluding version.
	template < class E >
	struct TComponentOps<E, false, false, false, true>
	{
		static constexpr bool
		Includes()
		{
			return TComponentOps<typename E::Type>::Excludes();
		}

		static constexpr bool
		Excludes()
		{
			return TComponentOps<typename E::Type>::Includes();
		}
	}; //-struct TComponentOps<E, false, false, false, true>

	/**
	 * Check if the filter includes a certain component.
	 */
	template < class C >
	static FORCEINLINE constexpr bool
	DoesIncludeAtCompileTime()
	{
		return TComponentOps<C>::Includes();
	}

	/**
	 * Check if the filter excludes a certain component.
	 */
	template < class C >
	static FORCEINLINE constexpr bool
	DoesExcludeAtCompileTime()
	{
		return TComponentOps<C>::Excludes();
	}

	template< typename... Ts >
	struct TIncludingAccumulator;

	template< >
	struct TIncludingAccumulator<>
	{
		static constexpr bool
		GetValue()
		{
			return true;
		}
	};

	template< typename Head, typename... Tail >
	struct TIncludingAccumulator<Head, Tail...>
	{
		static constexpr bool
		GetValue()
		{
			return DoesIncludeAtCompileTime<Head>() && TIncludingAccumulator<Tail...>::GetValue();
		} 
	};

	template< typename... Ts >
	struct TExcludingAccumulator;

	template< >
	struct TExcludingAccumulator<>
	{
		static constexpr bool
		GetValue()
		{
			return true;
		}
	};

	template< typename Head, typename... Tail >
	struct TExcludingAccumulator<Head, Tail...>
	{
		static constexpr bool
		GetValue()
		{
			return DoesExcludeAtCompileTime<Head>() && TExcludingAccumulator<Tail...>::GetValue();
		} 
	};

	/**
	 * Check for contradictions within the filter.
	 */
	template< typename... Ts >
	struct TContradictionTester;

	template< >
	struct TContradictionTester<>
	{
		static constexpr bool
		IsContradicting()
		{
			return false;
		}
	};

	template< >
	struct TContradictionTester<FFM_None>
	{
		static constexpr bool
		IsContradicting()
		{
			return false;
		}
	};

	template< >
	struct TContradictionTester<TExclude<FFM_None>>
	{
		static constexpr bool
		IsContradicting()
		{
			return false;
		}
	};

	template< typename Head, typename... Tail >
	struct TContradictionTester<Head, Tail...>
	{
		static constexpr bool
		IsContradicting()
		{
			return DoesExcludeAtCompileTime<Head>() || TContradictionTester<Tail...>::IsContradicting();
		} 
	};

  public:

#pragma region Compile-Time Flagmarking

	/**
	 * Get the compile-time flagmark specification.
	 */
	static constexpr EFlagmark
	GetFlagmarkAtCompileTime()
	{
		return HasFlagmarkSpec ? TFlagmarkAccumulator<Cs...>::Value : DefaultFlagmark;
	}

	/**
	 * Get the excluding compile-time flagmark specification.
	 */
	static constexpr EFlagmark
	GetExcludingFlagmarkAtCompileTime()
	{
		return HasExcludingFlagmarkSpec ? TExcludingFlagmarkAccumulator<Cs...>::Value : DefaultExcludingFlagmark;
	}

	/**
	 * Get the flagmark specification.
	 */
	constexpr EFlagmark
	GetFlagmark() const
	{
		return GetFlagmarkAtCompileTime();
	}

	/**
	 * Get the excluding flagmark specification.
	 */
	constexpr EFlagmark
	GetExcludingFlagmark() const
	{
		return GetExcludingFlagmarkAtCompileTime();
	}

#pragma endregion Compile-Time Flagmarking

#pragma region Inherited Methods

	/**
	 * Check whether a filter includes a certain flag at compile-time.
	 */
	static constexpr bool
	IncludesAtCompileTime(const EFlagmarkBit InBit)
	{
		if (more::is_contained<FFM_All, Cs...>::value) // Compile-time branch.
		{
			return true;
		}
		return GetFlagmarkAtCompileTime() & InBit;
	}

	/**
	 * Check whether a filter includes all of the passed flags at compile-time.
	 */
	static constexpr bool
	IncludesAtCompileTime(const EFlagmark InFlagmark)
	{
		if (more::is_contained<FFM_All, Cs...>::value) // Compile-time branch.
		{
			return true;
		}
		return EnumHasAllFlags(GetFlagmarkAtCompileTime(), InFlagmark);
	}

	/**
	 * Check whether a filter excludes a certain flag at compile-time.
	 */
	static constexpr bool
	ExcludesAtCompileTime(const EFlagmarkBit InBit)
	{
		if (more::is_contained<FFM_All, Cs...>::value) // Compile-time branch.
		{
			return false;
		}
		return GetExcludingFlagmarkAtCompileTime() & InBit;
	}

	/**
	 * Check whether a filter excludes all of the passed flags at compile-time.
	 */
	static constexpr bool
	ExcludesAtCompileTime(const EFlagmark InFlagmark)
	{
		if (more::is_contained<FFM_All, Cs...>::value) // Compile-time branch.
		{
			return InFlagmark == FM_None;
		}
		return EnumHasAllFlags(GetExcludingFlagmarkAtCompileTime(), InFlagmark);
	}

	/**
	 * Check whether a filter includes a certain flag.
	 */
	constexpr FORCEINLINE bool
	Includes(const EFlagmarkBit InBit) const
	{
		return IncludesAtCompileTime(InBit);
	}

	/**
	 * Check whether a filter includes all of the passed flags.
	 */
	constexpr FORCEINLINE bool
	Includes(const EFlagmark InFlagmark) const
	{
		return IncludesAtCompileTime(InFlagmark);
	}

	/**
	 * Check whether a filter excludes a certain flag.
	 */
	constexpr FORCEINLINE bool
	Excludes(const EFlagmarkBit InBit) const
	{
		return ExcludesAtCompileTime(InBit);
	}

	/**
	 * Check whether a filter excludes all of the passed flags.
	 */
	constexpr FORCEINLINE bool
	Excludes(const EFlagmark InFlagmark) const
	{
		return ExcludesAtCompileTime(InFlagmark);
	}

	/**
	 * Check if the filter includes certain component(s).
	 */
	template < class ...InCs >
	constexpr bool
	Includes() const
	{
		return TIncludingAccumulator<InCs...>::GetValue();
	}

	/**
	 * Check if the filter includes certain component(s).
	 */
	template < class ...InCs >
	constexpr bool
	Excludes() const
	{
		return TExcludingAccumulator<InCs...>::GetValue();
	}

	/**
	 * Check if the filter requires a belt-wise iterating process.
	 * 
	 * Returns @c true, if there are any details included within the filter.
	 */
	constexpr bool
	IsBeltBased() const
	{
		return more::is_base_contained<UDetail, Cs...>::value;
	}

#pragma endregion Inherited Methods

#pragma region Initialization

	/**
	 * Initialize a new filter instance.
	 */
	constexpr
	TFilter()
	{
		static_assert(!TContradictionTester<Cs...>::IsContradicting(),
					  "There should not be any contradictions within the filter specification.");
	}

#pragma endregion Initialization

}; //-struct TFilter

template < typename ...Cs >
FORCEINLINE
FFilter::FFilter(const TFilter<Cs...>& InFilter)
{
	Include<Cs...>();
	Include(InFilter.GetFlagmark());
	Exclude(InFilter.GetExcludingFlagmark());
}

template<>
struct TStructOpsTypeTraits<FFilter> : public TStructOpsTypeTraitsBase2<FFilter>
{
	enum 
	{
		WithCopy = true,
		WithIdentical = true,
		WithPostSerialize = true
	}; 
};
