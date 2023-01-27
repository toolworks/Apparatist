/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: TraitRecord.h
 * Created: 2021-07-12 14:42:46
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
#include "HAL/UnrealMemory.h"
#include "UObject/Class.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Serialization/Archive.h"
#include "JsonObjectConverter.h"
#include "Misc/Guid.h"
#include "Templates/SharedPointer.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define TRAIT_RECORD_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusCustomVersion.h"
#include "ApparatusStatus.h"

#ifdef TRAIT_RECORD_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "TraitRecord.generated.h"


// Forward declarations:
class ISubjective;
class FTraitRecordCustomization;
class FTraitRecordDataCustomization;

/**
 * The serializable editable container of the trait.
 */
USTRUCT(BlueprintType, Meta = (HasNativeMake = "ApparatusRuntime.ApparatusFunctionLibrary.MakeDefaultTraitRecord"))
struct APPARATUSRUNTIME_API FTraitRecord
{
	GENERATED_BODY()

  private:

	friend class FTraitRecordCustomization;
	friend class FTraitRecordDataCustomization;

	/**
	 * The trait type.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data",
			  Meta = (AllowPrivateAccess = "true"))
	UScriptStruct* Type = nullptr;

#if WITH_EDITORONLY_DATA

	/**
	 * Should the editor customization
	 * be absolutely updated on a next frame.
	 */
	mutable bool bForceEditorPull = false;

#endif // #if WITH_EDITORONLY_DATA

	/**
	 * The raw trait data.
	 * 
	 * The type must match the one in the FTraitRecord::Type field.
	 */
	mutable void* Data = nullptr;

	/**
	 * Deinitialize the data,
	 * possibly freeing the allocation.
	 */
	void
	DoDestroyData(const bool bFree = true) const
	{
		if (UNLIKELY(Data == nullptr)) return;
		if (LIKELY(Type != nullptr))
		{
			Type->RecursivelyPreload();
			Type->DestroyStruct(Data);
		}

		if (LIKELY(bFree))
		{
			FMemory::Free(Data);
			Data = nullptr;
		}

#if WITH_EDITORONLY_DATA
		bForceEditorPull = true;
#endif
	}

	/**
	 * Make sure the trait data is created
	 * for the current type and return a pointer to it.
	 */
	void*
	DoCreateData() const
	{
		if (LIKELY(Type != nullptr))
		{
			if (UNLIKELY(Data != nullptr)) return Data;

			Type->RecursivelyPreload();
			Data = FMemory::Malloc(FMath::Max(Type->GetStructureSize(), 1), Type->GetMinAlignment());
			Type->InitializeStruct(Data);

#if WITH_EDITORONLY_DATA
			bForceEditorPull = true;
#endif
			return Data;
		}
		return nullptr;
	}

	/**
	 * Recreate the data completely,
	 * essentially destroying it and creating anew.
	 */
	void*
	DoRecreateData() const
	{
		if (LIKELY(Type != nullptr))
		{
			if (LIKELY(Data != nullptr))
			{
				Type->RecursivelyPreload();
				Type->DestroyStruct(Data);
				// Freeing can be done later...

#if WITH_EDITORONLY_DATA
				bForceEditorPull = true;
#endif
			}
		}

		if (UNLIKELY(Type == nullptr))
		{
			// Can't create a data without a type:
			return nullptr;
		}

		Type->RecursivelyPreload();
		// Try to reuse the data here:
		Data = FMemory::Realloc(Data, FMath::Max(Type->GetStructureSize(), 1), Type->GetMinAlignment());
		Type->InitializeStruct(Data);
#if WITH_EDITORONLY_DATA
		bForceEditorPull = true;
#endif
		return Data;
	}

	/**
	 * Get the raw trait data.
	 */
	void*
	DoObtainData() const
	{
		if (UNLIKELY(Type == nullptr))
		{
			return nullptr;
		}
		if (UNLIKELY(Data == nullptr))
		{
			// The data must be changed or created anew...
			return DoCreateData();
		}
		return Data;
	}

