/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectOnScope.h
 * Created: 2021-10-05 18:17:31
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

#include "HAL/UnrealMemory.h"
#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Misc/Optional.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define SUBJECT_RECORD_H_SKIPPED_MACHINE_H
#endif

#include "Fingerprint.h"
#include "TraitRecord.h"

#ifdef SUBJECT_RECORD_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "SubjectRecord.generated.h"


// Forward declarations:
struct FSubjectInfo;
struct FSubjectHandle;
struct FSolidSubjectHandle;
struct FConstSubjectHandle;
class AMechanism;
class UMachine;

/**
 * A subject packed in a localized container scope.
 * 
 * Unlike usual iterable subjects, the records are allowed
 * to be serialized and authored in the Editor.
 *
 * These are mainly used as templates (prefabs) when spawning
 * the new subjects.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FSubjectRecord
{
	GENERATED_BODY()

  private:

	friend struct FSubjectInfo;
	friend class AMechanism;
	friend class UMachine;

	/**
	 * A list of traits on the subject.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data,
			  Meta = (AllowPrivateAccess = "true",
			  		  DisplayAfter = "Flagmark"))
	TArray<FTraitRecord> Traits;

	/**
	 * The flagmark of the subjective.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data,
			  Meta = (Bitmask, BitmaskEnum = "EFlagmark",
					  DisplayBefore = "Traits",
					  AllowPrivateAccess = "true"))
	int32 Flagmark = FM_None;

	/**
	 * The cached fingerprint.
	 */
	mutable FFingerprint FingerprintCache;

	/**
	 * Direct access to the trait records array.
	 * Constant version.
	 */
	FORCEINLINE const TArray<FTraitRecord>&
	GetTraitsRef() const
	{
		return Traits;
	}

	/**
	 * Direct access to the trait records array.
	 */
	FORCEINLINE TArray<FTraitRecord>&
	GetTraitsRef()
	{
		return Traits;
	}

	/**
	 * Get a trait record by its type.
	 * Constant version.
	 * 
	 * @warning The returned reference can change
	 * when the new records are added.
	 */
	const FTraitRecord&
	GetTraitRecord(UScriptStruct* const TraitType) const
	{
		check(TraitType);
		check(Contains(TraitType));
		for (int32 i = 0; i < Traits.Num(); ++i)
		{
			if (Traits[i].GetType() == TraitType)
			{
				return Traits[i];
			}
		}
		checkNoEntry();
		return *(new FTraitRecord());
	}

	/**
	 * Get a trait record by its type.
	 * 
	 * @warning The returned reference can change
	 * when the new records are added.
	 */
	FTraitRecord&
	GetTraitRecord(UScriptStruct* const TraitType)
	{
		check(TraitType);
		check(Contains(TraitType));
		for (int32 i = 0; i < Traits.Num(); ++i)
		{
			if (Traits[i].GetType() == TraitType)
			{
				return Traits[i];
			}
		}
		checkNoEntry();
		return *(new FTraitRecord());
	}

	/**
	 * Get a trait record by its type.
	 * Statically-typed immutable version.
	 * 
	 * @warning The returned reference can change
	 * when the new records are added.
	 * 
	 * @tparam T The type of the trait to get a record of.
	 * Must be a valid trait type.
	 */
	template < typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE const FTraitRecord&
	GetTraitRecord() const
	{
		return GetTraitRecord(T::StaticStruct());
	}

	/**
	 * Get a trait record by its type.
	 * Statically-typed mutable version.
	 * 
	 * @warning The returned reference can change
	 * when the new records are added.
	 * 
	 * @tparam T The type of the trait to get a record of.
	 * Must be a valid trait type.
	 */
	template < typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE FTraitRecord&
	GetTraitRecord()
	{
		return GetTraitRecord(T::StaticStruct());
	}

	/**
	 * Obtain a trait record by its type.
	 * 
	 * @warning The returned reference can change
	 * when the new records are added.
	 */
	FTraitRecord&
	ObtainTraitRecord(UScriptStruct* const TraitType)
	{
		check(TraitType);
		if (Contains(TraitType))
		{
			for (int32 i = 0; i < Traits.Num(); ++i)
			{
				if (Traits[i].GetType() == TraitType)
				{
					return Traits[i];
				}
			}
			checkNoEntry();
		}
		// No such trait. Add a new one...
		FingerprintCache.Add(TraitType);
		return Traits.Add_GetRef(TraitType);
	}

	/**
	 * Obtain a trait record by its type.
	 * Templated version.
	 * 
	 * @warning The returned reference can change
	 * when the new records are added.
	 *
	 * @tparam T The type of the trait to add a record of.
	 */
	template < typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE FTraitRecord&
	ObtainTraitRecord()
	{
		return ObtainTraitRecord(T::StaticStruct());
	}

	void
	RefreshFingerprintCache()
	{
		// Actualize the fingerprint cache...
		FingerprintCache.Reset((EFlagmark)Flagmark);
		for (auto& Trait : Traits)
		{
			if (UNLIKELY(!Trait)) continue;
			FingerprintCache.Add(Trait.GetType());
		}
	}

  public:

	/**
	 * Get the trait records.
	 */
	FORCEINLINE const TArray<FTraitRecord>&
	GetTraits() const
	{
		return Traits;
	}

	/**
	 * Get a trait record at specified index.
	 */
	FORCEINLINE const FTraitRecord&
	TraitAt(const int32 Index) const
	{
		return Traits[Index];
	}

	/**
	 * Get the number of traits in the subject.
	 */
	FORCEINLINE int32
	TraitsNum() const
	{
		return Traits.Num();
	}

	/**
	 * Get the cached fingerprint of the subject record.
	 */
	FORCEINLINE const FFingerprint&
	GetFingerprint() const
	{
		return FingerprintCache;
	}

	/**
	 * Get the cached traitmark of the subject record.
	 */
	FORCEINLINE const FTraitmark&
	GetTraitmark() const
	{
		return FingerprintCache.GetTraitmark();
	}

