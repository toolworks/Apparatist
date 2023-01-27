/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Detailmark.h
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

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Serialization/Archive.h"
#include "More/Containers/Array.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define DETAILMARK_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"
#include "BitMask.h"
#include "DetailInfo.h"

#ifdef DETAILMARK_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Detailmark.generated.h"


// Forward declarations:
struct FFingerprint;
struct FFilter;

/**
 * The detail-only fingerprint part.
 */
USTRUCT(BlueprintType, Meta = (HasNativeMake = "ApparatusRuntime.ApparatusFunctionLibrary.MakeDetailmark"))
struct APPARATUSRUNTIME_API FDetailmark
{
	GENERATED_BODY()

  public:

	/**
	 * Invalid detail identifier.
	 */
	static constexpr auto InvalidDetailId = FDetailInfo::InvalidId;

	/**
	 * The type of the details array container.
	 */
	typedef TArray<TSubclassOf<UDetail>> DetailsType;

  private:

	/**
	 * A list of details.
	 * 
	 * Doesn't contain neither nulls nor duplicates during the runtime.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detailmark",
			  Meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UDetail>> Details;

	/**
	 * The current details mask.
	 * 
	 * This is always updated according to the details list.
	 */
	mutable FBitMask DetailsMask;

	/**
	 * Decompose details with their base classes when they are
	 * added to the detailmark.
	 */
	bool bDecomposed = false;

	friend struct FFilter;
	friend class UChunk;
	friend struct FSubjectHandle;

	friend class UBelt;
	friend class AMechanism;
	friend class UApparatusFunctionLibrary;

	/**
	 * Get the details mask of the detailmark.
	 */
	FORCEINLINE FBitMask&
	GetDetailsMaskRef() { return DetailsMask; }

  public:

	/**
	 * Get a detail's unique identifier.
	 */
	static FDetailInfo::IdType
	GetDetailId(const TSubclassOf<UDetail> DetailClass);

	/**
	 * Get the total number of registered details so far.
	 */
	static int32
	RegisteredDetailsNum();

	/**
	 * Get the cached mask of a detail type.
	 */
	static const FBitMask&
	GetDetailMask(const TSubclassOf<UDetail> DetailClass);

	/**
	 * Get the excluded mask of a detail type.
	 */
	static const FBitMask&
	GetExcludingDetailMask(const TSubclassOf<UDetail> DetailClass);

	/**
	 * Get the mask of a detail's class.
	 * 
	 * Cached internally.
	 */
	static const FBitMask&
	GetDetailMask(const UDetail* Detail);

	/**
	 * Get the excluded mask of a details's class.
	 * 
	 * Cached internally.
	 */
	static FORCEINLINE const FBitMask&
	GetExcludingDetailMask(const UDetail* const Detail)
	{
		return GetExcludingDetailMask(Detail->GetClass());
	}

	/**
	 * Get the mask of a detail.
	 */
	template < class D >
	static FORCEINLINE const FBitMask&
	GetDetailMask()
	{
		return GetDetailMask(D::StaticClass());
	}

	/**
	 * Get the details of the detailmark.
	 */
	FORCEINLINE const DetailsType&
	GetDetails() const
	{
		return Details;
	}

	/**
	 * Convert to an array of detail classes.
	 */
	FORCEINLINE explicit operator const DetailsType&() const
	{
		return Details;
	}

	/**
	 * Check if the detailmark is empty.
	 * 
	 * The detailmark is empty if it has no details in it.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	IsEmpty() const
	{
		return Details.Num() == 0;
	}

	/**
	 * The number of details in the detailmark.
	 * 
	 * @return The number of details currently within
	 * the detailmark.
	 */
	FORCEINLINE int32
	DetailsNum() const
	{
		return Details.Num();
	}

	/**
	 * Get the details mask of the detailmark.
	 * Constant version.
	 * 
	 * @return The effective details bit-mask.
	 */
	FORCEINLINE const FBitMask&
	GetDetailsMask() const
	{
		return DetailsMask;
	}

	/**
	 * Get a detail type by its index.
	 * 
	 * @param Index The index of the detail to get.
	 * @return The detail at the specified index.
	 */
	FORCEINLINE TSubclassOf<UDetail>
	DetailAt(const int32 Index) const
	{
		return Details[Index];
	}

	/**
	 * Get a detail type by its index.
	 * 
	 * @param Index The index of the detail to get.
	 * @return The detail at the specified index.
	 * @see DetailAt()
	 */
	FORCEINLINE TSubclassOf<UDetail>
	operator[](const int32 Index) const
	{
		return DetailAt(Index);
	}

	/**
	 * Check if a detailmark is viable and has any actual effect.
	 */
	FORCEINLINE operator bool() const
	{
		return Details.Num() > 0;
	}

#pragma region Comparison