  public:

#pragma region Typing
	/// @name Typing
	/// @{

	/**
	 * Get the type of the struct used.
	 * 
	 * @return The type of the used struct.
	 */
	FORCEINLINE UScriptStruct*
	GetType() const
	{
		return Type;
	}

	/// @}
#pragma endregion Typing

#pragma region Getting The Data
	/// @name Getting The Data
	/// @{

	/**
	 * Get the raw data of the trait. Constant version.
	 */
	FORCEINLINE const void*
	GetData() const
	{
		return DoObtainData();
	}

	/**
	 * Get the raw data of the trait.
	 */
	FORCEINLINE void*
	GetData()
	{
#if WITH_EDITORONLY_DATA
		bForceEditorPull = true;
#endif
		return DoObtainData();
	}

	/**
	 * Get a reference to the trait data.
	 * Templated constant version.
	 */
	template < typename T >
	FORCEINLINE const T&
	GetDataRef() const
	{
		check(T::StaticStruct() == GetType());
		return *static_cast<const T*>(GetData());
	}

	/**
	 * Get a reference to the trait data.
	 * Templated version.
	 */
	template < typename T >
	FORCEINLINE T&
	GetDataRef()
	{
		check(T::StaticStruct() == GetType());
		return *static_cast<T*>(GetData());
	}

	/**
	 * Copy the raw data to the specified buffer.
	 * 
	 * @param[out] OutData The output memory buffer to copy the trait to.
	 * Must have enough bytes for the structure in question.
	 * May not be a @c nullptr.
	 * @param[in] bTraitDataInitialized Is the output data initialized?
	 */
	void
	GetData(void* const OutData,
			const bool  bTraitDataInitialized = true) const
	{
		check(Type && OutData);
		
		if (UNLIKELY(!bTraitDataInitialized))
		{
			Type->InitializeStruct(OutData);
		}

		if (LIKELY(Data != nullptr))
		{
			Type->CopyScriptStruct(OutData, Data);
		}
		else
		{
			// Safely clear the output data,
			// if is not available within the record (yet):
			Type->ClearScriptStruct(OutData);
		}
	}

	/**
	 * Copy the raw data to the specified buffer.
	 * Dynamically typed pointer version for UE compatibility.
	 * 
	 * @param[out] OutData The output memory buffer to copy the trait to.
	 * @param[in] bTraitDataInitialized Is the output data initialized?
	 */
	FORCEINLINE void
	GetData(uint8* const OutData,
			const bool   bTraitDataInitialized = true) const
	{
		GetData(static_cast<void* const>(OutData), bTraitDataInitialized);
	}

	/**
	 * Copy the raw data to the specified buffer.
	 * Statically typed pointer version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param[out] OutData The output memory buffer to copy to.
	 * @param[in] bTraitDataInitialized Is the output data initialized?
	 */
	template < typename T >
	FORCEINLINE void
	GetData(T* const   OutData,
			const bool bTraitDataInitialized = true) const
	{
		check(Type == T::StaticStruct());
		GetData(static_cast<void* const>(OutData), bTraitDataInitialized);
	}

	/**
	 * Copy the raw data to the specified buffer.
	 * Statically typed reference version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param[out] OutData The output data receiver.
	 * @param[in] bTraitDataInitialized Is the output data initialized?
	 */
	template < typename T >
	FORCEINLINE void
	GetData(T& OutData, const bool bTraitDataInitialized = true) const
	{
		GetData(&OutData, bTraitDataInitialized);
	}

	/// @}
#pragma endregion Getting The Data

#pragma region Setting
	/// @name Setting
	/// @{