#pragma region Flagmark

	/**
	 * Get the flagmark of the subject.
	 */
	FORCEINLINE EFlagmark
	GetFlagmark() const
	{
		return (EFlagmark)Flagmark;
	}

	/**
	 * Set the flagmark of the subject.
	 */
	FORCEINLINE EApparatusStatus
	SetFlagmark(const EFlagmark InFlagmark)
	{
		Flagmark = (decltype(Flagmark))InFlagmark;
		// TODO: Rework to be using a paradigm specification.
		return FingerprintCache.SetFlagmark<EParadigm::Polite>(InFlagmark);
	}

	/**
	 * Set the flagmark bit for the subject.
	 */
	FORCEINLINE EApparatusStatus
	SetFlag(const EFlagmarkBit InFlag, bool bState = true)
	{
		const auto NewFlagmark = EnumWithFlagSet(GetFlagmark(), InFlag, bState);
		return SetFlagmark(NewFlagmark);
	}

	/**
	 * Check if the flagmark bit is set on the subject.
	 */
	FORCEINLINE bool
	HasFlag(const EFlagmarkBit InFlag) const
	{
		return FingerprintCache.HasFlag(InFlag);
	}

#pragma endregion Flagmark

#pragma region Trait Getting

	/**
	 * Get the trait's data pointer.
	 */
	FORCEINLINE void*
	GetTraitPtr(UScriptStruct* const TraitType)
	{
		return GetTraitRecord(TraitType).GetData();
	}

	/**
	 * Get the trait's data pointer.
	 * Constant version.
	 */
	FORCEINLINE const void*
	GetTraitPtr(UScriptStruct* const TraitType) const
	{
		return GetTraitRecord(TraitType).GetData();
	}

	/**
	 * Get the trait's data pointer.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to get a pointer to.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), T*>::type
	GetTraitPtr()
	{
		return static_cast<T*>(GetTraitRecord<T>().GetData());
	}

	/**
	 * Get the trait's data pointer.
	 * Templated constant version.
	 * 
	 * @tparam T The type of the constant trait to get a pointer to.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), const T*>::type
	GetTraitPtr() const
	{
		return static_cast<const T*>(GetTraitRecord<T>().GetData());
	}

	/**
	 * Get the trait's data reference.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to get a reference for.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), T&>::type
	GetTraitRef()
	{
		return static_cast<T&>(GetTraitRecord<T>().template GetDataRef<T>());
	}

	/**
	 * Get the trait's data reference.
	 * Templated constant version.
	 *
	 * @tparam T The type of the constant trait to get a reference for.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), const T&>::type
	GetTraitRef() const
	{
		return static_cast<const T&>(GetTraitRecord<T>().template GetDataRef<T>());
	}

#pragma endregion Trait Getting

#pragma region Trait Setting

	/**
	 * Set a trait.
	 * Statically typed version.
	 * 
	 * @tparam T The type of the trait to set.
	 * @param InTrait The trait to set to.
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), EApparatusStatus>::type
	SetTrait(const T& InTrait)
	{
		return ObtainTraitRecord<T>().Set(InTrait);
	}

#pragma endregion Trait Setting

#pragma region Trait Obtainment

	/**
	 * Obtain a trait data pointer by its type.
	 *
	 * @warning The data pointer can change
	 * when the new trait records are added.
	 */
	void*
	ObtainTraitPtr(UScriptStruct* const TraitType)
	{
		return ObtainTraitRecord(TraitType).GetData();
	}

	/**
	 * Obtain a trait data reference by its type.
	 * Templated version.
	 *
	 * @warning The data reference can change
	 * when the new trait records are added.
	 * 
	 * @tparam T The type of the trait to obtain a reference for.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), T&>::type
	ObtainTraitRef()
	{
		return ObtainTraitRecord<T>().template GetDataRef<T>();
	}

#pragma endregion Trait Obtainment

#pragma region Examination

	/**
	 * Check if the subject record contains a trait of specific type.
	 */
	FORCEINLINE bool
	Contains(UScriptStruct* const TraitType) const
	{
		return FingerprintCache.Contains(TraitType);
	}

	/**
	 * Check if the subject record contains a trait of specific type.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to look for.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), bool>::type
	Contains() const
	{
		return Contains(T::StaticStruct());
	}

#pragma endregion Examination

#pragma region Assignment
	/// @name Assignment
	/// @{

	/**
	 * Move-assign the subject record
	 * from an another one.
	 */
	FSubjectRecord&
	operator=(FSubjectRecord&& InSubjectRecord)
	{
		// Move the data...
		Traits           = MoveTemp(InSubjectRecord.Traits);
		Flagmark         = InSubjectRecord.Flagmark;
		FingerprintCache = MoveTemp(InSubjectRecord.FingerprintCache);

		return *this;
	}

	/**
	 * Assign the subject record equal
	 * to an another record.
	 */
	FSubjectRecord&
	operator=(const FSubjectRecord& InSubjectRecord)
	{
		if (UNLIKELY(this == std::addressof(InSubjectRecord)))
		{
			return *this;
		}

		// Copy the data...
		Traits           = InSubjectRecord.Traits;
		Flagmark         = InSubjectRecord.Flagmark;
		FingerprintCache = InSubjectRecord.FingerprintCache;

		return *this;
	}

	/// @}