	/**
	 * Compare two detailmarks for equality.
	 * 
	 * Two detailmarks are considered to be equal
	 * if their details composition is equal (regardless of the ordering).
	 */
	FORCEINLINE bool
	operator==(const FDetailmark& Other) const
	{
		return GetDetailsMask() == Other.GetDetailsMask();
	}

	/**
	 * Compare two detailmarks for inequality.
	 * 
	 * Two detailmarks are considered to be inequal
	 * if their details composition is different (regardless of the ordering).
	 */
	FORCEINLINE bool
	operator!=(const FDetailmark& Other) const
	{
		return GetDetailsMask() != Other.GetDetailsMask();
	}

	/**
	 * Compare two detailmarks for equality. Editor-friendly method.
	 * 
	 * This compares the details arrays during the editing mode,
	 * since it is used for the changes detection.
	 * 
	 * @param Other The other detailmark to compare to.
	 * @param PortFlags The contextual port flags.
	 * @return The state of examination.
	 */
	bool
	Identical(const FDetailmark* Other, uint32 PortFlags) const
	{
#if WITH_EDITOR
		if (!FApp::IsGame())
		{
			// Correct support for property editing
			// requires comparing the actual details arrays.
			return Details == Other->Details;
		}
#endif
		return (*this) == (*Other);
	}

#pragma endregion Comparison

  private:

	/**
	 * @internal Generic component adapter
	 * supporting both traits and details compile-time.
	 */
	template < typename T, bool bIsDetail = IsDetailClass<T>() >
	struct TComponentOps;
	
	// Non-detail version.
	template < typename T >
	struct TComponentOps<T, /*bIsDetail=*/false>
	{
		
		static FORCEINLINE constexpr int32
		IndexOf(const FDetailmark* const)
		{
			return INDEX_NONE;
		}

		static FORCEINLINE constexpr bool
		Contains(const FDetailmark* const)
		{
			return false;
		}

	}; //-struct TComponentOps<T, bIsDetail=false>

	// Detail version.
	template < class D >
	struct TComponentOps<D, /*bIsDetail=*/true>
	{
		
		static FORCEINLINE int32
		IndexOf(const FDetailmark* const InDetailmark)
		{
			return InDetailmark->IndexOf(D::StaticClass());
		}

		static FORCEINLINE bool
		Contains(const FDetailmark* const InDetailmark)
		{
			return InDetailmark->Contains(D::StaticClass());
		}

	}; //-struct TComponentOps<T, bIsDetail=true>

#pragma region Search
	/// @name Search
	/// @{

  public:

	/**
	 * Get the index of a specific detail class.
	 * 
	 * Searches for an exact class first, by
	 * parental class information second.
	 * 
	 * @param DetailClass The detail to search for.
	 * @return The index of the detail class or
	 * @c INDEX_NONE, if there is no such detail within
	 * or @p DetailClass is @c nullptr.
	 */
	int32
	IndexOf(const TSubclassOf<UDetail> DetailClass) const
	{
		if (UNLIKELY(DetailClass == nullptr))
		{
			return INDEX_NONE;
		}

		const FBitMask& Mask = GetDetailMask(DetailClass);
		if (UNLIKELY(!GetDetailsMask().Includes(Mask)))
		{
			return INDEX_NONE;
		}

		// Try to find the exact class first...
		for (int32 i = 0; i < Details.Num(); ++i)
		{
			if (UNLIKELY(Details[i] == nullptr))
				continue;
			if (*Details[i] == *DetailClass)
				return i;
		}

		// Try to find some child class now...
		for (int32 i = 0; i < Details.Num(); ++i)
		{
			if (UNLIKELY(Details[i] == nullptr))
				continue;
			if (Details[i]->IsChildOf(DetailClass))
				return i;
		}

		checkf(false, TEXT("Detail was not found in the list while the bit mask matched: %s"),
			   *DetailClass->GetName());
		return INDEX_NONE;
	}

	/**
	 * Get the index of a specific detail class.
	 * Templated version.
	 * 
	 * Searches for an exact class first, by
	 * parental class information second.
	 * 
	 * @note The method actually supports non-detail classes,
	 * i.e. @c INDEX_NONE will be safely returned in that case.
	 * 
	 * @tparam D The detail to search for.
	 * @return The index of the detail class or
	 * @c INDEX_NONE, if there is no such detail within
	 * the mark.
	 */
	template < class D >
	FORCEINLINE constexpr int32
	IndexOf() const
	{
		return TComponentOps<D>::IndexOf(this);
	}