	/**
	 * Set the raw data of the record.
	 * 
	 * @param InData The raw data to set to.
	 * Its allocated size must match
	 * the record's type size. Also must be initialized.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	SetData(const void* const InData)
	{
		if (UNLIKELY(Type == nullptr))
		{
			check(InData == nullptr);
			return EApparatusStatus::Noop;
		}
		check(InData);
		if (UNLIKELY(Data == InData))
		{
			return EApparatusStatus::Noop;
		}
		Type->CopyScriptStruct(DoObtainData(), InData);
#if WITH_EDITORONLY_DATA
		bForceEditorPull = true;
#endif
		return EApparatusStatus::Success;
	}

	/**
	 * Set the raw data of the record.
	 * Compatibility for UE methods.
	 * 
	 * @param InData The raw data to set to.
	 * Its allocated size must match
	 * the record's type size. Also must be initialized.
	 */
	FORCEINLINE EApparatusStatus
	SetData(const uint8* const InData)
	{
		return SetData(static_cast<const void*>(InData));
	}

	/**
	 * Set the data of the record along with its type.
	 * Dynamically-typed version.
	 * 
	 * @param InType The type of
	 * of the trait to set.
	 * @param InData The raw data to set to.
	 * Its allocated size must match
	 * the type's size.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	Set(UScriptStruct* const InType,
		const void* const    InData)
	{
		if (UNLIKELY(InType == nullptr))
		{
			check(InData == nullptr);
			if (Type == nullptr)
			{
				return EApparatusStatus::Noop;
			}
			DoDestroyData(/*bFree=*/false);
			Type = nullptr;
#if WITH_EDITORONLY_DATA
			bForceEditorPull = true;
#endif
			return EApparatusStatus::Success;
		}
		check(InType && InData);
		if (Type == InType)
		{
			// The type is not changed.
			// Copy the new data into the existing data.
			return SetData(InData);
		}
		// The type has changed.
		DoDestroyData(/*bFree=*/false);
		Type = InType;
		Data = FMemory::Realloc(Data, FMath::Max(Type->GetStructureSize(), 1), Type->GetMinAlignment());
		Type->RecursivelyPreload();
		Type->InitializeStruct(Data);
		Type->CopyScriptStruct(Data, InData);
#if WITH_EDITORONLY_DATA
		bForceEditorPull = true;
#endif
		return EApparatusStatus::Success;
	}

	/**
	 * Set the data of the record.
	 * Templated version.
	 * 
	 * @tparam T The type of
	 * of the trait to set.
	 * @param InTrait The trait to set to.
	 */
	template < typename T >
	FORCEINLINE EApparatusStatus
	Set(const T& InTrait)
	{
		return Set(T::StaticStruct(), static_cast<const void*>(&InTrait));
	}

	/// @}