#pragma endregion Assignment

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Construct a new empty subject pack.
	 */
	FORCEINLINE
	FSubjectRecord()
	{}

	/**
	 * Initialize a subject record as a copy
	 * of another one.
	 */
	FORCEINLINE
	FSubjectRecord(const FSubjectRecord& InSubjectRecord)
	  : Traits(InSubjectRecord.Traits)
	  , Flagmark(InSubjectRecord.Flagmark)
	  , FingerprintCache(InSubjectRecord.FingerprintCache)
	{}

	/**
	 * Move-construct a new packed subject.
	 */
	FORCEINLINE
	FSubjectRecord(FSubjectRecord&& InSubjectRecord)
	  : Traits(MoveTemp(InSubjectRecord.Traits))
	  , Flagmark(InSubjectRecord.Flagmark)
	  , FingerprintCache(MoveTemp(InSubjectRecord.FingerprintCache))
	{}

	/**
	 * Construct a new packed subject with a traitmark
	 * and a flagmark.
	 */
	FORCEINLINE
	FSubjectRecord(const FTraitmark& InTraitmark,
				   const EFlagmark   InFlagmark = FM_None)
	  : Flagmark(InFlagmark - FM_AllSystemLevel)
	  , FingerprintCache(InTraitmark, Flagmark)
	{
		Traits.Reserve(InTraitmark.TraitsNum());
		for (int32 i = 0; i < InTraitmark.TraitsNum(); ++i)
		{
			if (LIKELY(InTraitmark[i]))
			{
				Traits.Add(InTraitmark[i]);
			}
		}
	}

	/**
	 * Construct a new subject record as a copy of an
	 * existing subject.
	 *
	 * @note Only user-level flags are copied by default.
	 *
	 * @param Subject A handle for the subject to copy.
	 * @param FlagmarkMask A mask for capturing the target subject flagmark.
	 * Only user-level flags are copied by default.
	 */
	explicit
	FSubjectRecord(const FConstSubjectHandle& Subject,
				   const EFlagmark            FlagmarkMask = FM_AllUserLevel);

	/**
	 * Construct a new subject record as a copy of an existing subject masked with a filter.
	 *
	 * @note Only user-level flags are copied by default.
	 *
	 * @param Subject A handle for the subject to copy.
	 * @param Mask A filter used as a mask for including/excluding
	 * traits from the @p Subject.
	 */
	FSubjectRecord(const FConstSubjectHandle& Subject,
				   const FFilter&             Mask);

	/// @}