	/**
	 * Find all of the indices of a detail class.
	 * 
	 * @tparam AllocatorT The type of the output array allocator.
	 * @param DetailClass A detail class to find the indices of.
	 * @param OutIndices The resulting indices storage.
	 * @return The status of the operation.
	 */
	template < typename AllocatorT >
	EApparatusStatus
	IndicesOf(const TSubclassOf<UDetail> DetailClass,
			  TArray<int32, AllocatorT>& OutIndices) const
	{
		OutIndices.Reset();

		const FBitMask& Mask = GetDetailMask(DetailClass);
		if (UNLIKELY(!GetDetailsMask().Includes(Mask)))
		{
			return EApparatusStatus::NoItems;
		}

		for (int32 i = 0; i < Details.Num(); ++i)
		{
			check(Details[i]);
			if (*Details[i] == *DetailClass) // Faster check.
			{
				OutIndices.Add(i);
			}
			else if (Details[i]->IsChildOf(DetailClass))
			{
				OutIndices.Add(i);
			}
		}

		check(OutIndices.Num() > 0);
		return EApparatusStatus::Success;
	}

	/**
	 * Find all of the indices of a detail class.
	 * 
	 * @tparam AllocatorT The type of the output array allocator.
	 * @param DetailClass A detail class to find the indices of.
	 * @param OutIndices The resulting indices storage.
	 * @return The status of the operation.
	 */
	template < class D, typename AllocatorT,
			   TDetailClassSecurity<D> = true >
	FORCEINLINE EApparatusStatus
	IndicesOf(TArray<int32, AllocatorT>& OutIndices) const
	{
		return IndicesOf(D::StaticClass(), OutIndices);
	}

	/**
	 * Check if the detailmark includes a detail class.
	 */
	FORCEINLINE bool
	Contains(const TSubclassOf<UDetail> DetailClass) const
	{
		checkf(DetailClass, TEXT("The detail class must be provided for detail checks."));
		const FBitMask& Mask = GetDetailMask(DetailClass);
		return GetDetailsMask().Includes(Mask);
	}

	/**
	 * Check if the detailmark includes a detail class. Templated version.
	 */
	template < class D >
	FORCEINLINE bool
	Contains() const
	{
		return TComponentOps<D>::Contains(this);
	}

	/// @}
#pragma endregion Search

#pragma region Mapping
	/// @name Mapping
	/// @{