#pragma endregion Setting

	/**
	 * Check if the record is meaningful.
	 */
	FORCEINLINE
	operator bool() const
	{
		return Type != nullptr;
	}

	/**
	 * Serialize the record via an archive.
	 */
	bool
	Serialize(FArchive& Archive)
	{
		Archive.UsingCustomVersion(FApparatusCustomVersion::GUID);
		const auto Version = Archive.CustomVer(FApparatusCustomVersion::GUID);

		if (Archive.IsLoading())
		{
			// Destroy the existing state completely...
			DoDestroyData();
			Type = nullptr;
		}

		// Save the type to deal with its possible change:
		const auto TypeSave = Type;
		Archive << Type;
		const auto bTypeChanged = (Type != TypeSave);
		if (Archive.IsLoading() && Type != nullptr)
		{
			Archive.Preload(Type);
			// Make sure the type is fully there, when we need it:
			Type->RecursivelyPreload();
		}

		const auto bOverhaul = Version >= FApparatusCustomVersion::TraitRecordOverhaul;

#if WITH_EDITORONLY_DATA
		if (LIKELY(!bOverhaul))
		{
			int32 DataGeneration = 0;
			FGuid DataGuid;
			if ((!Archive.IsCooking() || Archive.IsLoading()) && !Archive.IsFilterEditorOnly()) // This data is not needed during run-time.
			{
				Archive << DataGeneration;
				Archive << DataGuid;
			}
		}
#endif // WITH_EDITORONLY_DATA

		static constexpr uint16 AutoSizedTraitSize = 0xFFFF;
		if (Archive.IsLoading())
		{
			// Load the struct's data...
#if WITH_EDITORONLY_DATA
			bForceEditorPull = true;
#endif
			uint16 TraitSize = 0;
			if (LIKELY(bOverhaul))
			{
				Archive << TraitSize;
			}

			if (UNLIKELY(Type == nullptr))
			{
				if (TraitSize > 0)
				{
					if (UNLIKELY(TraitSize == AutoSizedTraitSize))
					{
						UE_LOG(LogApparatus, Error,
							   TEXT("The trait was saved in auto-sized mode (was too large), "
									"but its type is no longer available so perhaps it won't be "
									"properly loaded now. "));
					}
					else
					{
						UE_LOG(LogApparatus, Error,
							   TEXT("The type of the trait record has failed to load. "
									"Maybe it's no longer available. "
									"The trait data will be skipped."));
						Archive.Seek(Archive.Tell() + TraitSize);
					}
				}
			}
			else
			{
				// Remember the position just not to under/overflow in case the structure was resized.
				const auto DataStartPos = Archive.Tell();
				Data = FMemory::Realloc(Data, FMath::Max(Type->GetStructureSize(), 1),
										Type->GetMinAlignment());
				Type->InitializeStruct(Data);
				Type->SerializeItem(Archive, Data, /*Defaults=*/nullptr);
				// Make sure to go to the end of the trait structure...
				if (LIKELY(bOverhaul && (TraitSize != AutoSizedTraitSize)))
				{
					Archive.Seek(DataStartPos + TraitSize);
				}
			}
		}
		else if (Archive.IsSaving())
		{
			const auto TraitSizePos = Archive.Tell();
			uint16 TraitSize = 0;
			Archive << TraitSize;
			if (LIKELY(Type != nullptr))
			{
				const auto DataStartPos = TraitSizePos + sizeof(decltype(TraitSize));
				Type->SerializeItem(Archive, DoObtainData(), /*Defaults=*/nullptr);
				const auto DataEndPos = Archive.Tell();
				if ((DataEndPos - DataStartPos) >= TNumericLimits<uint16>::Max())
				{
					UE_LOG(LogApparatus, Warning,
						   TEXT("The size of the '%s' trait instance is too large (%i). "
								"It will be saved in an auto-sized mode."),
						   *(Type->GetName()),
						   (int)(DataEndPos - DataStartPos));
					TraitSize = AutoSizedTraitSize;
				}
				else
				{
					TraitSize = DataEndPos - DataStartPos;
				}
				Archive.Seek(TraitSizePos);
				Archive << TraitSize;
				Archive.Seek(DataEndPos);
			}
		}

		return true;
	};

	/**
	 * Serialize the record via an archive
	 * using a raw binary approach.
	 * 
	 * @note The results are not backward compatible.
	 * 
	 * @param Archive An archive to serialize to/from.
	 */
	void
	SerializeBin(FArchive& Archive)
	{
		Archive << Type;
		if (LIKELY(Type != nullptr))
		{
			Type->SerializeBin(Archive, GetData());
		}
	};

	/**
	 * Move-assign a trait record.
	 * 
	 * @param InTraitRecord The trait record to move.
	 * @return Returns itself.
	 */
	FTraitRecord&
	operator=(FTraitRecord&& InTraitRecord)
	{
		if (LIKELY(Type != nullptr))
		{
			if (LIKELY(Data != nullptr))
			{
				Type->DestroyStruct(Data);
			}
		}
		if (LIKELY(InTraitRecord.Type != nullptr))
		{
			if (LIKELY(InTraitRecord.Data != nullptr))
			{
				if (LIKELY(Data != nullptr))
				{
					FMemory::Free(Data);
				}
				Data = InTraitRecord.Data;
				InTraitRecord.Data = nullptr;
			}
		}
		Type = InTraitRecord.Type;

#if WITH_EDITORONLY_DATA
		bForceEditorPull = true;
#endif

		return *this;
	}

	/**
	 * Set the trait record equal to an another one.
	 * 
	 * @param InTraitRecord The trait record to set to.
	 * @return Returns itself.
	 */
	FTraitRecord&
	operator=(const FTraitRecord& InTraitRecord)
	{
		if (std::addressof(InTraitRecord) == this)
		{
			return *this;
		}
		const auto InType = InTraitRecord.Type;
		if (Type == InType)
		{
			if (LIKELY(InType != nullptr))
			{
				if (LIKELY(InTraitRecord.Data != nullptr))
				{
					Type->CopyScriptStruct(DoObtainData(), InTraitRecord.Data);
#if WITH_EDITORONLY_DATA
					bForceEditorPull = true;
#endif
				}
				else if (LIKELY(Data != nullptr))
				{
					// Reset the data to its default state:
					Type->ClearScriptStruct(Data);
#if WITH_EDITORONLY_DATA
					bForceEditorPull = true;
#endif
				}
				// The incoming data and the existing data are both nullptr.
				// Do nothing and return:
			}
			// If we are already no-type just do nothing.
		}
		else
		{
			// The type has changed.
			DoDestroyData(/*bFree=*/false);
			if (UNLIKELY(InType != nullptr))
			{
				if (UNLIKELY(InTraitRecord.Data == nullptr))
				{
					// The incoming data is a default one.
					// Reinitialize the existing data, but only if it's
					// already allocated.
					if (LIKELY(Data != nullptr))
					{
						// The data is already allocated, so reallocate it for new needs.
						Data = FMemory::Realloc(Data, FMath::Max(InType->GetStructureSize(), 1), InType->GetMinAlignment());
						InType->InitializeStruct(Data);
					}
				}
				else
				{
					// Some valid incoming data is approaching,
					// so we have to organize our own as well.
					Data = FMemory::Realloc(Data, FMath::Max(InType->GetStructureSize(), 1), InType->GetMinAlignment());
					InType->InitializeStruct(Data);
					InType->CopyScriptStruct(Data, InTraitRecord.Data);
				}
			}
			Type = InType;
#if WITH_EDITORONLY_DATA
			bForceEditorPull = true;
#endif
		}

		return *this;
	}