#pragma endregion Initialization

#pragma region Serialization
	/// @name Serialization
	/// @{

	/**
	 * Post-serialize the record via an archive.
	 */
	void
	PostSerialize(const FArchive& Archive)
	{
		RefreshFingerprintCache();
	};

	/**
	 * Called after creation via Blueprints.
	 */
	void
	PostScriptConstruct()
	{
		RefreshFingerprintCache();
	}

	/**
	 * Construct a new subject record from an archive
	 * with the trait types and flagmark supplied explicitly.
	 *
	 * @param InTraitTypes The supplied trait types.
	 * @param InFlagmark The supplied flagmark.
	 * @param Archive The serializing archive. Should be opened
	 * for loading.
	 */
	template < typename AllocatorT >
	void
	SerializeBin(const TArray<UScriptStruct*, AllocatorT>& InTraitTypes,
				 const EFlagmark                           InFlagmark,
				 FArchive&                                 Archive);

	/**
	 * Construct a new subject record from an archive
	 * with the trait types supplied explicitly.
	 *
	 * @param InTraitTypes The supplied trait types.
	 * @param Archive The serializing archive. Should be opened
	 * for loading.
	 */
	template < typename AllocatorT >
	FORCEINLINE void
	SerializeBin(const TArray<UScriptStruct*, AllocatorT>& InTraitTypes,
				 FArchive&                                 Archive)
	{
		SerializeBin(InTraitTypes, GetFlagmark(), Archive);
	}

	/// @}
#pragma endregion Serialization

#pragma region Comparison
	/// @name Comparison
	/// @{

	/**
	 * Compare the record to an other record.
	 * 
	 * @param Other The record to compare to.
	 * @return true If the records are equal.
	 * @return false If the records are different.
	 */
	bool
	operator==(const FSubjectRecord& Other) const
	{
		if (UNLIKELY(this == std::addressof(Other)))
		{
			return true;
		}
		if (GetFingerprint() != Other.GetFingerprint())
		{
			return false;
		}
		return Traits == Other.Traits;
	}

	/**
	 * Compare the record to be unequal to an other record.
	 * 
	 * @param Other The record to compare to.
	 * @return true If the records are different.
	 * @return false If the records are the same.
	 */
	bool
	operator!=(const FSubjectRecord& Other) const
	{
		if (UNLIKELY(this == std::addressof(Other)))
		{
			return false;
		}
		if (GetFingerprint() != Other.GetFingerprint())
		{
			return true;
		}
		return Traits != Other.Traits;
	}

	/**
	 * Compare two subject records for equality.
	 * 
	 * @param Other The other subject record to compare to.
	 * @param PortFlags The contextual port flags.
	 * @return The state of examination.
	 */
	bool
	Identical(const FSubjectRecord* Other, uint32 PortFlags) const
	{
		if (UNLIKELY(this == Other)) return true;
		if (UNLIKELY(Other == nullptr)) return false;
		
		return (*this) == (*Other);
	}

	/// @}
#pragma endregion Comparison

}; //-struct FSubjectRecord

template < >
struct TTraitsExtractor<FSubjectRecord>
{

  private:

	const FSubjectRecord& SubjectRecord;

  public:

	FORCEINLINE
	TTraitsExtractor(const FSubjectRecord& InSubjectRecord)
	  : SubjectRecord(InSubjectRecord)
	{}

	FORCEINLINE int32
	Num() const
	{
		return SubjectRecord.TraitsNum();
	}

	FORCEINLINE UScriptStruct*
	TypeAt(const int32 Index) const
	{
		return SubjectRecord.GetTraits()[Index].GetType();
	}

	FORCEINLINE bool
	Contains(UScriptStruct* const Type) const
	{
		return SubjectRecord.Contains(Type);
	}

	FORCEINLINE const void*
	TraitAt(const int32 Index) const
	{
		return SubjectRecord.GetTraits()[Index].GetData();
	}

}; //-struct TTraitsExtractor<FSubjectRecord>

template<>
struct TStructOpsTypeTraits<FSubjectRecord> : public TStructOpsTypeTraitsBase2<FSubjectRecord>
{
	enum 
	{
		WithCopy = true,
		WithPostSerialize = true,
		WithPostScriptConstruct = true,
		WithIdentical = true
	}; 
};

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