	/**
	 * Find an indexing mapping from another detailmark defined by an array of details.
	 *
	 * @param InDetailsClasses A detailmark to get a mapping from.
	 * @param OutMapping The resulting mapping.
	 */
	template < typename TAllocatorA, typename TAllocatorB >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const TArray<TSubclassOf<UDetail>, TAllocatorA>& InDetailsClasses,
					TArray<int32, TAllocatorB>&                      OutMapping) const
	{
		OutMapping.Reset(InDetailsClasses.Num());
		for (int32 i = 0; i < InDetailsClasses.Num(); ++i)
		{
			OutMapping.Add(IndexOf(InDetailsClasses[i]));
		}
		return OutMapping.Num() > 0 ? EApparatusStatus::Success : EApparatusStatus::NoItems;
	}

	/**
	 * Find an indexing mapping from another detailmark defined by an array of details.
	 * Standard array version.
	 *
	 * @param InDetailsClasses A detailmark to get a mapping from.
	 * @param OutMapping The resulting mapping to output to.
	 */
	template < typename TAllocator, int32 Size >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const TArray<TSubclassOf<UDetail>, TAllocator>& InDetailsClasses,
					std::array<int32, Size>&                        OutMapping) const
	{
		checkf(Size >= InDetailsClasses.Num(),
			   TEXT("The size of the destination array must be enough to store the mapping."));
		for (int32 i = 0; i < InDetailsClasses.Num(); ++i)
		{
			OutMapping[i] = IndexOf(InDetailsClasses[i]);
		}
		return InDetailsClasses.Num() > 0 ? EApparatusStatus::Success : EApparatusStatus::NoItems;
	}

	/**
	 * Get an indexing mapping from another detailmark's details.
	 *
	 * @param InDetailmark A detailmark to get a mapping from.
	 * @param OutMapping The resulting details mapping.
	 * @return The status of the operation.
	 */
	template < typename AllocatorT >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const FDetailmark&         InDetailmark,
					TArray<int32, AllocatorT>& OutMapping) const
	{
		return FindMappingFrom(InDetailmark.GetDetails(), OutMapping);
	}

	/**
	 * Get an indexing mapping from another detailmark's details.
	 *
	 * @param InDetailmark A detailmark to get a mapping from.
	 * @param OutMapping The resulting details mapping.
	 * @return The status of the operation.
	 */
	template < int32 Size >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const FDetailmark&       InDetailmark,
					std::array<int32, Size>& OutMapping) const
	{
		return FindMappingFrom(InDetailmark.GetDetails(), OutMapping);
	}

	/**
	 * Get an indexing multi-mapping from another detailmark defined by an array of details.
	 *
	 * @param InDetailsClasses A detailmark to get a mapping from.
	 * @param OutMapping The resulting two-dimensional multi-mapping.
	 */
	template< typename TAllocatorA,
			  typename TAllocatorB,
			  typename TAllocatorC >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const TArray<TSubclassOf<UDetail>, TAllocatorA>& InDetailsClasses,
					TArray<TArray<int32, TAllocatorC>, TAllocatorB>& OutMapping) const
	{
		OutMapping.Reset(InDetailsClasses.Num());
		for (int32 i = 0; i < InDetailsClasses.Num(); ++i)
		{
			IndicesOf(InDetailsClasses[i], OutMapping.AddDefaulted_GetRef());
		}
		return OutMapping.Num() > 0 ? EApparatusStatus::Success : EApparatusStatus::Noop;
	}

	/**
	 * Get an indexing multi-mapping from another detailmark.
	 *
	 * @param InDetailmark A detailmark to get a mapping from.
	 * @param OutMapping The resulting two-dimensional multi-mapping.
	 */
	template < typename TAllocatorA, typename TAllocatorB >
	FORCEINLINE EApparatusStatus
	FindMappingFrom(const FDetailmark&                               InDetailmark,
					TArray<TArray<int32, TAllocatorB>, TAllocatorA>& OutMapping) const
	{
		return FindMappingFrom(InDetailmark.GetDetails(), OutMapping);
	}

	/**
	 * Find an indexing details mapping to another detailmark.
	 *
	 * @param InDetailmark A detailmark to get the details mapping to.
	 * @param OutMapping The resulting mapping.
	 * @return The status of the operation.
	 */
	template < typename AllocatorT >
	FORCEINLINE EApparatusStatus
	FindMappingTo(const FDetailmark&         InDetailmark,
				  TArray<int32, AllocatorT>& OutMapping) const
	{
		return InDetailmark.FindMappingFrom(*this, OutMapping);
	}

	/**
	 * Find an indexing details mapping to another detailmark.
	 * Standard array output version.
	 *
	 * @tparam Size The size of output mapping buffer.
	 * @param InDetailmark A detailmark to get the details mapping to.
	 * @param OutMapping The resulting mapping.
	 * @return The status of the operation.
	 */
	template < int32 Size >
	FORCEINLINE EApparatusStatus
	FindMappingTo(const FDetailmark&       InDetailmark,
				  std::array<int32, Size>& OutMapping) const
	{
		return InDetailmark.FindMappingFrom(*this, OutMapping);
	}

	/**
	 * Find a multi-indexing details mapping to another detailmark.
	 *
	 * @param InDetailmark A detailmark to get the details mapping to.
	 * @param OutMapping The resulting multi-mapping.
	 * @return The status of the operation.
	 */
	template < typename TAllocatorA, typename TAllocatorB >
	FORCEINLINE EApparatusStatus
	FindMappingTo(const FDetailmark&                               InDetailmark,
				  TArray<TArray<int32, TAllocatorB>, TAllocatorA>& OutMapping) const
	{
		return InDetailmark.FindMappingFrom(*this, OutMapping);
	}

	/// @}
#pragma region Mapping

#pragma region Matching
	/// @name Matching
	/// @{

	/**
	 * Check if the detailmark matches a filter.
	 */
	bool
	Matches(const FFilter& Filter) const;

	/**
	 * Check if the detailmark matches another detailmark acting as a filter.
	 */
	FORCEINLINE bool
	Matches(const FDetailmark& InDetailmark) const
	{
		return GetDetailsMask().Includes(InDetailmark.GetDetailsMask());
	}

	/// @}
#pragma endregion Matching