#pragma region Comparison
	/// @name Comparison
	/// @{

	/**
	 * Compare trait records for equality.
	 * 
	 * @param InTraitRecord The trait record to compare to.
	 * @return Returns @c true if the trait records are equal,
	 * @c false otherwise.
	 */
	bool
	operator==(const FTraitRecord& InTraitRecord) const
	{
		if (UNLIKELY(std::addressof(InTraitRecord) == this))
		{
			return true;
		}
		if (Type != InTraitRecord.Type)
		{
			return false;
		}
		if (UNLIKELY(Type == nullptr))
		{
			return true;
		}
		if (UNLIKELY(Data == InTraitRecord.Data))
		{
			// Both a nullptr:
			return true;
		}
		return Type->CompareScriptStruct(GetData(), InTraitRecord.GetData(), STRUCT_IdenticalNative);
	}

	/**
	 * Compare trait records for inequality.
	 * 
	 * @param InTraitRecord The trait record to compare to.
	 * @return Returns @c true if the trait records are not equal,
	 * @c false otherwise.
	 */
	bool
	operator!=(const FTraitRecord& InTraitRecord) const
	{
		if (UNLIKELY(std::addressof(InTraitRecord) == this))
		{
			return false;
		}
		if (Type != InTraitRecord.Type)
		{
			return true;
		}
		if (UNLIKELY(Type == nullptr))
		{
			return false;
		}
		if (UNLIKELY(Data == InTraitRecord.Data))
		{
			// Both a nullptr:
			return false;
		}
		return !(Type->CompareScriptStruct(GetData(), InTraitRecord.GetData(), STRUCT_IdenticalNative));
	}

	/// @}
#pragma endregion Comparison

#pragma region Hashing
	/// @name Hashing
	/// @{

	/**
	 * Calculate the hash of this trait record.
	 */
	FORCEINLINE uint32
	CalcHash() const
	{
		if (UNLIKELY(Type == nullptr)) return 0;
		// We absolutely need data here since otherwise
		// the default struct instance won't match the allocated one.
		return HashCombine(GetTypeHash(Type), Type->GetStructTypeHash(GetData()));
	}

	/// @}
#pragma endregion Hashing

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Initialize an empty trait record.
	 */
	FORCEINLINE
	FTraitRecord()
	{}

	/**
	 * Move-initialize a trait record.
	 * 
	 * @param InTraitRecord The trait record to move.
	 */
	FORCEINLINE
	FTraitRecord(FTraitRecord&& InTraitRecord)
	  : Type(InTraitRecord.Type)
	  , Data(InTraitRecord.Data)
	{
		InTraitRecord.Data = nullptr;
	}

	/**
	 * Initialize the trait record as a copy
	 * of another one.
	 * 
	 * @param InTraitRecord The trait record to copy.
	 * May be an empty one.
	 */
	FORCEINLINE
	FTraitRecord(const FTraitRecord& InTraitRecord)
	  : Type(InTraitRecord.Type)
	{
		if (LIKELY((InTraitRecord.Type != nullptr)
				&& (InTraitRecord.Data != nullptr)))
		{
			Type->CopyScriptStruct(
				GetData(),
				InTraitRecord.Data);
		}
	}

	/**
	 * Initialize the trait record with its type only.
	 * 
	 * The data is assumed to be a default trait.
	 * 
	 * @param InType The type of the trait to initialize with.
	 * Can be a @c nullptr to initialize an empty record.
	 */
	FORCEINLINE
	FTraitRecord(UScriptStruct* const InType)
	  : Type(InType)
	{}

	/**
	 * Initialize a new trait record with a type and the data.
	 * 
	 * The trait data gets copied and is not stored as a pointer.
	 * 
	 * @param InType The type of the trait.
	 * @param InData The data of the trait to store a copy of.
	 * Must be initialized beforehand.
	 * May be a @c nullptr, of the @p InType parameter is also a @c nullptr.
	 */
	FTraitRecord(UScriptStruct* const InType,
				 const void* const    InData)
	  : Type(InType)
	{
		if (LIKELY(InType != nullptr))
		{
			check(InData != nullptr);
			Type->CopyScriptStruct(GetData(), InData);
		}
	}

	/**
	 * Initialize a new trait record with a trait instance.
	 * Statically-typed version.
	 * 
	 * The trait data gets copied and is not stored as a pointer.
	 * 
	 * @tparam T The type of the trait.
	 * @param InTrait The trait to initialize with.
	 */
	template < typename T >
	FTraitRecord(const T& InTrait)
	  : FTraitRecord(T::StaticStruct(), static_cast<const void*>(&InTrait))
	{}

	/**
	 * Make a trait record from a trait.
	 * 
	 * @tparam T The type of trait to initialize with.
	 * @param InTrait The trait to initialize with.
	 */
	template < typename T >
	static FORCEINLINE FTraitRecord
	Make(const T& InTrait)
	{
		return FTraitRecord(T::StaticStruct(), static_cast<const void*>(&InTrait));
	}

	/**
	 * Make a trait record from a trait passed in as a pointer.
	 * 
	 * @tparam T The type of trait to initialize with.
	 * @param InTrait The trait to initialize with.
	 */
	template < typename T >
	static FORCEINLINE FTraitRecord
	Make(const T* InTrait)
	{
		return FTraitRecord(T::StaticStruct(), static_cast<const void*>(InTrait));
	}

	/**
	 * Deinitialize the trait record.
	 */
	FORCEINLINE virtual
	~FTraitRecord()
	{
		DoDestroyData();
	}

	/// @}