#pragma region Assignment
	/// @name Assignment
	/// @{

	/**
	 * Set a detailmark to an array of detail classes.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The allocator for the array.
	 * @param InDetailsClasses The classes of the details to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename AllocatorT >
	TOutcome<Paradigm>
	Set(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailsClasses)
	{
		if (IsHarsh(Paradigm)) // Compile-time branch.
		{
			Reset<Paradigm>();
			return Add<Paradigm>(InDetailsClasses);
		}
		else
		{
			EApparatusStatus Status = EApparatusStatus::Noop;
			FBitMask NewDetailsMask;
			for (auto InDetailClass : InDetailsClasses)
			{
				if (InDetailClass == nullptr)
				{
					continue;
				}
				const auto& DetailMask = GetDetailMask(InDetailClass);
				NewDetailsMask.Include(DetailMask);
			}
			if (UNLIKELY(DetailsMask == NewDetailsMask))
			{
				return EApparatusStatus::Noop;
			}
			for (auto InDetailClass : InDetailsClasses)
			{
				if (InDetailClass == nullptr)
				{
					continue;
				}
				const auto& InDetailMask = GetDetailMask(InDetailClass);
				if (LIKELY(DetailsMask.template Include<MakePolite(Paradigm)>(InDetailMask) == EApparatusStatus::Success))
				{
					Details.Add(InDetailClass);
				}
			}
		}
	}

	/**
	 * Set a detailmark to an array of active details.
	 * 
	 * Only active details get actually added.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetails The details to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename AllocatorT >
	TOutcome<Paradigm>
	Set(const TArray<UDetail*, AllocatorT>& InDetails)
	{
		if (IsHarsh(Paradigm))
		{
			Reset<Paradigm>();
			return Add<Paradigm>(InDetails);
		}
		else
		{
			EApparatusStatus Status = EApparatusStatus::Noop;
			FBitMask NewDetailsMask;
			for (auto InDetail : InDetails)
			{
				if (InDetail == nullptr)
				{
					continue;
				}
				if (!InDetail->IsActive())
				{
					continue;
				}
				const auto& DetailMask = GetDetailMask(InDetail);
				NewDetailsMask.Include(DetailMask);
			}
			if (UNLIKELY(DetailsMask == NewDetailsMask))
			{
				return EApparatusStatus::Noop;
			}
			for (auto InDetail : InDetails)
			{
				if (InDetail == nullptr)
				{
					continue;
				}
				if (!InDetail->IsActive())
				{
					continue;
				}
				const auto& InDetailMask = GetDetailMask(InDetail->GetClass());
				if (LIKELY(DetailsMask.template Include<MakePolite(Paradigm)>(InDetailMask) == EApparatusStatus::Success))
				{
					Details.Add(InDetail->GetClass());
				}
			}
		}
	}

	/**
	 * Move an another detailmark to the detailmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The detailmark to move.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Set(FDetailmark&& InDetailmark)
	{
		if (UNLIKELY(GetDetailsMask() == InDetailmark.GetDetailsMask()))
		{
			return EApparatusStatus::Noop;
		}
		Details     = MoveTemp(InDetailmark.Details);
		DetailsMask = MoveTemp(InDetailmark.DetailsMask);
		return EApparatusStatus::Success;
	}

	/**
	 * Set the detailmark equal to another detailmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The detailmark to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Set(const FDetailmark& InDetailmark)
	{
		if (UNLIKELY(std::addressof(InDetailmark) == this))
		{
			return EApparatusStatus::Noop;
		}
		const auto Status = GetDetailsMaskRef().Set<MakePolite(Paradigm)>(InDetailmark.GetDetailsMask());
		if (LIKELY(Status == EApparatusStatus::Success))
		{
			Details = InDetailmark.Details;
		}
		return Status;
	}

	/**
	 * Move a detailmark.
	 */
	FORCEINLINE FDetailmark&
	operator=(FDetailmark&& InDetailmark)
	{
		Set(InDetailmark);
		return *this;
	}

	/**
	 * Set a detailmark equal to another detailmark.
	 */
	FORCEINLINE FDetailmark&
	operator=(const FDetailmark& InDetailmark)
	{
		Set(InDetailmark);
		return *this;
	}

	/// @}
#pragma endregion Assignment