#pragma endregion Initialization

}; // struct FTraitRecord


/**
 * Calculate a hash sum of a trait record.
 * 
 * @param TraitRecord The trait record to hash.
 * @return The resulting hashing sum.
 */
FORCEINLINE uint32
GetTypeHash(const FTraitRecord& TraitRecord)
{
	return TraitRecord.CalcHash();
}

template<>
struct TStructOpsTypeTraits<FTraitRecord> : public TStructOpsTypeTraitsBase2<FTraitRecord>
{
	enum 
	{
		WithCopy = true,
		WithIdenticalViaEquality = true,
		WithSerializer = true
	};
};

/**
 * A strongly-typed trait record.
 * 
 * @tparam T The type of the trait.
 */
template < typename T >
struct APPARATUSRUNTIME_API TTraitRecord
  : protected FTraitRecord
{
	/**
	 * The base struct type.
	 */
	using Super = FTraitRecord;

#pragma region Data Access
	/// @name Data Access
	/// @{

	/**
	 * Get a reference to the trait data.
	 * Constant version.
	 */
	FORCEINLINE const T&
	GetDataRef() const
	{
		return Super::template GetDataRef<T>();
	}

	/**
	 * Get a reference to the trait data.
	 * Mutable version.
	 */
	FORCEINLINE T&
	GetDataRef()
	{
		return Super::template GetDataRef<T>();
	}

	/// @}
#pragma endregion Data Access

#pragma region Assignment
	/// @name Assignment
	/// @{

	/**
	 * Set the data of the record.
	 * 
	 * @param InTrait The trait to set to.
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	Set(const T& InTrait)
	{
		return Super::Set(InTrait);
	}

	/**
	 * Set the data of the record.
	 * 
	 * @param InTrait The trait to set to.
	 * @return Returns itself.
	 */
	FORCEINLINE TTraitRecord&
	operator=(const T& InTrait)
	{
		Set(InTrait);
		return *this;
	}

	/// @}
#pragma endregion Assignment

#pragma region Conversion
	/// @name Conversion
	/// @{

	/// @}
#pragma endregion Conversion

#pragma region Initialization
	/// @name Initialization
	/// @{
	
	/**
	 * Initialize a new instance of the trait record.
	 */
	FORCEINLINE
	TTraitRecord()
	  : Super(T::StaticStruct())
	{}

	/**
	 * Initialize a new record instance as a copy of a trait.
	 * 
	 * @param InTrait The trait to copy.
	 */
	FORCEINLINE
	TTraitRecord(const T& InTrait)
	  : Super(InTrait)
	{}

	/// @}
#pragma endregion Initialization
}; //-struct TTraitRecord

/**
 * The trait extractor specialization for
 * an array of trait types and data.
 */
template < typename A1, typename A2, typename T >
struct TTraitsExtractor<TArray<UScriptStruct*, A1>, TArray<T*, A2>>
{
  private:

	const TArray<UScriptStruct*, A1>& Types;

	const TArray<T*, A2>& Traits;

	bool bHasRecords = false;

  public:

	FORCEINLINE
	TTraitsExtractor(const TArray<UScriptStruct*, A1>& InTypes,
					 const TArray<T*, A2>&             InTraits)
	  : Types(InTypes)
	  , Traits(InTraits)
	{
		checkf(InTypes.Num() == InTraits.Num(), TEXT("The number of types and traits must match."));
		for (const auto InType : InTypes)
		{
			if (InType == FTraitRecord::StaticStruct())
			{
				bHasRecords = true;
				break;
			}
		}
	}

	FORCEINLINE int32
	Num() const
	{
		return Types.Num();
	}

	FORCEINLINE UScriptStruct*
	TypeAt(const int32 Index) const
	{
		if (bHasRecords)
		{
			if (Types[Index] == FTraitRecord::StaticStruct())
			{
				const auto TraitRecord = static_cast<FTraitRecord*>(Traits[Index]);
				return TraitRecord->GetType();
			}
		}
		return Types[Index];
	}

	bool
	Contains(UScriptStruct* const Type) const
	{
		check(Type != FTraitRecord::StaticStruct())
		if (Types.Contains(Type))
		{
			return true;
		}
		if (bHasRecords)
		{
			for (int32 i = 0; i < Types.Num(); ++i)
			{
				if (Types[i] == FTraitRecord::StaticStruct())
				{
					const auto TraitRecord = static_cast<FTraitRecord*>(Traits[i]);
					if (TraitRecord->GetType() == Type)
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	FORCEINLINE void*
	TraitAt(const int32 Index)
	{
		if (bHasRecords)
		{
			if (Types[Index] == FTraitRecord::StaticStruct())
			{
				const auto TraitRecord = static_cast<FTraitRecord*>(Traits[Index]);
				return TraitRecord->GetData();
			}
		}
		return static_cast<void*>(Traits[Index]);
	}

	FORCEINLINE const void*
	TraitAt(const int32 Index) const
	{
		if (bHasRecords)
		{
			if (Types[Index] == FTraitRecord::StaticStruct())
			{
				const auto TraitRecord = static_cast<FTraitRecord*>(Traits[Index]);
				return TraitRecord->GetData();
			}
		}
		return static_cast<const void*>(Traits[Index]);
	}
};

/**
 * The traits extraction specialization
 * for a trait record array.
 */
template < typename A >
struct TTraitsExtractor<TArray<FTraitRecord, A>>
{
  private:

	TArray<FTraitRecord>& Records;

  public:

	FORCEINLINE
	TTraitsExtractor(TArray<FTraitRecord, A>& InRecords)
	  : Records(InRecords)
	{}

	FORCEINLINE int32
	Num() const
	{
		return Records.Num();
	}

	FORCEINLINE UScriptStruct*
	TypeAt(const int32 Index) const
	{
		return Records[Index].GetType();
	}

	FORCEINLINE bool
	Contains(UScriptStruct* const Type) const
	{
		for (const auto& Record : Records)
		{
			if (Record.GetType() == Type) return true;
		}
		return false;
	}

	FORCEINLINE void*
	TraitAt(const int32 Index)
	{
		return static_cast<void*>(Records[Index].GetData());
	}

	FORCEINLINE const void*
	TraitAt(const int32 Index) const
	{
		return static_cast<const void*>(Records[Index].GetData());
	}
};

/**
 * The traits extraction specialization
 * for an immutable trait record array.
 */
template < typename A >
struct TTraitsExtractor<const TArray<FTraitRecord, A>>
{
  private:

	const TArray<FTraitRecord>& Records;

  public:

	FORCEINLINE
	TTraitsExtractor(const TArray<FTraitRecord, A>& InRecords)
	  : Records(InRecords)
	{}

	FORCEINLINE int32
	Num() const
	{
		return Records.Num();
	}

	FORCEINLINE UScriptStruct*
	TypeAt(const int32 Index) const
	{
		return Records[Index].GetType();
	}

	FORCEINLINE bool
	Contains(UScriptStruct* const Type) const
	{
		for (const auto& Record : Records)
		{
			if (Record.GetType() == Type) return true;
		}
		return false;
	}

	FORCEINLINE const void*
	TraitAt(const int32 Index) const
	{
		return static_cast<const void*>(Records[Index].GetData());
	}
};

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