#pragma region Addition
	/// @name Addition
	/// @{

	/**
	 * Add a detail class.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The class of the detail to add.
	 * May be a @c nullptr and will ignore it silently
	 * in this case.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Add(const TSubclassOf<UDetail> DetailClass)
	{
		if (UNLIKELY(DetailClass == nullptr)) return EApparatusStatus::Noop;
		if (bDecomposed)
		{
			return DoAddDecomposed<Paradigm>(DetailClass);
		}
		auto Status = EApparatusStatus::Noop;
		
		const FBitMask& DetailMask = GetDetailMask(DetailClass);
		// We have to accommodate for base classes which already may be included
		// in the mask, but still should be added to the array explicitly...
		if (LIKELY((DetailsMask.template Include<MakePolite(Paradigm)>(DetailMask) == EApparatusStatus::Success)
				|| (LIKELY(Details.Find(DetailClass) == INDEX_NONE))))
		{
			Details.Add(DetailClass);
			Status = EApparatusStatus::Success;
		}

		return Status;
	}

	/**
	 * Add detail classes to the detailmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailClasses The detail classes to add.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Add(std::initializer_list<TSubclassOf<UDetail>> InDetailClasses)
	{
		auto Status = EApparatusStatus::Noop;
		for (auto InDetailClass : InDetailClasses)
		{
			if (UNLIKELY(InDetailClass == nullptr))
			{
				continue;
			}
			StatusAccumulate(Status, ToStatus(Add<Paradigm>(InDetailClass)));
		}
		return Status;
	}

	/**
	 * Add a detailmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The detailmark to add.
	 * @return The outcome of the operation.
	 * Returns EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Add(const FDetailmark& InDetailmark)
	{
		if (bDecomposed)
		{
			return DoAddDecomposed<Paradigm>(InDetailmark);
		}
		if (UNLIKELY(std::addressof(InDetailmark) == this))
		{
			return EApparatusStatus::Noop;
		}

		return Add<Paradigm>(InDetailmark.Details);
	}

	/**
	 * Add an array of detail classes.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the array allocator.
	 * @param InDetailsClasses The detail classes to add.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, typename AllocatorT >
	TOutcome<Paradigm>
	Add(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailsClasses)
	{
		EApparatusStatus Status = EApparatusStatus::Noop;
		for (auto InDetailClass : InDetailsClasses)
		{
			if (UNLIKELY(InDetailClass == nullptr))
			{
				continue;
			}
			const FBitMask& Mask = GetDetailMask(InDetailClass);
			// Check if already is included:
			if (LIKELY(DetailsMask.template Include<MakePolite(Paradigm)>(Mask) == EApparatusStatus::Success))
			{
				Details.Add(InDetailClass);
				Status = EApparatusStatus::Success;
			}
		}
		return Status;
	}

	/**
	 * Add an array of detail classes.
	 * 
	 * @tparam AllocatorT The type of the array allocator.
	 * @param InDetailsClasses The detail classes to add.
	 * @return The status of the operation.
	 */
	template < typename AllocatorT >
	FORCEINLINE auto
	Add(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailsClasses)
	{
		return Add<EParadigm::Default>(InDetailsClasses);
	}

	/**
	 * Add an array of details.
	 * 
	 * Only active details' classes get added.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the array allocator.
	 * @param InDetails The details to add.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, typename AllocatorT >
	TOutcome<Paradigm>
	Add(const TArray<UDetail*, AllocatorT>& InDetails)
	{
		auto Status = EApparatusStatus::Noop;
		for (auto InDetail : InDetails)
		{
			if (UNLIKELY(!InDetail))
			{
				continue;
			}
			if (UNLIKELY(!InDetail->IsEnabled()))
			{
				continue;
			}
			
			const auto InDetailClass = InDetail->GetClass();
			const FBitMask& DetailMask = GetDetailMask(InDetailClass);
			// Check if already is included:
			if (LIKELY(DetailsMask.template Include<MakePolite(Paradigm)>(DetailMask) == EApparatusStatus::Success))
			{
				Details.Add(InDetailClass);
				Status = EApparatusStatus::Success;
			}
		}
		return Status;
	}

	/**
	 * Add an array of details.
	 * 
	 * Only active details' classes get added.
	 * 
	 * @tparam AllocatorT The type of the array allocator.
	 * @param InDetails The details to add.
	 * @return The status of the operation.
	 */
	template < typename AllocatorT >
	FORCEINLINE auto
	Add(const TArray<UDetail*, AllocatorT>& InDetails)
	{
		return Add<EParadigm::Default>(InDetails);
	}

  private:

	/**
	 * Add a detail class to the detailmark. Templated version.
	 *
	 * This is an internal helper method.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to add.
	 * The pointer will be stripped automatically as needed.
	 * @return The status of the operation.
	 * 
	 * @see Add()
	 */
	template < EParadigm Paradigm, class D, typename std::enable_if<IsDetailClass<typename std::remove_pointer<D>::type>(), int>::type = 0 >
	FORCEINLINE TOutcome<Paradigm>
	DoAdd()
	{
		return Add<Paradigm>(std::remove_pointer<D>::type::StaticClass());
	}

	/**
	 * An addition stub for non-detail classes.
	 */
	template < EParadigm Paradigm, class D, typename std::enable_if<!IsDetailClass<typename std::remove_pointer<D>::type>(), int>::type = 0 >
	FORCEINLINE TOutcome<Paradigm>
	DoAdd()
	{
		return TOutcome<Paradigm>::Noop();
	}

	/**
	 * Add a detail class while decomposing it
	 * with its base classes.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass A class of the detail to add.
	 * May be a @c nullptr, since adding nothing is a valid EApparatusStatus::Noop
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	DoAddDecomposed(const TSubclassOf<UDetail> DetailClass)
	{
		if (UNLIKELY(DetailClass == nullptr))
		{
			return EApparatusStatus::Noop;
		}
		const FBitMask& Mask = GetDetailMask(DetailClass);
		// Check if already is included:
		if (UNLIKELY(IsNoop(DetailsMask.template Include<MakePolite(Paradigm)>(Mask))))
		{
			return EApparatusStatus::Noop;
		}
		for (TSubclassOf<UDetail> BaseClass = DetailClass->GetSuperClass();
					 			  BaseClass != nullptr;
								  BaseClass = BaseClass->GetSuperClass())
		{
			// There is some base class available.
			// Add it as well since we should be decomposing:
			Details.AddUnique(BaseClass);
		}
		Details.Add(DetailClass);
		return EApparatusStatus::Success;
	}

	/**
	 * Add a detailmark while decomposing its details to their base classes.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The detailmark to add.
	 * @return The status of the operation.
	 * Returns EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	DoAddDecomposed(const FDetailmark& InDetailmark)
	{
		if (UNLIKELY(GetDetailsMask().Includes(InDetailmark.GetDetailsMask())))
		{
			return EApparatusStatus::Noop;
		}
		for (const auto InDetail : InDetailmark.Details)
		{
#if !WITH_EDITOR
			check(InDetail != nullptr);
#endif
			AssessOK(Paradigm, DoAddDecomposed<Paradigm>(InDetail));
		}
		return EApparatusStatus::Success;
	}

  public:

	/**
	 * Add detail(s) to the detailmark.
	 * Templated paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ds The detail(s) to add.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, class... Ds >
	FORCEINLINE TOutcome<Paradigm>
	Add()
	{
		if (sizeof...(Ds) == 0) // Compile-time branch.
		{
			return EApparatusStatus::Noop;
		}
		Details.Reserve(Details.Num() + sizeof...(Ds));
		return OutcomeCombine(DoAdd<Paradigm, Ds>()...);
	}

	/**
	 * Add detail(s) to the detailmark.
	 * Templated version.
	 * 
	 * @tparam Ds The detail(s) to add.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < class... Ds, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Add()
	{
		return Add<Paradigm, Ds...>();
	}

	/**
	 * Add variadic detail classes to the detailmark.
	 */
	FORCEINLINE FDetailmark&
	operator+=(std::initializer_list<TSubclassOf<UDetail>> DetailClasses)
	{
		Add(DetailClasses);
		return *this;
	}

	/**
	 * Add an array of detail classes to the detailmark.
	 */
	FORCEINLINE FDetailmark&
	operator+=(const TArray<TSubclassOf<UDetail>>& DetailClasses)
	{
		Add(DetailClasses);
		return *this;
	}

	/**
	 * Add a single detail class to the detailmark.
	 */
	FORCEINLINE FDetailmark&
	operator+=(const TSubclassOf<UDetail> DetailClass)
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
	 * Remove a detail class from the detailmark.
	 *'
	 * @tparam Paradigm The paradigm to work under.
	 * May be a @c nullptr and will ignore be ignored silently
	 * in that case.
	 * @param DetailClass A detail class to remove.
	 * @returns Was anything actually changed?
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Remove(const TSubclassOf<UDetail> DetailClass)
	{
		if (UNLIKELY(!DetailClass))
		{
			return EApparatusStatus::Noop;
		}

		// Check if there is actually such detail in the detailmark...
		const FBitMask& DetailMask = GetDetailMask(DetailClass);
		if (UNLIKELY(!GetDetailsMask().Includes(DetailMask)))
		{
			return EApparatusStatus::Noop;
		}

		if (UNLIKELY(Details.RemoveSwap(DetailClass) == 0))
		{
			return EApparatusStatus::Noop;
		}

		// We can't just set the mask here, as
		// there can be other details with the same
		// base class detail mask, so we rebuild
		// the mask completely now...
		DetailsMask.Reset();
		for (int32 i = 0; i < Details.Num(); ++i)
		{
			if (UNLIKELY(Details[i] == nullptr)) continue;
			const FBitMask& Mask = GetDetailMask(Details[i]);
			DetailsMask |= Mask;
		}

		return EApparatusStatus::Success;
	}

  private:

	/**
	 * Remove a detail class from the detailmark. Internal templated version.
	 *
	 * @tparam Paradigm The paradigm to execute under.
	 * @tparam D The class of the detail to add.
	 * @returns The resulting outcome.
	 */
	template < EParadigm Paradigm, typename D, typename std::enable_if<IsDetailClass<typename std::remove_pointer<D>::type>(), int>::type = 0 >
	FORCEINLINE TOutcome<Paradigm>
	DoRemove()
	{
		return Remove<Paradigm>(more::flatten<D>::type::StaticClass());
	}

	/**
	 * A removal stub for non-detail classes.
	 */
	template < EParadigm Paradigm, typename D, typename std::enable_if<!IsDetailClass<typename std::remove_pointer<D>::type>(), int>::type = 0 >
	FORCEINLINE TOutcome<Paradigm>
	DoRemove()
	{
		return TOutcome<Paradigm>::Noop();
	}

  public:

	/**
	 * Remove a detail class from the detailmark.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ds The classes of the details to remove.
	 * @returns The outcome of the operation.
	 */
	template < EParadigm Paradigm, class... Ds >
	FORCEINLINE TOutcome<Paradigm>
	Remove()
	{
		if (sizeof...(Ds) == 0) // Compile-time branch.
		{
			return EApparatusStatus::Noop;
		}
		return OutcomeCombine(DoRemove<Paradigm, Ds>()...);
	}

	/**
	 * Remove a detail class from the detailmark.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ds The classes of the details to remove.
	 * @returns The outcome of the operation.
	 */
	template < class... Ds, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	Remove()
	{
		return Remove<Paradigm, Ds...>();
	}

	/**
	 * Remove a detail type from the detailmark.
	 */
	FORCEINLINE FDetailmark&
	operator-=(const TSubclassOf<UDetail> DetailClass)
	{
		Remove(DetailClass);
		return *this;
	}

	/**
	 * Clear the detailmark without any deallocations.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If the detailmark is already empty.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Reset()
	{
		if (UNLIKELY(Details.Num() == 0))
		{
			return EApparatusStatus::Noop;
		}
		Details.Reset();
		DetailsMask.Reset();
		return EApparatusStatus::Success;
	}

	/// @}
#pragma endregion Removal

	/**
	 * Convert a detailmark to a string.
	 */
	FString ToString() const;

	/**
	 * An empty detailmark constant.
	 */
	static const FDetailmark Zero;

#pragma region Serialization
	/// @name Serialization
	/// @{

	/**
	 * Serialization operator.
	 */
	friend FORCEINLINE FArchive&
	operator<<(FArchive& Ar, FDetailmark& Detailmark)
	{
		Ar << Detailmark.Details;
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

	/// @}
#pragma endregion Serialization

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Initialize a new detailmark.
	 */
	FORCEINLINE
	FDetailmark()
	  : DetailsMask(RegisteredDetailsNum())
	{}

	/**
	 * Move-initialize a detailmark.
	 * 
	 * @param Detailmark The detailmark to copy.
	 */
	FORCEINLINE
	FDetailmark(FDetailmark&& Detailmark)
	  : Details(MoveTemp(Detailmark.Details))
	  , DetailsMask(MoveTemp(Detailmark.DetailsMask))
	{}

	/**
	 * Initialize a new detailmark by moving an another one.
	 * 
	 * @param Detailmark The detailmark to move.
	 */
	FORCEINLINE
	FDetailmark(const FDetailmark& Detailmark)
	  : Details(Detailmark.Details)
	  , DetailsMask(Detailmark.DetailsMask)
	{}

	/**
	 * Initialize a new detailmark with a single detail class.
	 * 
	 * @param DetailClass A detail class to initialize with.
	 */
	FDetailmark(TSubclassOf<UDetail> DetailClass);

	/**
	 * Initialize a new detailmark with an initializer list of detail classes
	 * and an optional boot state.
	 */
	FDetailmark(std::initializer_list<TSubclassOf<UDetail>> InDetailClasses);

	/**
	 * Initialize a new detailmark with an array of detail classes
	 * and an optional boot state.
	 */
	template < typename AllocatorT >
	FDetailmark(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses);

	/**
	 * Construct a new detailmark with an array of details
	 * and an optional boot state.
	 */
	template < typename AllocatorT >
	FDetailmark(const TArray<UDetail*, AllocatorT>& InDetails);

	/**
	 * Make a new detailmark with a list of details classes.
	 * 
	 * @tparam Ds The classes of details to fill with.
	 * @return The resulting detailmark.
	 */
	template < typename... Ds >
	static FORCEINLINE FDetailmark
	Make()
	{
		FDetailmark Detailmark;
		verify(OK(Detailmark.template Add<Ds...>()));
		return MoveTemp(Detailmark);
	}

	/// @}
#pragma endregion Initialization

}; //-FDetailmark

FORCEINLINE uint32
GetTypeHash(const FDetailmark& Detailmark)
{
	return GetTypeHash(Detailmark.GetDetailsMask());
}

FORCEINLINE bool
operator==(const TSubclassOf<UDetail>& DetailClassA,
		   UClass* const               ClassB)
{
	return *DetailClassA == ClassB;
}

FORCEINLINE bool
operator!=(const TSubclassOf<UDetail>& DetailClassA,
		   UClass* const               ClassB)
{
	return *DetailClassA != ClassB;
}

FORCEINLINE bool
operator==(const TSubclassOf<UDetail>& DetailClassA,
		   const TSubclassOf<UDetail>& DetailClassB)
{
	return *DetailClassA == *DetailClassB;
}

FORCEINLINE bool
operator!=(const TSubclassOf<UDetail>& DetailClassA,
		   const TSubclassOf<UDetail>& DetailClassB)
{
	return *DetailClassA != *DetailClassB;
}

template<>
struct TStructOpsTypeTraits<FDetailmark> : public TStructOpsTypeTraitsBase2<FDetailmark>
{
	enum 
	{
		WithCopy = true,
		WithIdentical = true,
		WithSerializer = true,
		WithPostSerialize = true
	}; 
};
