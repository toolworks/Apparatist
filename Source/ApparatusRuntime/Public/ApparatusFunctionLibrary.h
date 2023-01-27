/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusFunctionLibrary.h
 * Created: Friday, 23rd October 2020 7:00:48 pm
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2020–2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define APPARATUS_FUNCTION_LIBRARY_H_SKIPPED_MACHINE_H
#endif

#include "Fingerprint.h"
#include "Filter.h"
#include "Machine.h"
#include "SubjectiveActorComponent.h"
#include "SubjectiveUserWidget.h"
#include "SubjectHandles4.h"
#include "SubjectHandles8.h"
#include "SubjectHandles16.h"

#ifdef APPARATUS_FUNCTION_LIBRARY_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "ApparatusFunctionLibrary.generated.h"


/**
 * The main Apparatus function library.
 */
UCLASS()
class APPARATUSRUNTIME_API UApparatusFunctionLibrary
  : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

  public:

	#pragma region Subject Handles Arrays

	/**
	 * Add a subject handle to an array.
	 * 
	 * Invalid handles won't be added for
	 * the purpose of economy.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Add Subject", CompactNodeTitle = "ADD"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32
	AddToSubjectHandles4(UPARAM(Ref) FSubjectHandles4& SubjectHandles, FSubjectHandle SubjectHandle)
	{
		return SubjectHandles.Add(SubjectHandle);
	}

	/**
	 * Add a unique subject handle to an array.
	 * 
	 * Invalid handles won't be added for
	 * the purpose of economy.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Add Unique Subject", CompactNodeTitle = "ADDUNIQUE"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32

	AddUniqueToSubjectHandles4(UPARAM(Ref) FSubjectHandles4& SubjectHandles, FSubjectHandle SubjectHandle)
	{
		return SubjectHandles.AddUnique(SubjectHandle);
	}

	/**
	 * Remove a subject handle from the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Remove Subject", CompactNodeTitle = "REMOVE"),
			  Category = "Apparatus|Subject")
	static void
	RemoveFromSubjectHandles4(UPARAM(Ref) FSubjectHandles4& SubjectHandles, FSubjectHandle SubjectHandle);

	/**
	 * Get the number of elements in the array.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Get Length", CompactNodeTitle = "LENGTH"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32
	GetSubjectHandles4Length(UPARAM(Ref) FSubjectHandles4& SubjectHandles)
	{
		return SubjectHandles.Num();
	}

	/**
	 * Get a copy for a subject handle at a specified index.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Get Subject", CompactNodeTitle = "GET"),
			  Category = "Apparatus|Subject")
	static FSubjectHandle
	GetSubjectHandles4At(const FSubjectHandles4& SubjectHandles, const int32 Index)
	{
		return SubjectHandles.At(Index);
	}

	/**
	 * Set a subject handle at a specified index of the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Set Subject", CompactNodeTitle = "SET"),
			  Category = "Apparatus|Subject")
	static void
	SetSubjectHandles4At(UPARAM(Ref) FSubjectHandles4& SubjectHandles,
						 const int32				   Index,
						 const FSubjectHandle&		   SubjectHandle)
	{
		SubjectHandles.At(Index) = SubjectHandle;
	}

	/**
	 * Remove all elements from the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Clear Subjects", CompactNodeTitle = "CLEAR"),
			  Category = "Apparatus|Subject")
	static void
	ClearSubjectHandles4(UPARAM(Ref) FSubjectHandles4& SubjectHandles)
	{
		SubjectHandles.Empty();
	}

	//--------------------

	/**
	 * Add a subject handle to an array.
	 * 
	 * Invalid handles won't be added for
	 * the purpose of economy.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Add Subject", CompactNodeTitle = "ADD"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32
	AddToSubjectHandles8(UPARAM(Ref) FSubjectHandles8& SubjectHandles, FSubjectHandle SubjectHandle)
	{
		return SubjectHandles.Add(SubjectHandle);
	}

	/**
	 * Add a unique subject handle to an array.
	 * 
	 * Invalid handles won't be added for
	 * the purpose of economy.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Add Unique Subject", CompactNodeTitle = "ADDUNIQUE"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32

	AddUniqueToSubjectHandles8(UPARAM(Ref) FSubjectHandles8& SubjectHandles, FSubjectHandle SubjectHandle)
	{
		return SubjectHandles.AddUnique(SubjectHandle);
	}

	/**
	 * Remove a subject handle from the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Remove Subject", CompactNodeTitle = "REMOVE"),
			  Category = "Apparatus|Subject")
	static void
	RemoveFromSubjectHandles8(UPARAM(Ref) FSubjectHandles8& SubjectHandles, FSubjectHandle SubjectHandle);

	/**
	 * Get the number of elements in the array.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Get Length", CompactNodeTitle = "LENGTH"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32
	GetSubjectHandles8Length(UPARAM(Ref) FSubjectHandles8& SubjectHandles)
	{
		return SubjectHandles.Num();
	}

	/**
	 * Get a copy for a subject handle at a specified index.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Get Subject", CompactNodeTitle = "GET"),
			  Category = "Apparatus|Subject")
	static FSubjectHandle
	GetSubjectHandles8At(const FSubjectHandles8& SubjectHandles, const int32 Index)
	{
		return SubjectHandles.At(Index);
	}

	/**
	 * Set a subject handle at a specified index of the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Set Subject", CompactNodeTitle = "SET"),
			  Category = "Apparatus|Subject")
	static void
	SetSubjectHandles8At(UPARAM(Ref) FSubjectHandles8& SubjectHandles,
						 const int32				   Index,
						 const FSubjectHandle&		   SubjectHandle)
	{
		SubjectHandles.At(Index) = SubjectHandle;
	}

	/**
	 * Remove all elements from the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Clear Subjects", CompactNodeTitle = "CLEAR"),
			  Category = "Apparatus|Subject")
	static void
	ClearSubjectHandles8(UPARAM(Ref) FSubjectHandles8& SubjectHandles)
	{
		SubjectHandles.Empty();
	}

	//------------------------------

	/**
	 * Add a subject handle to an array.
	 * 
	 * Invalid handles won't be added for
	 * the purpose of economy.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Add Subject", CompactNodeTitle = "ADD"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32
	AddToSubjectHandles16(UPARAM(Ref) FSubjectHandles16& SubjectHandles, FSubjectHandle SubjectHandle)
	{
		return SubjectHandles.Add(SubjectHandle);
	}

	/**
	 * Add a unique subject handle to an array.
	 * 
	 * Invalid handles won't be added for
	 * the purpose of economy.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Add Unique Subject", CompactNodeTitle = "ADDUNIQUE"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32

	AddUniqueToSubjectHandles16(UPARAM(Ref) FSubjectHandles16& SubjectHandles, FSubjectHandle SubjectHandle)
	{
		return SubjectHandles.AddUnique(SubjectHandle);
	}

	/**
	 * Remove a subject handle from the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Remove Subject", CompactNodeTitle = "REMOVE"),
			  Category = "Apparatus|Subject")
	static void
	RemoveFromSubjectHandles16(UPARAM(Ref) FSubjectHandles16& SubjectHandles, FSubjectHandle SubjectHandle);

	/**
	 * Get the number of elements in the array.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Get Length", CompactNodeTitle = "LENGTH"),
			  Category = "Apparatus|Subject")
	static FORCEINLINE int32
	GetSubjectHandles16Length(UPARAM(Ref) FSubjectHandles16& SubjectHandles)
	{
		return SubjectHandles.Num();
	}

	/**
	 * Get a copy for a subject handle at a specified index.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Get Subject", CompactNodeTitle = "GET"),
			  Category = "Apparatus|Subject")
	static FSubjectHandle
	GetSubjectHandles16At(const FSubjectHandles16& SubjectHandles, const int32 Index)
	{
		return SubjectHandles.At(Index);
	}

	/**
	 * Set a subject handle at a specified index of the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Set Subject", CompactNodeTitle = "SET"),
			  Category = "Apparatus|Subject")
	static void
	SetSubjectHandles16At(UPARAM(Ref) FSubjectHandles16& SubjectHandles,
						 const int32				   Index,
						 const FSubjectHandle&		   SubjectHandle)
	{
		SubjectHandles.At(Index) = SubjectHandle;
	}

	/**
	 * Remove all elements from the array.
	 */
	UFUNCTION(BlueprintCallable,
			  Meta = (DisplayName = "Clear Subjects", CompactNodeTitle = "CLEAR"),
			  Category = "Apparatus|Subject")
	static void
	ClearSubjectHandles16(UPARAM(Ref) FSubjectHandles16& SubjectHandles)
	{
		SubjectHandles.Empty();
	}

#pragma endregion Subject Handles Arrays

	/**
	 * Make a traitmark from a list of trait types.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Make Traitmark From Array", NativeMakeFunc),
			  Category = "Apparatus|Traitmark")
	static FTraitmark
	MakeTraitmark(const TArray<UScriptStruct*>& Traits);

	/**
	 * Make a detailmark from a list of detail classes.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Make Detailmark From Array", NativeMakeFunc),
			  Category = "Apparatus|Detailmark")
	static FDetailmark
	MakeDetailmark(const TArray<TSubclassOf<UDetail>>& Details);

	/**
	 * Make a fingerprint.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Make Fingerprint From Arrays", NativeMakeFunc),
			  Category = "Apparatus|Fingerprint")
	static FFingerprint
	MakeFingerprint(const TArray<UScriptStruct*>&       Traits,
					const TArray<TSubclassOf<UDetail>>& Details,
					UPARAM(Meta=(Bitmask, BitmaskEnum="EFlagmark")) const int32 Flagmark);


#pragma region Trait Record

	/**
	 * Make a default-filled trait record.
	 * 
	 * @param TraitType The type of trait to create.
	 * @param OutTraitRecord The trait record to output to.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Records",
			  Meta = (DisplayName = "Make Default Trait Record",
					  KeyWords = "create"))
	static void
	MakeDefaultTraitRecord(UPARAM(DisplayName="Type")   UScriptStruct* TraitType,
						   UPARAM(DisplayName="Record") FTraitRecord&  OutTraitRecord)
	{
		OutTraitRecord = FTraitRecord(TraitType);
	}

	/**
	 * Make a trait record.
	 * 
	 * @param TraitType The type of trait to create.
	 * @param TraitData The trait data to fill with.
	 * @param OutTraitRecord The trait record to output to.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, CustomThunk, BlueprintInternalUseOnly,
			  Category = "Apparatus|Records",
			  Meta = (DisplayName = "Make Trait Record",
					  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "create"))
	static void
	MakeTraitRecord(UPARAM(DisplayName="Type")   UScriptStruct*        TraitType,
					UPARAM(DisplayName="Trait")  const FGenericStruct& TraitData,
					UPARAM(DisplayName="Record") FTraitRecord&         OutTraitRecord)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_MakeTraitRecord(UScriptStruct* const TraitType,
							const void* const    TraitData,
							FTraitRecord&        OutTraitRecord)
	{
		OutTraitRecord = FTraitRecord(TraitType, TraitData);
	}

	DECLARE_FUNCTION(execMakeTraitRecord)
	{
		P_GET_OBJECT(UScriptStruct, TraitType);
		Stack.StepCompiledIn<FStructProperty>(nullptr);
		const void* const SrcTraitAddr = Stack.MostRecentPropertyAddress;
		P_GET_STRUCT_REF(FTraitRecord, OutTraitRecord);

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_MakeTraitRecord(TraitType, SrcTraitAddr, OutTraitRecord);
		P_NATIVE_END;
	}

#pragma endregion Trait Record

	/**
	 * Get fingerprint's flagmark.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Get Flagmark"),
			  Category = "Apparatus|Fingerprint")
	static UPARAM(Meta=(Bitmask, BitmaskEnum="EFlagmark")) int32
	GetFingerprintFlagmark(const FFingerprint& Fingerprint)
	{
		return (int32)Fingerprint.GetFlagmark();
	}

	/**
	 * Make a filter from the list of details, exclusion details,
	 * and a boot filter.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Make Filter From Arrays", NativeMakeFunc),
			  Category = "Apparatus|Filter")
	static FFilter
	MakeFilter(const TArray<UScriptStruct*>&       Traits,
			   const TArray<TSubclassOf<UDetail>>& Details,
			   const TArray<UScriptStruct*>&       ExcludedTraits,
			   const TArray<TSubclassOf<UDetail>>& ExcludedDetails,
			   UPARAM(Meta=(Bitmask, BitmaskEnum="EFlagmark")) const int32 Flagmark = 0x2/*FM_Booted*/,
			   UPARAM(Meta=(Bitmask, BitmaskEnum="EFlagmark")) const int32 ExcludingFlagmark = 0x1/*FM_Stale*/);

#pragma region Cached Filters

	/**
	 * Make a cached filter with 0 components;
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Meta = (DisplayName = "Make Filter Cache x0"),
			  Category = "Apparatus|Filter")
	static FFilter
	MakeCachedFilter0(const FString&                     Key,
					  UPARAM(Meta=(Bitmask)) const int32 Flagmark)
	{
		check(UMachine::HasInstance());
		FFilter* FilterPtr = UMachine::Instance->FiltersByKeysCache.Find(Key);
		if (LIKELY(FilterPtr))
		{
			return *FilterPtr;
		}

		return UMachine::Instance->FiltersByKeysCache.Add(Key) = FFilter(Flagmark);
	}

	/**
	 * Make a cached filter with 1 component;
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Meta = (DisplayName = "Make Filter Cache x1"),
			  Category = "Apparatus|Filter")
	static FFilter
	MakeCachedFilter1(const FString& Key,
					  UScriptStruct* Trait_0,
			   		  TSubclassOf<UDetail> Detail_0,
					  UScriptStruct* ExcludedTrait_0,
			   		  TSubclassOf<UDetail> ExcludedDetail_0,
					  UPARAM(Meta=(Bitmask)) const int32 Flagmark)
	{
		check(UMachine::HasInstance());
		FFilter* FilterPtr = UMachine::Instance->FiltersByKeysCache.Find(Key);
		if (LIKELY(FilterPtr))
		{
			return *FilterPtr;
		}
		
		TArray<UScriptStruct*, TInlineAllocator<1>> Traits({Trait_0});
		TArray<UScriptStruct*, TInlineAllocator<1>> ExcludedTraits({ExcludedTrait_0});
		TArray<TSubclassOf<UDetail>, TInlineAllocator<1>> Details({Detail_0});
		TArray<TSubclassOf<UDetail>, TInlineAllocator<1>> ExcludedDetails({ExcludedDetail_0});

		return UMachine::Instance->FiltersByKeysCache.Add(Key) = FFilter(Traits, Details, ExcludedTraits, ExcludedDetails,
																		 (EFlagmark)Flagmark);
	}

	/**
	 * Make a cached filter with 2 components;
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Meta = (DisplayName = "Make Filter Cache x2"),
			  Category = "Apparatus|Filter")
	static FFilter
	MakeCachedFilter2(const FString& Key,
					  UScriptStruct* Trait_0,
					  UScriptStruct* Trait_1,
			   		  TSubclassOf<UDetail> Detail_0,
					  TSubclassOf<UDetail> Detail_1,
					  UScriptStruct* ExcludedTrait_0,
					  UScriptStruct* ExcludedTrait_1,
			   		  TSubclassOf<UDetail> ExcludedDetail_0,
					  TSubclassOf<UDetail> ExcludedDetail_1,
					  UPARAM(Meta=(Bitmask)) const int32 Flagmark)
	{
		check(UMachine::HasInstance());
		FFilter* FilterPtr = UMachine::Instance->FiltersByKeysCache.Find(Key);
		if (LIKELY(FilterPtr))
		{
			return *FilterPtr;
		}
		
		TArray<UScriptStruct*, TInlineAllocator<2>> Traits({Trait_0, Trait_1});
		TArray<UScriptStruct*, TInlineAllocator<2>> ExcludedTraits({ExcludedTrait_0, ExcludedTrait_1});
		TArray<TSubclassOf<UDetail>, TInlineAllocator<2>> Details({Detail_0, Detail_1});
		TArray<TSubclassOf<UDetail>, TInlineAllocator<2>> ExcludedDetails({ExcludedDetail_0, ExcludedDetail_1});

		return UMachine::Instance->FiltersByKeysCache.Add(Key) = FFilter(Traits, Details, ExcludedTraits, ExcludedDetails,
																		 (EFlagmark)Flagmark);
	}

	/**
	 * Make a cached filter with 4 components;
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Meta = (DisplayName = "Make Filter Cache x4"),
			  Category = "Apparatus|Filter")
	static FFilter
	MakeCachedFilter4(const FString& Key,
					  UScriptStruct* Trait_0,
					  UScriptStruct* Trait_1,
					  UScriptStruct* Trait_2,
					  UScriptStruct* Trait_3,
			   		  TSubclassOf<UDetail> Detail_0,
					  TSubclassOf<UDetail> Detail_1,
					  TSubclassOf<UDetail> Detail_2,
					  TSubclassOf<UDetail> Detail_3,
					  UScriptStruct* ExcludedTrait_0,
					  UScriptStruct* ExcludedTrait_1,
					  UScriptStruct* ExcludedTrait_2,
					  UScriptStruct* ExcludedTrait_3,
			   		  TSubclassOf<UDetail> ExcludedDetail_0,
					  TSubclassOf<UDetail> ExcludedDetail_1,
					  TSubclassOf<UDetail> ExcludedDetail_2,
					  TSubclassOf<UDetail> ExcludedDetail_3,
					  UPARAM(Meta=(Bitmask)) const int32 Flagmark)
	{
		check(UMachine::HasInstance());
		FFilter* FilterPtr = UMachine::Instance->FiltersByKeysCache.Find(Key);
		if (LIKELY(FilterPtr))
		{
			return *FilterPtr;
		}
		
		TArray<UScriptStruct*, TInlineAllocator<4>> Traits({Trait_0, Trait_1, Trait_2, Trait_3});
		TArray<UScriptStruct*, TInlineAllocator<4>> ExcludedTraits({ExcludedTrait_0, ExcludedTrait_1, ExcludedTrait_2, ExcludedTrait_3});
		TArray<TSubclassOf<UDetail>, TInlineAllocator<4>> Details({Detail_0, Detail_1, Detail_2, Detail_3});
		TArray<TSubclassOf<UDetail>, TInlineAllocator<4>> ExcludedDetails({ExcludedDetail_0, ExcludedDetail_1, ExcludedDetail_2, ExcludedDetail_3});

		return UMachine::Instance->FiltersByKeysCache.Add(Key) = FFilter(Traits, Details, ExcludedTraits, ExcludedDetails,
																		 (EFlagmark)Flagmark);
	}

	/**
	 * Make a cached filter with 8 components;
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Meta = (DisplayName = "Make Filter Cache x8"),
			  Category = "Apparatus|Filter")
	static FFilter
	MakeCachedFilter8(const FString& Key,
					  UScriptStruct* Trait_0,
					  UScriptStruct* Trait_1,
					  UScriptStruct* Trait_2,
					  UScriptStruct* Trait_3,
					  UScriptStruct* Trait_4,
					  UScriptStruct* Trait_5,
					  UScriptStruct* Trait_6,
					  UScriptStruct* Trait_7,
			   		  TSubclassOf<UDetail> Detail_0,
					  TSubclassOf<UDetail> Detail_1,
					  TSubclassOf<UDetail> Detail_2,
					  TSubclassOf<UDetail> Detail_3,
					  TSubclassOf<UDetail> Detail_4,
					  TSubclassOf<UDetail> Detail_5,
					  TSubclassOf<UDetail> Detail_6,
					  TSubclassOf<UDetail> Detail_7,
					  UScriptStruct* ExcludedTrait_0,
					  UScriptStruct* ExcludedTrait_1,
					  UScriptStruct* ExcludedTrait_2,
					  UScriptStruct* ExcludedTrait_3,
					  UScriptStruct* ExcludedTrait_4,
					  UScriptStruct* ExcludedTrait_5,
					  UScriptStruct* ExcludedTrait_6,
					  UScriptStruct* ExcludedTrait_7,
			   		  TSubclassOf<UDetail> ExcludedDetail_0,
					  TSubclassOf<UDetail> ExcludedDetail_1,
					  TSubclassOf<UDetail> ExcludedDetail_2,
					  TSubclassOf<UDetail> ExcludedDetail_3,
					  TSubclassOf<UDetail> ExcludedDetail_4,
					  TSubclassOf<UDetail> ExcludedDetail_5,
					  TSubclassOf<UDetail> ExcludedDetail_6,
					  TSubclassOf<UDetail> ExcludedDetail_7,
					  UPARAM(Meta=(Bitmask)) const int32 Flagmark)
	{
		check(UMachine::HasInstance());
		FFilter* FilterPtr = UMachine::Instance->FiltersByKeysCache.Find(Key);
		if (LIKELY(FilterPtr))
		{
			return *FilterPtr;
		}
		
		TArray<UScriptStruct*, TInlineAllocator<8>> Traits({Trait_0, Trait_1, Trait_2, Trait_3,
															Trait_4, Trait_5, Trait_6, Trait_7});
		TArray<UScriptStruct*, TInlineAllocator<8>> ExcludedTraits({ExcludedTrait_0, ExcludedTrait_1, ExcludedTrait_2, ExcludedTrait_3,
																	ExcludedTrait_4, ExcludedTrait_5, ExcludedTrait_6, ExcludedTrait_7});
		TArray<TSubclassOf<UDetail>, TInlineAllocator<8>> Details({Detail_0, Detail_1, Detail_2, Detail_3,
									 					   Detail_4, Detail_5, Detail_6, Detail_7});
		TArray<TSubclassOf<UDetail>, TInlineAllocator<8>> ExcludedDetails({ExcludedDetail_0, ExcludedDetail_1, ExcludedDetail_2, ExcludedDetail_3,
									 							   ExcludedDetail_4, ExcludedDetail_5, ExcludedDetail_6, ExcludedDetail_7});

		return UMachine::Instance->FiltersByKeysCache.Add(Key) = FFilter(Traits, Details, ExcludedTraits, ExcludedDetails,
																		 (EFlagmark)Flagmark);
	}

#pragma endregion Cached Filters

	/**
	 * Check if a fingerprint matches a filter.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Fingerprint Matches Filter"),
			  Category = "Apparatus|Fingerprint")
	static FORCEINLINE bool
	FingerprintMatches(const FFingerprint& Fingerprint,
					   const FFilter&      Filter)
	{
		return Fingerprint.Matches(Filter);
	}

	/**
	 * Disjunct two bitmasks resulting in a bitmask.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Bit Mask OR", CompactNodeTitle = "|",
			  		  Keywords = "logical | or disjunction",
			  		  CommutativeAssociativeBinaryOperator = "true"),
			  Category = "Apparatus|Bit Mask")
	static FORCEINLINE FBitMask
	BitMaskOr(const FBitMask& A,
			  const FBitMask& B)
	{
		return A | B;
	}

	/**
	 * Conjunct two bit masks resulting in a bit mask.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Meta = (DisplayName = "Bit Mask AND", CompactNodeTitle = "&",
			  		  Keywords = "logical & and conjunction",
			  		  CommutativeAssociativeBinaryOperator = "true"),
			  Category = "Apparatus|Bit Mask")
	static FORCEINLINE FBitMask
	BitMaskAnd(const FBitMask& A,
			   const FBitMask& B)
	{
		return A & B;
	}

	/**
	 * Add a detail class to a fingerprint.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Fingerprint")
	static FFingerprint&
	AddDetailToFingerprint(UPARAM(Ref) FFingerprint&  Fingerprint,
						   const TSubclassOf<UDetail> DetailClass);

	/**
	 * Remove a detail class from a fingerprint.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Fingerprint")
	static FORCEINLINE FFingerprint&
	RemoveDetailFromFingerprint(UPARAM(Ref) FFingerprint&  Fingerprint,
								const TSubclassOf<UDetail> DetailClass);

#pragma region Detail

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Detail",
			  Meta = (Keywords = "subjective"))
	static FORCEINLINE AMechanism*
	GeDetailMechanism(const UDetail* Detail)
	{
		if (UNLIKELY(!ensureMsgf(Detail,
								 TEXT("A valid detail must be supplied in order to get its mechanism."))))
		{
			return nullptr;
		}
		return Detail->GetMechanism();
	}

#pragma endregion Detail

#pragma region Chain

	/**
	 * Enchain multiple iterable sequences (chunks or belts) based
	 * on a filter.
	 *
	 * @param WorldContextObject An object to get a world context from.
	 * @param Filter A filter to enchain under.
	 * @param OutChainId The output chain identifier.
	 * @param Mechanism The explicit mechanism to use.
	 */
	UFUNCTION(BlueprintCallable, Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Enchain",
			  		  WorldContext = "WorldContextObject",
					  AdvancedDisplay = "Mechanism",
			  		  UnsafeDuringActorConstruction))
	static void
	MechanismEnchain(UObject*       WorldContextObject,
					 const FFilter& Filter,
					 int32&         OutChainId,
					 AMechanism*    Mechanism = nullptr);

	UFUNCTION(BlueprintCallable,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Enchain Halted",
			  		  WorldContext = "WorldContextObject",
					  AdvancedDisplay = "Mechanism",
			  		  UnsafeDuringActorConstruction))
	static void
	MechanismEnchainHalted(UObject*             WorldContextObject,
						   UPARAM(Ref) FFilter& Filter,
						   int32&               OutChainId,
						   AMechanism*          Mechanism = nullptr);

	UFUNCTION(BlueprintCallable,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Enchain Booted",
			  		  WorldContext = "WorldContextObject",
					  AdvancedDisplay = "Mechanism",
			  		  UnsafeDuringActorConstruction))
	static void
	MechanismEnchainBooted(UObject*             WorldContextObject,
						   UPARAM(Ref) FFilter& Filter,
						   int32&               OutChainId,
						   AMechanism*          Mechanism = nullptr);

	/**
	 * Get the total iterable number of entities (Subjects/Subjectives) in the chain.
	 *
	 * @param WorldContextObject An object to get a world context from.
	 * @param ChainId The identifier of a chain to examine.
	 * @param Mechanism The explicit mechanism to use.
	 * @return The number of entities available for iteration.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Chain",
			  Meta = (DisplayName = "Iterable Count",
					  WorldContext = "WorldContextObject",
					  CallableWithoutWorldContext,
					  KeyWords = "belt chunk belt chain iterate number length",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE int32
	ChainIterableNum(UObject*    WorldContextObject,
					 const int32 ChainId,
					 AMechanism* Mechanism = nullptr)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			return Mechanism->ObtainChain(ChainId)->IterableNum();
		}
		return 0;
	}

	/**
	 * Begin iterating upon a chain.
	 *
	 * @param WorldContextObject An object to get a world context from.
	 * @param ChainId The identifier of a chain to iterate upon.
	 * @param Mechanism The explicit mechanism to use.
	 * @return Is the current iteration viable?
	 */
	UFUNCTION(BlueprintCallable,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DisplayName = "Begin",
					  WorldContext = "WorldContextObject",
					  KeyWords = "belt chunk chain iterate first start",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE bool
	BeginChain(UObject*    WorldContextObject,
			   const int32 ChainId,
			   AMechanism* Mechanism = nullptr)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			return Mechanism->ObtainChain(ChainId)->Begin();
		}
		return false;
	}

	/**
	 * Advance while iterating upon a chain.
	 *
	 * @param WorldContextObject An object to get a world context from.
	 * @param ChainId The identifier of a chain to iterate upon.
	 * @param Mechanism The explicit mechanism to use.
	 * @return Is the current iteration viable?
	 */
	UFUNCTION(BlueprintCallable,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DisplayName = "Advance",
			  		  WorldContext = "WorldContextObject",
					  KeyWords = "belt chunk chain iterate next go",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static bool
	AdvanceChain(UObject*    WorldContextObject,
				 const int32 ChainId,
				 AMechanism* Mechanism = nullptr);

	/**
	 * Begin iterating or advance while iterating upon a chain.
	 *
	 * @param WorldContextObject An object to get a world context from.
	 * @param ChainId The identifier of a chain to iterate upon.
	 * @param Mechanism The explicit mechanism to use.
	 * @return Is the current iteration viable or an end has come?
	 */
	UFUNCTION(BlueprintCallable,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DisplayName = "Begin or Advance",
					  WorldContext = "WorldContextObject",
					  KeyWords = "belt chunk chain iterate next go start",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE bool
	BeginOrAdvanceChain(UObject*    WorldContextObject,
						const int32 ChainId,
						AMechanism* Mechanism = nullptr)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			return Mechanism->ObtainChain(ChainId)->BeginOrAdvance();
		}
		return false;
	}

	/**
	 * Get the current iteration's subject handle.
	 *
	 * @param WorldContextObject An object to get a world context from.
	 * @param ChainId The identifier of a chain to iterate upon.
	 * @param Mechanism The explicit mechanism to use.
	 * @return The subject handle of the current iteration.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DisplayName = "Get Subject",
					  WorldContext = "WorldContextObject",
					  KeyWords = "belt chain",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE FSubjectHandle
	GetChainSubject(UObject*    WorldContextObject,
					const int32 ChainId,
					AMechanism* Mechanism = nullptr)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			return Mechanism->ObtainChain(ChainId)->GetSubject();
		}
		return FSubjectHandle::Invalid;
	}

	/**
	 * Get the current iteration's subjective (if any).
	 *
	 * @param WorldContextObject An object to get a world context from.
	 * @param ChainId The identifier of a chain to iterate upon.
	 * @param Mechanism An explicit mechanism specifier.
	 * @return The subjective of the current iteration.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DisplayName = "Get Subjective",
					  WorldContext = "WorldContextObject",
					  KeyWords = "belt chain",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE TScriptInterface<ISubjective>
	GetChainSubjective(UObject*    WorldContextObject,
					   const int32 ChainId,
					   AMechanism* Mechanism = nullptr)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			return TScriptInterface<ISubjective>(Cast<UObject>(Mechanism->ObtainChain(ChainId)->GetSubjective()));
		}
		return TScriptInterface<ISubjective>(nullptr);
	}

	/**
	 * Get an existing trait structure from the current chain
	 * cursor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DisplayName = "Get Chunk Chain Trait",
			  		  CustomStructureParam = "OutTraitData",
					  AutoCreateRefTerm = "OutTraitData",
					  WorldContext = "WorldContextObject",
					  KeyWords = "find",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static void
	GetChainTrait(UObject*        WorldContextObject,
				  const int32     ChainId,
				  UScriptStruct*  TraitType,
				  FGenericStruct& OutTraitData,
				  AMechanism*     Mechanism = nullptr)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_GetChainTrait(UObject* const WorldContextObject,
						  const int32&   ChainId,
						  UScriptStruct* TraitType,
						  void* const    OutTraitData,
						  AMechanism*    Mechanism)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			ensure(OK(Mechanism->ObtainChain(ChainId)->GetSubject().GetTrait(TraitType, OutTraitData)));
		}
	}

	DECLARE_FUNCTION(execGetChainTrait)
	{
		P_GET_OBJECT  (UObject,       WorldContextObject);
		P_GET_PROPERTY(FIntProperty,  ChainId);
		P_GET_OBJECT  (UScriptStruct, TraitType);

		Stack.StepCompiledIn<FStructProperty>(nullptr);
		void* SrcTraitAddr = Stack.MostRecentPropertyAddress;

		P_GET_OBJECT  (AMechanism,    Mechanism);

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_GetChainTrait(WorldContextObject, ChainId, TraitType, SrcTraitAddr, Mechanism);
		P_NATIVE_END;
	}

	/**
	 * Get an existing trait structure from the current chain
	 * cursor with a specific index hinted.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DisplayName = "Get Chain Trait (Hinted)",
					  WorldContext = "WorldContextObject",
			  		  CustomStructureParam = "OutTraitData",
					  AutoCreateRefTerm = "OutTraitData",
					  KeyWords = "find",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static void
	GetChainTraitHinted(UObject*        WorldContextObject,
						const int32     ChainId,
						UScriptStruct*  TraitType,
						const int32     TraitIndex,
						FGenericStruct& OutTraitData,
						AMechanism*     Mechanism = nullptr)
	{
		checkNoEntry();
	}

	static void
	Generic_GetChainTraitHinted(UObject* const WorldContextObject,
								const int32    ChainId,
								UScriptStruct* TraitType,
								const int32    TraitIndex,
								void* const    OutTraitData,
								AMechanism*    Mechanism)
	{
		check(TraitType);
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism != nullptr))
		{
			EnsureOK(Mechanism->ObtainChain(ChainId)->GetTraitHinted<EParadigm::DefaultBlueprint>(TraitType, TraitIndex, OutTraitData));
		}
	}

	DECLARE_FUNCTION(execGetChainTraitHinted)
	{
		P_GET_OBJECT  (UObject,       WorldContextObject);
		P_GET_PROPERTY(FIntProperty,  ChainId);
		P_GET_OBJECT  (UScriptStruct, TraitType);
		P_GET_PROPERTY(FIntProperty,  TraitIndex);

		Stack.StepCompiledIn<FStructProperty>(nullptr);
		void* DstTraitAddr = Stack.MostRecentPropertyAddress;
		check(DstTraitAddr);

		P_GET_OBJECT  (AMechanism, Mechanism);

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_GetChainTraitHinted(WorldContextObject, ChainId, TraitType, TraitIndex, DstTraitAddr, Mechanism);
		P_NATIVE_END;
	}

	/**
	 * Get a detail from a current chain iteration.
	 * 
	 * @param WorldContextObject An object to get a world context from.
	 * @param ChainId The identifier of a chain to get from.
	 * @param DetailClass The class of the detail to get.
	 * @param Mechanism The specific mechanism to use.
	 * @return The detail of the specified type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DeterminesOutputType = "DetailClass",
					  WorldContext = "WorldContextObject",
					  DisplayName = "Get Detail",
					  Keywords = "chain chunk belt",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE UDetail*
	GetChainDetail(UObject*             WorldContextObject,
				   const int32          ChainId,
				   TSubclassOf<UDetail> DetailClass,
				   AMechanism*          Mechanism = nullptr)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			return Mechanism->ObtainChain(ChainId)->GetDetail(DetailClass);
		}
		return nullptr;
	}

	/**
	 * Get a detail from a chain while hinting its index.
	 * 
	 * @param WorldContextObject An object to get a world context from.
	 * @param ChainId The identifier of a chain to iterate upon.
	 * @param DetailClass A class of the detail to get.
	 * @param DetailIndexHint The hinted index of the detail to get.
	 * @param Mechanism The explicit mechanism specification.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Chain",
			  Meta = (DeterminesOutputType = "DetailClass",
					  WorldContext = "WorldContextObject",
					  DisplayName = "Get Detail (Hinted)",
					  Keywords = "hinted chain chunk belt",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE UDetail*
	GetChainDetailHinted(UObject*             WorldContextObject,
						 const int32          ChainId,
						 TSubclassOf<UDetail> DetailClass,
						 const int32          DetailIndexHint,
						 AMechanism*          Mechanism = nullptr)
	{
		check(DetailClass);
		// DetailClass is used to determine the output pin type only here,
		// so we don't pass it down the line...
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			const auto Outcome = Mechanism->ObtainChain(ChainId)->GetDetailHinted<EParadigm::DefaultBlueprint>(DetailClass, DetailIndexHint);
			if (EnsureOK(Outcome))
			{
				return Outcome;
			}
		}
		return nullptr;
	}

#pragma endregion Chain

#pragma region Subjects

	/**
	 * Spawn a new subject within a machine.
	 * 
	 * This function may not be used in the construction scripts.
	 * 
	 * @param WorldContextObject An object for the world context.
	 * @param Flagmark The initial flagmark to spawn with.
	 * You are not allowed to use system flags like Booted and Online here.
	 * @param OutSubjectHandle The resulting subject handle.
	 * @param Mechanism The explicit mechanism specification.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Spawn Subject",
					  WorldContext = "WorldContextObject",
					  KeyWords = "subject add create new entity +",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static void
	SpawnMachineSubject(                                                  UObject*        WorldContextObject,
						UPARAM(Meta=(Bitmask, BitmaskEnum = "EFlagmark")) const int32     Flagmark,
						UPARAM(Meta=(DisplayName = "Subject"))            FSubjectHandle& OutSubjectHandle,
																		  AMechanism*     Mechanism = nullptr)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			const auto Outcome = Mechanism->SpawnSubject<EParadigm::DefaultBlueprint>((EFlagmark)Flagmark);
			if (EnsureOK(Outcome))
			{
				OutSubjectHandle = Outcome;
			}
		}
	}

	/**
	 * Spawn a new subject based on a record.
	 * 
	 * This function may not be used in the construction scripts.
	 * 
	 * @param WorldContextObject An object for the world context.
	 * @param Record The subject record to clone.
	 * @param Mechanism The explicit mechanism to use.
	 * @return The resulting handle.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Spawn Subject From Record",
					  WorldContext = "WorldContextObject",
					  KeyWords = "subject add create new entity +",
					  AdvancedDisplay = "Mechanism",
					  UnsafeDuringActorConstruction))
	static FSubjectHandle
	SpawnMachineSubjectFromRecord(
		UObject*              WorldContextObject,
		const FSubjectRecord& Record,
		AMechanism*           Mechanism = nullptr)
	{
		if (LIKELY(Mechanism == nullptr))
		{
			Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
		}
		if (ensure(Mechanism))
		{
			const auto Subject = Mechanism->SpawnSubject<EParadigm::DefaultBlueprint>(Record);
			if (EnsureOK(Subject))
			{
				return Subject;
			}
		}
		return FSubjectHandle::Invalid;
	}

	/**
	 * Get the invalid subject handle constant.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Invalid",
					  KeyWords = "null subject"))
	static FSubjectHandle
	GetInvalidSubjectHandle()
	{
		return FSubjectHandle::Invalid;
	}

	/**
	 * Check if the subject handle is a valid one.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Is Valid",
					  KeyWords = "check invalid"))
	static bool
	IsSubjectHandleValid(UPARAM(Ref) FSubjectHandle& SubjectHandle)
	{
		return SubjectHandle.IsValid();
	}

	/**
	 * Reset the subject handle, making it invalid.
	 * 
	 * The subject itself won't be despawned though.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Reset Handle",
					  KeyWords = "clear invalid"))
	static void
	ResetSubjectHandle(UPARAM(Ref) FSubjectHandle& SubjectHandle)
	{
		SubjectHandle.ResetHandle();
	}

	/**
	 * Get the current mechanism of a subject.
	 * 
	 * Will return an invalid mechanism if the
	 * passed subject handle is invalid.
	 *
	 * @param SubjectHandle The subject to examine.
	 * @return The current mechanism of the subject.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Mechanism",
					  KeyWords = "world"))
	static FORCEINLINE AMechanism*
	GetSubjectMechanism(UPARAM(Ref, Meta=(DisplayName = "Subject")) const FSubjectHandle& SubjectHandle)
	{
		return SubjectHandle.GetMechanism();
	}

	/**
	 * Set the mechanism of a subject.
	 * 
	 * Move the subject to a (new) mechanism.
	 * The subjective part (if any) gets moved to a new mechanism's belt as well.
	 *
	 * @param SubjectHandle The subject to transition.
	 * Must be valid.
	 * @param Mechanism The mechanism to move the subject to.
	 * Must be valid.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Set Mechanism",
					  KeyWords = "world",
					  UnsafeDuringActorConstruction))
	static void
	SetSubjectMechanism(UPARAM(Ref, Meta=(DisplayName = "Subject")) FSubjectHandle& SubjectHandle,
																	AMechanism*     Mechanism)
	{
		if (ensureMsgf(Mechanism, TEXT("A valid mechanism must be provided in order to move a subject to.")))
		{
			ensure(OK(SubjectHandle.SetMechanism<EParadigm::DefaultBlueprint>(Mechanism)));
		}
	}

	/**
	 * Check if two handles point at the same subject.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Handles Equal", CompactNodeTitle = "==",
					  KeyWords = "same check subject"))
	static bool
	AreSubjectHandlesEqual(const FSubjectHandle& SubjectA,
						   const FSubjectHandle& SubjectB)
	{
		return SubjectA == SubjectB;
	}

	/**
	 * Check if two handles point at different subjects.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Handles Not Equal", CompactNodeTitle = "!=",
					  KeyWords = "different check subject"))
	static bool
	AreSubjectHandlesNotEqual(const FSubjectHandle& SubjectA,
							  const FSubjectHandle& SubjectB)
	{
		return SubjectA != SubjectB;
	}

#pragma endregion Subjects

#pragma region Subject Records

	/**
	 * Create a new subject record dump from a subject.
	 * 
	 * @param SubjectHandle The subject to dump.
	 * @param FlagmarkMask The flagmark mask to filter the flags.
	 * @param OutSubjectRecord The record to output to.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Records",
			  Meta = (DisplayName = "Dump Subject", KeyWords = "create record"))
	static void
	DumpSubjectToRecord(UPARAM(Ref, Meta=(DisplayName = "Subject"))       FSubjectHandle& SubjectHandle,
						UPARAM(Meta=(DisplayName = "Record"))             FSubjectRecord& OutSubjectRecord,
						UPARAM(Meta=(Bitmask, BitmaskEnum = "EFlagmark")) int32           FlagmarkMask = 0x7FFFFFE0)
	{
		static_assert(0x7FFFFFE0 == FM_AllUserLevel, "The all user flag mask must match its literal.");
		OutSubjectRecord = FSubjectRecord(SubjectHandle, (EFlagmark)FlagmarkMask);
	}

	/**
	 * Create a new subject record dump from a masked subject.
	 * 
	 * @param SubjectHandle The subject to dump.
	 * @param Mask The filter used as a mask.
	 * @param OutSubjectRecord The record to output to.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Records",
			  Meta = (DisplayName = "Dump Masked Subject", KeyWords = "create record mask"))
	static void
	DumpMaskedSubjectToRecord(UPARAM(Ref, Meta=(DisplayName = "Subject")) FSubjectHandle& SubjectHandle,
							  UPARAM(Meta=(DisplayName = "Record"))       FSubjectRecord& OutSubjectRecord,
																		  const FFilter&  Mask)
	{
		OutSubjectRecord = FSubjectRecord(SubjectHandle, Mask);
	}

	/**
	 * Create a new subject record dump from a subject.
	 * 
	 * @param SubjectHandle The subject to convert (dump).
	 * @return The converted record dump.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Records",
			  Meta = (DisplayName = "Dumped Subject", CompactNodeTitle = "->", BlueprintAutocast, KeyWords = "record convert"))
	static FSubjectRecord
	Conv_SubjectHandleToSubjectRecord(UPARAM(Meta=(DisplayName = "Subject")) const FSubjectHandle& SubjectHandle)
	{
		return FSubjectRecord(SubjectHandle);
	}

#pragma endregion Subject Records

#pragma region Subjectives

	/**
	 * Get the current mechanism of a subject.
	 * 
	 * Will return an invalid mechanism if the
	 * passed subject handle is invalid.
	 *
	 * @param Subjective The subjective to examine.
	 * Must be a valid one.
	 * @return The current mechanism of the subject.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Get Mechanism (Subjective)",
					  KeyWords = "world"))
	static FORCEINLINE AMechanism*
	GetSubjectiveMechanism(TScriptInterface<ISubjective> Subjective)
	{
		return Subjective->GetMechanism();
	}

	/**
	 * Set the mechanism of a subjective.
	 * 
	 * Move the subjective to a (new) mechanism.
	 * The subject part (if any) gets moved to a new mechanism's belt as well.
	 * This method can be called during construction phase in order
	 * to override the spawning mechanism.
	 *
	 * @param Subjective The subjective to transition. Must be valid.
	 * @param Mechanism The mechanism to move the subjective to. Must be valid.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Set Mechanism (Subjective)",
					  KeyWords = "world"))
	static void
	SetSubjectiveMechanism(TScriptInterface<ISubjective> Subjective,
						   AMechanism*                   Mechanism)
	{
		if (ensureMsgf(Mechanism, TEXT("A valid mechanism must be provided in order to move a subjective to it.")))
		{
			ensure(OK(Subjective->SetMechanism<EParadigm::DefaultBlueprint>(Mechanism)));
		}
	}

	/**
	 * Get the current fingerprint of a subjective.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Get Fingerprint (Subjective)",
					  Keywords = "subject subjective traitmark"))
	static void
	GetSubjectiveFingerprint(TScriptInterface<ISubjective> Subjective,
							 FFingerprint&                 OutFingerprint)
	{
		if (LIKELY(ensureMsgf(Subjective, TEXT("The Subjective must be set."))))
		{
			OutFingerprint = Subjective->GetFingerprint();
		}
	}

	/**
	 * Cast subjective to an actor component.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Subjective")
	static FORCEINLINE USubjectiveActorComponent*
	GetSubjectiveComponent(TScriptInterface<ISubjective> Subjective)
	{
		return Cast<USubjectiveActorComponent>(Subjective.GetObject());
	}

	/**
	 * Get the actor of the subjective in question.
	 * 
	 * @param Subjective The subjective to examine.
	 * Must be valid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Get Actor (Subjective)",
					  Keywords = "subject subjective"))
	static FORCEINLINE AActor*
	GetSubjectiveActor(TScriptInterface<ISubjective> Subjective)
	{
		if (LIKELY(ensureMsgf(Subjective, TEXT("A valid subjective must be provided."))))
		{
			return Subjective->GetActor();
		}
		return nullptr;
	}

	/**
	 * Get a subjective from an object.
	 * 
	 * This method is smart enough to search for subjective within
	 * an actor's components (if any).
	 * 
	 * @param Object The object to examine. If is an actor, its components are also examined.
	 * @return The subjective derived from the object. Returns an invalid one if the search has failed.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Get Subjective",
					  Keywords = "search find discover derive"))
	static TScriptInterface<ISubjective>
	GetObjectSubjective(UObject* Object)
	{
		if (LIKELY(ensureMsgf(Object, TEXT("A valid object must be provided."))))
		{
			TScriptInterface<ISubjective> Subjective(Object);
			if (Subjective) 
			{
				return Subjective;
			}
			auto Actor = Cast<AActor>(Object);
			if (Actor)
			{
				for (const auto Component : Actor->GetComponents())
				{
					TScriptInterface<ISubjective> ComponentSubjective(Component);
					if (ComponentSubjective)
					{
						return ComponentSubjective;
					}
				}
			}
		}
		return nullptr;
	}

	/**
	 * Get the widget in question, if this subjective is
	 * actually an user widget.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Get Widget (Subjective)",
					  Keywords = "subject subjective gui ui hud"))
	static FORCEINLINE UUserWidget*
	GetSubjectiveWidget(TScriptInterface<ISubjective> Subjective)
	{
		return Cast<UUserWidget>(Subjective.GetObject());
	}

#pragma endregion Subjectives

#pragma region Flagmark

	/**
	 * Get the flagmark of the subject.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid one.
	 * @return The current flagmark of the subject.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Get Flagmark",
					  KeyWords = "subject flags bits",
					  UnsafeDuringActorConstruction))
	static UPARAM(Meta=(Bitmask, BitmaskEnum = "EFlagmark")) int32
	GetSubjectFlagmark(UPARAM(Meta=(DisplayName = "Subject")) const FSubjectHandle& SubjectHandle)
	{
		if (UNLIKELY(!ensureMsgf(SubjectHandle,
								 TEXT("Invalid subject handle to get the flagmark of. "
								 	  "Was it despawned already?"))))
		{
			return (int32)SubjectHandle.GetFlagmark();
		}
		return 0x0;
	}

	/**
	 * Set the flagmark of a subject.
	 * 
	 * @param SubjectHandle The subject to change. Must be a valid one.
	 * @param Flagmark The flagmark to set.
	 * You are not allowed to use system flags like Booted and Online here.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Set Flagmark",
					  KeyWords = "subject flags bits",
					  UnsafeDuringActorConstruction))
	static void
	SetSubjectFlagmark(UPARAM(Meta=(DisplayName = "Subject"))            const FSubjectHandle& SubjectHandle,
					   UPARAM(Meta=(Bitmask, BitmaskEnum = "EFlagmark")) const int32           Flagmark = 0x0)
	{
		const auto RealFlagmark = (EFlagmark)Flagmark;
		if (LIKELY(ensureMsgf(SubjectHandle, TEXT("The subject to set flagmark of must be valid.")) &&
				   ensureMsgf(!HasSystemLevelFlags(RealFlagmark),
							  TEXT("System-level flags are not allowed to be set."))))
		{
			const auto Outcome = SubjectHandle.SetFlagmark<EParadigm::DefaultBlueprint>(RealFlagmark);
			ensureMsgf(OK(Outcome), TEXT("Failed to set the flagmark of the subject: %s"), *ToString(Outcome));
		}
	}

	/**
	 * Get the state of a flag of the subject.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid one.
	 * @param Flag The flagmark bit to examine.
	 * @return The state of the flag.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Has Flag",
					  KeyWords = "get subject flag bit",
					  UnsafeDuringActorConstruction))
	static bool
	HasSubjectFlag(UPARAM(Meta=(DisplayName = "Subject")) const FSubjectHandle& SubjectHandle,
				   const EFlagmarkBit Flag = EFlagmarkBit::A)
	{
		if (UNLIKELY(ensureMsgf(SubjectHandle,
								TEXT("Invalid subject handle to get the flag from. "
								 	 "Was it despawned already?"))))
		{
			return SubjectHandle.HasFlag(Flag);
		}
		return false;
	}

	/**
	 * Set/clear the flag of the subject.
	 * 
	 * @param SubjectHandle The subject handle to set for. Must be a valid one.
	 * @param Flag The flagmark bit to set. You are not allowed to use system flags like Booted and Online here.
	 * @param bState The flag state to set to.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Set Flag",
					  KeyWords = "subject flag bit clear",
					  UnsafeDuringActorConstruction))
	static void
	SetSubjectFlag(UPARAM(Meta=(DisplayName = "Subject")) const FSubjectHandle& SubjectHandle,
				   const EFlagmarkBit Flag = EFlagmarkBit::A,
				   bool bState = true)
	{
		if (LIKELY(ensureMsgf(SubjectHandle, TEXT("The subject handle to set the flag for must be valid. "
												  "Was it already despawned?")) && 
				   ensureMsgf(!IsSystemLevel(Flag), TEXT("The flag to set must not be a system-level one."))))
		{
			const auto Outcome = SubjectHandle.SetFlag<EParadigm::DefaultBlueprint>(Flag, bState);
			ensureMsgf(OK(Outcome), TEXT("Failed to set the flagmark of the subject: %s"), *ToString(Outcome));
		}
	}

	/**
	 * Toggle the flag of the subject.
	 * 
	 * @param SubjectHandle The subject handle to change for. Must be a valid one.
	 * @param Flag The flagmark bit to toggle. You are not allowed to use system flags like Booted and Online here.
	 * @return The new state of the flag.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Toggle Flag",
					  KeyWords = "subject bit change",
					  UnsafeDuringActorConstruction))
	static bool
	ToggleSubjectFlag(UPARAM(Meta=(DisplayName = "Subject")) const FSubjectHandle& SubjectHandle,
					  const EFlagmarkBit Flag = EFlagmarkBit::A)
	{
		if (LIKELY(ensureMsgf(SubjectHandle, TEXT("The subject handle to toggle the flag for must be valid. "
												  "Was it already despawned?")) && 
				   ensureMsgf(!IsSystemLevel(Flag), TEXT("The flag to toggle must not be a system-level one."))))
		{
			return EnsuredPayload(SubjectHandle.ToggleFlag<EParadigm::DefaultBlueprint>(Flag));
		}
		return false;
	}

	/**
	 * Get the flagmark of the subjective.
	 * 
	 * Unlike the subject version this is safe to be called
	 * during the construction script.
	 * 
	 * @param Subjective The subjective to examine. Must be a valid one.
	 * @return The current flagmark of the subjective.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Get Flagmark",
					  KeyWords = "subjective flags bits"))
	static UPARAM(Meta=(Bitmask, BitmaskEnum = "EFlagmark")) int32
	GetSubjectiveFlagmark(TScriptInterface<ISubjective> Subjective)
	{
		if (UNLIKELY(!ensureMsgf(Subjective,
								 TEXT("Invalid subjective to get the flagmark of. "
								 	  "Was it already destroyed?"))))
		{
			return 0x0;
		}
		return (int32)Subjective->GetFlagmark();
	}

	/**
	 * Get the subject handle for a subjective.
	 * 
	 * If the subjective is not valid, returns an
	 * invalid subject handle.
	 * 
	 * @param Subjective The subjective to get a handle from.
	 * @return A handle to a subjective's subject.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Get Handle",
					  KeyWords = "subject pointer internal"))
	static FSubjectHandle
	GetSubjectiveHandle(TScriptInterface<ISubjective> Subjective)
	{
		if (UNLIKELY(Subjective == nullptr))
		{
			return FSubjectHandle::Invalid;
		}
		return Subjective->GetHandle();
	}

	/**
	 * Convert a subjective to a low-level subject handle.
	 * 
	 * This acquires the internal handle.
	 * 
	 * @param Subjective The subjective to convert.
	 * @return The subject handle.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "To Subject Handle", CompactNodeTitle = "->", BlueprintAutocast, KeyWords = "convert entity"))
	static FSubjectHandle
	Conv_SubjectiveToSubjectHandle(TScriptInterface<ISubjective> Subjective)
	{
		if (UNLIKELY(Subjective == nullptr))
		{
			return FSubjectHandle::Invalid;
		}
		return Subjective->GetHandle();
	}

	/**
	 * Set the flagmark of the subjective.
	 * 
	 * Unlike the subject version this is safe to be called
	 * during the construction script.
	 * 
	 * @param Subjective The subjective to change. Must be a valid one.
	 * @param Flagmark The flagmark to set.
	 * You are not allowed to use system-level flags here.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Set Flagmark",
					  KeyWords = "subjective flags bits"))
	static void
	SetSubjectiveFlagmark(TScriptInterface<ISubjective> Subjective,
						  UPARAM(Meta=(Bitmask, BitmaskEnum = "EFlagmark")) const int32 Flagmark = 0x0)
	{
		const auto RealFlagmark = (EFlagmark)Flagmark;
		if (LIKELY(ensureMsgf(Subjective, TEXT("The subjective must be valid.")) &&
				   ensureMsgf(!HasSystemLevelFlags(RealFlagmark),
							  TEXT("System-level flags are not allowed to be set."))))
		{
			const auto Outcome = Subjective->SetFlagmark<EParadigm::DefaultBlueprint>(RealFlagmark);
			ensureMsgf(OK(Outcome),
					   TEXT("Failed to set the flagmark of the subjective: %s"), *ToString(Outcome));
		}
	}

	/**
	 * Get the state of a flag in the subjective.
	 * 
	 * @param Subjective The subjective to examine. Must be a valid one.
	 * @param Flag The flagmark bit to examine.
	 * @return The state of the flag.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Has Flag",
					  KeyWords = "get subjective bit",
					  UnsafeDuringActorConstruction))
	static bool
	HasSubjectiveFlag(TScriptInterface<ISubjective> Subjective,
					  const EFlagmarkBit            Flag = EFlagmarkBit::A)
	{
		if (LIKELY(ensureMsgf(Subjective,
							  TEXT("The subjective to set the flag for must be valid. "
								   "Was it already destroyed?"))))
		{
			return Subjective->HasFlag(Flag);
		}
		return false;
	}

	/**
	 * Set/clear a flag of the subjective.
	 * 
	 * You are not allowed to set any of the system-level flags.
	 * 
	 * Unlike subject-based version this is safe
	 * to be used during the construction script.
	 * 
	 * @param Subjective The subjective to change. Must be a valid one.
	 * @param Flag The flagmark bit to set. You are not allowed to use system flags like Booted and Online here.
	 * @param bState The flag state to set to.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Set Flag",
					  KeyWords = "subjective bit clear"))
	static void
	SetSubjectiveFlag(TScriptInterface<ISubjective> Subjective,
					  const EFlagmarkBit            Flag = EFlagmarkBit::A,
					  bool                          bState = true)
	{
		if (LIKELY(ensureMsgf(Subjective, TEXT("The subjective to set the flag for must be valid. "
											   "Was it already destroyed?")) && 
				   ensureMsgf(!IsSystemLevel(Flag), TEXT("The flag to set must not be a system-level one."))))
		{
			const auto Outcome = Subjective->SetFlag<EParadigm::DefaultBlueprint>(Flag, bState);
			ensureMsgf(OK(Outcome),
					   TEXT("Failed to set the flag of the subjective: %s"), *ToString(Outcome));
		}
	}

	/**
	 * Toggle a flag of the subjective.
	 * 
	 * You are not allowed to change any of the system-level flags.
	 * 
	 * Unlike subject-based version this is safe
	 * to be used during the construction script.
	 * 
	 * @param Subjective The subjective to change. Must be a valid one.
	 * @param Flag The flag bit to change. You are not allowed to use system flags like Booted and Online here.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Toggle Flag",
					  KeyWords = "subjective bit change"))
	static void
	ToggleSubjectiveFlag(TScriptInterface<ISubjective> Subjective,
						 const EFlagmarkBit            Flag = EFlagmarkBit::A)
	{
		if (LIKELY(ensureMsgf(Subjective, TEXT("The subjective to toggle the flag for must be valid. "
											   "Was it already destroyed?")) && 
				   ensureMsgf(!IsSystemLevel(Flag), TEXT("The flag to toggle must not be a system-level one."))))
		{
			Subjective->ToggleFlag(Flag);
		}
	}

#pragma endregion Flagmark

#pragma region Traits

	/**
	 * Get a trait from a subjective.
	 * 
	 * @param Subjective The subjective to get the trait from.
	 * @param TraitType The type of the trait to get.
	 * @param TraitData The trait got.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Get Trait",
			  		  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "subjective find"))
	static void
	GetSubjectiveTrait(                            TScriptInterface<ISubjective> Subjective,
					   UPARAM(DisplayName="Type")  UScriptStruct*                TraitType,
					   UPARAM(DisplayName="Trait") FGenericStruct&               TraitData)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_GetSubjectiveTrait(TScriptInterface<ISubjective> Subjective,
							   UScriptStruct* const          TraitType,
							   void* const                   TraitData)
	{
		if (ensureMsgf(Subjective, TEXT("A valid subjective must be supplied in order to get its trait.")))
		{
			ensure(OK(Subjective->GetTrait(TraitType, TraitData)));
		}
	}

	DECLARE_FUNCTION(execGetSubjectiveTrait)
	{
		P_GET_TINTERFACE(ISubjective, Subjective);
		P_GET_OBJECT(UScriptStruct, TraitType);

		Stack.StepCompiledIn<FStructProperty>(nullptr);
		void* const DstTraitAddr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_GetSubjectiveTrait(Subjective, TraitType, DstTraitAddr);
		P_NATIVE_END;
	}

	/**
	 * Set a trait structure for a subjective.
	 * 
	 * @param Subjective The subjective to set the trait for.
	 * @param TraitType The type of trait to set.
	 * @param TraitData The trait to set.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Set Trait",
					  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "subjective add new"))
	static void
	SetSubjectiveTrait(                            TScriptInterface<ISubjective> Subjective,
					   UPARAM(DisplayName="Type")  UScriptStruct*                TraitType,
					   UPARAM(DisplayName="Trait") const FGenericStruct&         TraitData)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_SetSubjectiveTrait(TScriptInterface<ISubjective> Subjective,
							   UScriptStruct* const          TraitType,
							   const void* const             TraitData)
	{
		ensure(OK(Subjective->template SetTrait<EParadigm::DefaultBlueprint>(TraitType, TraitData)));
	}

	DECLARE_FUNCTION(execSetSubjectiveTrait)
	{
		P_GET_TINTERFACE(ISubjective, Subjective);
		P_GET_OBJECT(UScriptStruct, TraitType);

		Stack.StepCompiledIn<FStructProperty>(nullptr);
		const void* const SrcTraitAddr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_SetSubjectiveTrait(Subjective, TraitType, SrcTraitAddr);
		P_NATIVE_END;
	}

	/**
	 * Obtain a trait from a subjective.
	 * 
	 * @param Subjective The subjective to obtain a trait from.
	 * @param TraitType The type of the trait to obtain.
	 * @param TraitData The obtained trait.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Obtain Trait",
			  		  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "subject create new"))
	static void
	ObtainSubjectiveTrait(TScriptInterface<ISubjective> Subjective,
						  UScriptStruct*                TraitType,
						  FGenericStruct&               TraitData)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_ObtainSubjectiveTrait(TScriptInterface<ISubjective> Subjective,
								  UScriptStruct* const          TraitType,
								  void* const                   TraitData)
	{
		ensure(OK(Subjective->template ObtainTrait<EParadigm::DefaultBlueprint>(TraitType, TraitData)));
	}

	DECLARE_FUNCTION(execObtainSubjectiveTrait)
	{
		P_GET_TINTERFACE(ISubjective, Subjective);
		P_GET_OBJECT(UScriptStruct, TraitType);

		Stack.StepCompiledIn<FStructProperty>(nullptr);
		void* const DstTraitAddr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_ObtainSubjectiveTrait(Subjective, TraitType, DstTraitAddr);
		P_NATIVE_END;
	}

	/**
	 * Get a trait from a subject.
	 * 
	 * @param SubjectHandle The subject to get from.
	 * @param TraitType The type of the trait to get.
	 * @param TraitData The trait got.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Trait",
			  		  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "subject find",
					  UnsafeDuringActorConstruction))
	static void
	GetSubjectTrait(const FSubjectHandle& SubjectHandle,
					UScriptStruct*        TraitType,
					FGenericStruct&       TraitData)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_GetSubjectTrait(const FSubjectHandle& SubjectHandle,
							UScriptStruct* const  TraitType,
							void* const           TraitData)
	{
		ensure(OK(SubjectHandle.template GetTrait<EParadigm::DefaultBlueprint>(TraitType, TraitData)));
	}

	DECLARE_FUNCTION(execGetSubjectTrait)
	{
		P_GET_STRUCT_REF(FSubjectHandle, SubjectHandle);
		P_GET_OBJECT(UScriptStruct, TraitType);

		Stack.StepCompiledIn<FStructProperty>(nullptr);
		void* SrcTraitAddr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_GetSubjectTrait(SubjectHandle, TraitType, SrcTraitAddr);
		P_NATIVE_END;
	}

	/**
	 * Set a trait for a subject.
	 * 
	 * @param SubjectHandle The subject to set a trait for.
	 * @param TraitType The type of the trait to set.
	 * @param TraitData The trait to set.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Set Trait",
			  		  CustomStructureParam = "TraitData",
					  KeyWords = "subject add new trait +",
					  UnsafeDuringActorConstruction))
	static void
	SetSubjectTrait(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle,
					UPARAM(DisplayName="Type")    UScriptStruct*        TraitType,
					UPARAM(DisplayName="Trait")   const FGenericStruct& TraitData)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_SetSubjectTrait(const FSubjectHandle& SubjectHandle,
							UScriptStruct* const  TraitType,
							const void* const     TraitData)
	{
		SubjectHandle.SetTrait(TraitType, TraitData);
	}

	DECLARE_FUNCTION(execSetSubjectTrait)
	{
		P_GET_STRUCT_REF(FSubjectHandle, SubjectHandle);
		P_GET_OBJECT(UScriptStruct, TraitType);

		Stack.StepCompiledIn<FStructProperty>(nullptr);
		const void* const SrcTraitAddr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_SetSubjectTrait(SubjectHandle, TraitType, SrcTraitAddr);
		P_NATIVE_END;
	}

	/**
	 * Obtain a trait from a subject.
	 * 
	 * @param SubjectHandle The subject to obtain the trait from.
	 * @param TraitType The type of trait to obtain.
	 * @param TraitData The obtained trait.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Obtain Trait",
			  		  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "subject create new",
					  UnsafeDuringActorConstruction))
	static void
	ObtainSubjectTrait(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle,
					   UPARAM(DisplayName="Type")    UScriptStruct*        TraitType,
					   UPARAM(DisplayName="Trait")   FGenericStruct&       TraitData)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_ObtainSubjectTrait(const FSubjectHandle& SubjectHandle,
							   UScriptStruct* const  TraitType,
							   void* const           TraitData)
	{
		ensure(OK(SubjectHandle.template ObtainTrait<EParadigm::DefaultBlueprint>(TraitType, TraitData)));
	}

	DECLARE_FUNCTION(execObtainSubjectTrait)
	{
		P_GET_STRUCT_REF(FSubjectHandle, SubjectHandle);
		P_GET_OBJECT(UScriptStruct, TraitType);

		Stack.StepCompiledIn<FStructProperty>(nullptr);
		void* SrcTraitAddr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_ObtainSubjectTrait(SubjectHandle, TraitType, SrcTraitAddr);
		P_NATIVE_END;
	}

	/**
	 * Remove a trait from the subject.
	 * 
	 * @param SubjectHandle The subject to remove a trait from.
	 * @param TraitType The type of trait to remove.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Remove Trait",
					  KeyWords = "subject delete -",
					  UnsafeDuringActorConstruction))
	static void
	RemoveSubjectTrait(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle,
					   UPARAM(DisplayName="Type")    UScriptStruct*        TraitType)
	{
		ensure(OK(SubjectHandle.template RemoveTrait<EParadigm::DefaultBlueprint>(TraitType)));
	}

	/**
	 * Copy all of the traits from one subject to another.
	 * 
	 * The existing traits of the destination subject are not removed
	 * but the new ones are added and/or override the existing matching.
	 * 
	 * @param SourceSubjectHandle The source subject to copy from. Must be a valid handle.
	 * @param DestinationSubjectHandle The destination subject to copy to. Must be a valid handle.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Copy Traits",
					  KeyWords = "clone duplicate subject",
					  UnsafeDuringActorConstruction))
	static void
	CopySubjectTraits(UPARAM(DisplayName = "From") const FSubjectHandle& SourceSubjectHandle,
					  UPARAM(DisplayName = "To")   const FSubjectHandle& DestinationSubjectHandle)
	{
		if (LIKELY(ensureMsgf(SourceSubjectHandle.IsValid() && DestinationSubjectHandle.IsValid(),
								TEXT("The handles for the subjects to copy from and to must be valid. "
									 "Were they already despawned?"))))
		{
			EnsureOK(SourceSubjectHandle.CopyTraitsTo<EParadigm::DefaultBlueprint>(DestinationSubjectHandle));
		}
	}

	/**
	 * Remove all traits from a subject.
	 * 
	 * @param SubjectHandle The subject to remove the traits from. Must be a valid handle.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Remove All Traits",
					  KeyWords = "delete destroy subject",
					  UnsafeDuringActorConstruction))
	static void
	RemoveAllSubjectTraits(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle)
	{
		if (UNLIKELY(ensureMsgf(SubjectHandle.IsValid(),
								TEXT("The handle for the subject to remove all traits from must be valid. "
									 "Was it already despaned?"))))
		{
			EnsureOK(SubjectHandle.RemoveAllTraits<EParadigm::DefaultBlueprint>());
		}
	}

	/**
	 * Remove a trait from the subjective.
	 * 
	 * @param Subjective The subjective to remove a trait from. Must be a valid one.
	 * @param TraitType The type of the trait to remove.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Remove Trait",
					  KeyWords = "subjective delete erase -",
					  UnsafeDuringActorConstruction))
	static void
	RemoveSubjectiveTrait(TScriptInterface<ISubjective> Subjective,
						  UScriptStruct*                TraitType)
	{
		ensure(OK(Subjective->template RemoveTrait<EParadigm::DefaultBlueprint>(TraitType)));
	}

	/**
	 * Check if the subject contains a trait of a certain type.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid handle.
	 * @param TraitType The trait type to check for.
	 * @return The result of examination.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Has Trait",
					  KeyWords = "subject check find does contains",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE bool
	HasSubjectTrait(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle,
					                              UScriptStruct*        TraitType)
	{
		return SubjectHandle.HasTrait(TraitType);
	}

	/**
	 * Check if the subjective contains a trait of a certain type.
	 * 
	 * @param Subjective The subjective to examine. Must be a valid one.
	 * @param TraitType The type of the trait to check for.
	 * @return The state of examination.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Has Trait",
					  KeyWords = "subjective check find does contains"))
	static FORCEINLINE bool
	HasSubjectiveTrait(TScriptInterface<ISubjective> Subjective,
					   UScriptStruct*                TraitType)
	{
		return Subjective->HasTrait(TraitType);
	}

#pragma endregion Traits

#pragma region Details

	/**
	 * Enable a detail of a certain type for a subjective.
	 *
	 * If the detail already exists and is active, nothing is performed, and
	 * that exact detail is returned. If the detail does not exist at all, a new
	 * detail is added, activated and returned. If the detail exists but is not
	 * currently active, it is activated and returned.
	 * 
	 * @param Subjective The subjective to modify. Must be a valid one.
	 * @param DetailClass The class of the detail to enable.
	 * @return The enabled detail instance.
	 */
	UFUNCTION(BlueprintCallable, Category = "Apparatus|Subject",
			  Meta = (DeterminesOutputType = "DetailClass",
					  DisplayName = "Enable Detail",
					  Keywords = "subject subjective"))
	static FORCEINLINE UDetail*
	EnableSubjectiveDetail(TScriptInterface<ISubjective> Subjective,
						   TSubclassOf<UDetail>          DetailClass)
	{
		return EnsuredPayload(Subjective->EnableDetail<EParadigm::DefaultBlueprint>(DetailClass));
	}

	/**
	 * Get the preferred user-set belt.
	 *
	 * @return The preferred belt of the subjective or null, if was not set.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Preferred Belt",
					  Keywords = "subject subjective"))
	static FORCEINLINE class UBelt*
	GetSubjectivePreferredBelt(TScriptInterface<ISubjective> Subjective)
	{
		return Subjective->GetPreferredBelt();
	}

	/**
	 * Check if the subjective contains a detail of a certain type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Has Detail",
					  KeyWords = "subjective contains does search"))
	static FORCEINLINE bool
	HasSubjectiveDetail(TScriptInterface<ISubjective> Subjective,
						TSubclassOf<UDetail>          DetailClass,
						const bool                    bIncludeDisabled = false)
	{
		return Subjective->HasDetail(DetailClass, bIncludeDisabled);
	}

	/**
	 * Find a detail by its type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Subjective",
			  Meta = (DeterminesOutputType = "DetailClass",
					  DisplayName = "Get Detail",
					  KeyWords = "by type subject subjective search find"))
	static FORCEINLINE UDetail*
	FindSubjectiveDetail(TScriptInterface<ISubjective> Subjective,
						 TSubclassOf<UDetail>          DetailClass,
						 const bool                    bIncludeDisabled = false)
	{
		return Subjective->GetDetail(DetailClass, bIncludeDisabled);
	}

	/**
	 * Get the current list of details in the subject.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Get Details",
					  KeyWords = "subjective find"))
	static void
	GetSubjectiveDetails(
		TScriptInterface<ISubjective> Subjective,
		TArray<UDetail*>&             OutDetails,
		const bool                    bIncludeDisabled = false);

	/**
	 * Find the details by their type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subjective",
			  Meta = (DeterminesOutputType = "DetailClass",
					  DisplayName = "Get Details of Class",
					  KeyWords = "subjective by type search find"))
	static TArray<UDetail*>
	FindSubjectiveDetails(
		TScriptInterface<ISubjective> Subjective,
		TSubclassOf<UDetail>          DetailClass,
		const bool                    bIncludeDisabled = false); 

	/**
	 * Add or reuse a detail of a certain type.
	 *
	 * @param Subjective The subjective to add a detail to.
	 * @param DetailClass The type of a detail to add to a subject.
	 * @param bReuseDisabled Should an inactive detail be reused, if possible.
	 * If the detail already exists and is active, a new detail is created,
	 * activated and returned. If the detail exists but is not currently
	 * active, it is activated and returned.
	 *
	 * @return A new or reused detail. Guaranteed to be active.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DeterminesOutputType = "DetailClass",
					  DefaultToSelf = "Subjective",
					  DisplayName = "Add Detail",
					  KeyWords = "subjective append new"))
	static FORCEINLINE class UDetail*
	AddSubjectiveDetail(TScriptInterface<ISubjective> Subjective,
						TSubclassOf<UDetail>          DetailClass,
						const bool                    bReuseDisabled = true)
	{
		if (ensureMsgf(Subjective, TEXT("The subject must not be a None.")))
		{
			return Subjective->AddDetail(DetailClass, bReuseDisabled);
		}
		return nullptr;
	}

	/**
	 * Disable a first detail matching the passed type.
	 * 
	 * You can't remove details, but only disable them instead.
	 *
	 * @param Subjective The subjective to disable a detail for.
	 * @param DetailClass The class of detail(s) to disable.
	 * @param bDisableMultiple Should multiple matching details be disabled at
	 * once, or only the first one?
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Disable Detail",
					  KeyWords = "subjective destroy delete remove"))
	static void
	DisableSubjectiveDetail(TScriptInterface<ISubjective> Subjective,
							TSubclassOf<UDetail>          DetailClass,
							const bool                    bDisableMultiple = false)
	{
		EnsureOK(Subjective->DisableDetail<EParadigm::DefaultBlueprint>(DetailClass, bDisableMultiple));
	}

#pragma endregion Details

#pragma region Networking

	/**
	 * Bring the subject to be available on the network.
	 * 
	 * Assigns a unique network identifier and makes
	 * cross-peer procedures possible.
	 * 
	 * @param SubjectHandle The subject to bring online. Must be a valid handle.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Bring Online (Server)",
					  KeyWords = "subject replicate network",
					  UnsafeDuringActorConstruction))
	static void
	BringSubjectOnlineOnServer(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle)
	{
		if (LIKELY(ensureMsgf(SubjectHandle.IsValid(),
							  TEXT("Handle must be valid in order to bring its subject to network."))))
		{
			EnsureOK(SubjectHandle.BringOnline<EParadigm::DefaultBlueprint>());
		}
	}

	/**
	 * Bring the subject to be available on the network while allocated its identifier within a range.
	 * 
	 * Assigns a unique network identifier and makes
	 * cross-peer procedures possible.
	 * 
	 * @param SubjectHandle The subject to bring online. Must be a valid handle.
	 * @param IdRange The range for network identifiers.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Bring Online Ranged (Server)",
					  KeyWords = "subject replicate network range ranged interval pool",
					  UnsafeDuringActorConstruction))
	static void
	BringSubjectOnlineOnServerWithIdRange(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle,
										                                const FInt32Range&    IdRange)
	{
		if (LIKELY(ensureMsgf(SubjectHandle.IsValid(),
							  TEXT("Handle must be valid in order to bring its subject to network."))))
		{
			if (ensure(IdRange.HasLowerBound() && IdRange.HasUpperBound()))
			{
				TRange<SubjectNetworkIdType>::BoundsType Lower;
				TRange<SubjectNetworkIdType>::BoundsType Upper;
				if (IdRange.GetLowerBound().IsInclusive())
				{
					Lower = TRange<SubjectNetworkIdType>::BoundsType::Inclusive(IdRange.GetLowerBound().GetValue());
				}
				else
				{
					Lower = TRange<SubjectNetworkIdType>::BoundsType::Exclusive(IdRange.GetLowerBound().GetValue());
				}
				if (IdRange.GetUpperBound().IsInclusive())
				{
					Upper = TRange<SubjectNetworkIdType>::BoundsType::Inclusive(IdRange.GetUpperBound().GetValue());
				}
				else
				{
					Upper = TRange<SubjectNetworkIdType>::BoundsType::Exclusive(IdRange.GetUpperBound().GetValue());
				}
				const auto RealIdRange = TRange<SubjectNetworkIdType>(Lower, Upper);
				EnsureOK(SubjectHandle.BringOnline<EParadigm::DefaultBlueprint>(RealIdRange));
			}
		}
	}

	/**
	 * Get the unique network identifier of the subject.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid handle.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Network Id",
					  KeyWords = "subject network offline",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE int64
	GetSubjectNetworkId(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle)
	{
		if (LIKELY(ensureMsgf(SubjectHandle,
							  TEXT("The handle must be valid in order to get its network identifier."))))
		{
			return SubjectHandle.GetNetworkId();
		}
		return FSubjectNetworkState::InvalidId;
	}

	/**
	 * Check if the subject is online.
	 * 
	 * Examines if a subject was brought online
	 * and is currently networked.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid handle.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Is Online?",
					  KeyWords = "subject network offline",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE bool
	IsSubjectOnline(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle)
	{
		if (LIKELY(ensureMsgf(SubjectHandle,
							  TEXT("The handle must be valid in order to test for being online."))))
		{
			return SubjectHandle.IsOnline();
		}
		return false;
	}

	/**
	 * Check if the subject is server-side.
	 * 
	 * A subject must be both online and be part
	 * of a server world in order to be considered
	 * server-side.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid handle.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Is Server-Side?",
					  KeyWords = "subject network",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE bool
	IsSubjectServerSide(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle)
	{
		if (LIKELY(ensureMsgf(SubjectHandle,
							  TEXT("The handle must be valid in order to test for being a server-side."))))
		{
			return SubjectHandle.IsServerSide();
		}
		return false;
	}

	/**
	 * Check if the subject is client-side.
	 * 
	 * A subject must be both online and be part
	 * of a client world in order to be considered
	 * client-side.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid handle.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Is Client-Side?",
					  KeyWords = "subject network",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE bool
	IsSubjectClientSide(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle)
	{
		if (LIKELY(ensureMsgf(SubjectHandle,
							  TEXT("The handle must be valid in order to test for being a client-side."))))
		{
			return SubjectHandle.IsClientSide();
		}
		return false;
	}

	/**
	 * Get the server-side connection that is
	 * allowed to push traits to this subject.
	 * 
	 * Can only be queried on a server-side subject version.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid handle.
	 * @return The current connection permit.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Connection Permit",
					  KeyWords = "subject network filter whitelist",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE UObject*
	GetSubjectConnectionPermit(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle)
	{
		if (LIKELY(ensureMsgf(SubjectHandle.IsServerSide(),
							  TEXT("The handle must be a valid server-side one in order to get the connection permit."))))
		{
			return SubjectHandle.GetConnectionPermit();
		}
		return nullptr;
	}

	/**
	 * Set the server-side connection from which
	 * it is allowed to push to this subject.
	 * 
	 * Can only be set for a server-side subject version.
	 * 
	 * @param SubjectHandle The subject to set for. Must be a valid handle.
	 * @param InConnectionPermit The object to derive the connection from. May
	 * be an actor.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Set Connection Permit",
					  KeyWords = "subject network whitelist allow permit owner",
					  UnsafeDuringActorConstruction))
	static void
	SetSubjectConnectionPermit(UPARAM(DisplayName="Subject")    const FSubjectHandle& SubjectHandle,
							   UPARAM(DisplayName="Connection") UObject*              InConnectionPermit)
	{
		UNetConnection* NetConnection = nullptr;
		if (LIKELY(InConnectionPermit != nullptr))
		{
			NetConnection = Cast<UNetConnection>(InConnectionPermit);
			if (NetConnection == nullptr)
			{
				const auto Actor = Cast<AActor>(InConnectionPermit);
				if (LIKELY(Actor != nullptr))
				{
					NetConnection = Actor->GetNetConnection();
				}
			}
		}
		if (LIKELY(ensureMsgf(SubjectHandle.IsServerSide(),
							  TEXT("The handle must be a valid server-side one in order to set the connection permit."))))
		{
			EnsureOK(SubjectHandle.SetConnectionPermit<EParadigm::DefaultBlueprint>(NetConnection));
		}
	}

	/**
	 * Get the server-side list of
	 * traits allowed to be received from clients.
	 * 
	 * Can only be queried on a server-side subject version.
	 * 
	 * @param SubjectHandle The subject to examine. Must be a valid handle.
	 * @return The current traitmark permit whitelist.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Traitmark Permit",
					  KeyWords = "subject network filter whitelist",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE FTraitmark
	GetSubjectTraitmarkPermit(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle)
	{
		if (LIKELY(ensureMsgf(SubjectHandle.IsServerSide(),
							  TEXT("The handle must be a valid server-side one in order to get the traitmark permit."))))
		{
			return SubjectHandle.GetTraitmarkPermit();
		}
		return FTraitmark::Zero;
	}

	/**
	 * Set the list of traits allowed to be
	 * received from clients on the server.
	 * 
	 * Can only be set for a server-side subject.
	 * 
	 * @note The traitmark permit is also used to auto-detect the
	 * peer role during the trait pushing on a listen-server, i.e.
	 * if a trait is included within the permit the role is considered
	 * to be a client. This, of course, can be overriden by an explicit
	 * peer role specification during the push.
	 * 
	 * @param SubjectHandle The subject to set for. Must be a valid handle.
	 * @param InTraitmarkPass The traitmark to be used as a whitelist.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Set Traitmark Permit",
					  KeyWords = "subject network",
					  UnsafeDuringActorConstruction))
	static void
	SetSubjectTraitmarkPermit(UPARAM(DisplayName="Subject")   const FSubjectHandle& SubjectHandle,
							  UPARAM(DisplayName="Traitmark") const FTraitmark&     InTraitmarkPass)
	{
		if (LIKELY(ensureMsgf(SubjectHandle.IsServerSide(),
							  TEXT("The handle must be a valid server-side one in order to set the traitmark permit."))))
		{
			EnsureOK(SubjectHandle.SetTraitmarkPermit<EParadigm::DefaultBlueprint>(InTraitmarkPass));
		}
	}

	/**
	 * Get the server-side connection that is allowed
	 * to push to this subject.
	 * 
	 * Can only be queried on a server-side subjective version.
	 * 
	 * @param Subjective The subjective to examine. Must be a valid server-side subjective.
	 * @return The current connection permit.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Connection Permit (Subjective)",
					  KeyWords = "subject network filter whitelist",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE UObject*
	GetSubjectiveConnectionPermit(TScriptInterface<ISubjective> Subjective)
	{
		if (LIKELY(ensureMsgf(Subjective && Subjective->IsServerSide(),
							  TEXT("The subjective must be a valid server-side one in order to get the connection permit."))))
		{
			return Subjective->GetConnectionPermit();
		}
		return nullptr;
	}

	/**
	 * Set the server-side connection
	 * that is allowed to push to this subjective.
	 * 
	 * Can only be set for a server-side subjective version.
	 * 
	 * @param Subjective The subjective to set for. Must be a valid server-side subjective.
	 * @param InConnectionPermit The object to derive the connection from. May
	 * be an actor.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Set Connection Permit (Subjective)",
					  KeyWords = "subject network owner",
					  UnsafeDuringActorConstruction))
	static void
	SetSubjectiveConnectionPermit(                                 TScriptInterface<ISubjective> Subjective,
								  UPARAM(DisplayName="Connection") UObject*                      InConnectionPermit)
	{
		UNetConnection* NetConnection = nullptr;
		if (LIKELY(InConnectionPermit != nullptr))
		{
			NetConnection = Cast<UNetConnection>(InConnectionPermit);
			if (NetConnection == nullptr)
			{
				const auto Actor = Cast<AActor>(InConnectionPermit);
				if (LIKELY(Actor != nullptr))
				{
					NetConnection = Actor->GetNetConnection();
				}
			}
		}
		if (LIKELY(ensureMsgf(Subjective && Subjective->IsServerSide(),
							  TEXT("The subjective must be a valid server-side one in order to set the connection permit."))))
		{
			EnsureOK(Subjective->SetConnectionPermit<EParadigm::DefaultBlueprint>(NetConnection));
		}
	}

	/**
	 * Get the server-side list of
	 * traits allowed to be received from clients.
	 * 
	 * Can only be queried on a server-side subjective version.
	 * 
	 * @param Subjective The subjective to set for. Must be a valid server-side subjective.
	 * @return The current traitmark permit whitelist.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Traitmark Permit (Subjective)",
					  KeyWords = "subject network filter whitelist"))
	static FORCEINLINE FTraitmark
	GetSubjectiveTraitmarkPermit(TScriptInterface<ISubjective> Subjective)
	{
		if (LIKELY(ensureMsgf(Subjective && Subjective->IsServerSide(),
							  TEXT("The subjective must be a valid server-side one in order to set the traitmark permit."))))
		{
			return Subjective->GetTraitmarkPermit();
		}
		return FTraitmark::Zero;
	}

	/**
	 * Set the list of traits allowed to be
	 * received from clients on the server.
	 * 
	 * Can only be set for a server-side subject.
	 * 
	 * @note The traitmark permit is also used to auto-detect the
	 * peer role during the trait pushing on a listen-server, i.e.
	 * if a trait is included within the permit the role is considered
	 * to be a client. This, of course, can be overriden by an explicit
	 * peer role specification during the push.
	 * 
	 * @param Subjective The subjective to set for. Must be a valid server-side subjective.
	 * @param InTraitmarkPass The new traitmark to be used as a whitelist.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Set Traitmark Permit (Subjective)",
					  KeyWords = "subject network whitelist"))
	static void
	SetSubjectiveTraitmarkPermit(                                TScriptInterface<ISubjective> Subjective,
								 UPARAM(DisplayName="Traitmark") const FTraitmark&             InTraitmarkPass)
	{
		if (LIKELY(ensureMsgf(Subjective->IsServerSide(),
							  TEXT("The subjective must be a valid server-side one in order to set the traitmark permit."))))
		{
			EnsureOK(Subjective->SetTraitmarkPermit<EParadigm::DefaultBlueprint>(InTraitmarkPass));
		}
	}

	/**
	 * Push an existing trait structure to the clients version of the online subject.
	 * 
	 * @param SubjectHandle A handle for the subject to push to.
	 * @param TraitType The type of the trait to push.
	 * @param PeerRole The explicit peer role specification.
	 * This may needed to be specified when on a listen server mode and
	 * you want to push traits to the clients that are also in the traitmark permit.
	 * @param bReliable Should reliable channel be used for a transaction.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Push Trait",
					  KeyWords = "online network replicate",
					  UnsafeDuringActorConstruction,
					  AdvancedDisplay = "PeerRole"))
	static void
	PushSubjectTrait(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle,
					 UPARAM(DisplayName="Type")    UScriptStruct*        TraitType,
												   const EPeerRole       PeerRole = EPeerRole::None,
												   const bool            bReliable = true)
	{
		if (ensureMsgf(SubjectHandle.IsValid() && (TraitType != nullptr),
					   TEXT("The subject to push the trait to and the type of the trait itself must all be valid.")))
		{
			EnsureOK(SubjectHandle.PushTrait<EParadigm::DefaultBlueprint>(TraitType, PeerRole, bReliable));
		}
	}

	/**
	 * Push an additional trait structure to the clients version of the online subject.
	 * 
	 * @param SubjectHandle A handle for the subject to push to.
	 * @param TraitType The type of the trait to push.
	 * @param TraitData The data of the trait to push.
	 * @param bSetForLocal Also set the trait for the local version.
	 * @param PeerRole The explicit peer role specification.
	 * This may needed to be specified when on a listen server mode and
	 * you want to push traits to the clients that are also in the traitmark permit.
	 * @param bReliable Should reliable channel be used for a transaction.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Push Extra Trait",
					  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "online network replicate",
					  UnsafeDuringActorConstruction,
					  AdvancedDisplay = "PeerRole"))
	static void
	PushExtraSubjectTrait(UPARAM(DisplayName="Subject") const FSubjectHandle& SubjectHandle,
						  UPARAM(DisplayName="Type")    UScriptStruct*        TraitType,
						  UPARAM(DisplayName="Trait")   const FGenericStruct& TraitData,
						                                const bool            bSetForLocal = false,
														const EPeerRole       PeerRole = EPeerRole::None,
														const bool            bReliable = true)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_PushExtraSubjectTrait(const FSubjectHandle& SubjectHandle,
								  UScriptStruct* const  TraitType,
								  const void* const     TraitData,
								  const bool            bSetForLocal = false,
								  const EPeerRole       PeerRole = EPeerRole::None,
								  const bool            bReliable = true)
	{
		if (ensureAlwaysMsgf(!bSetForLocal, TEXT("The 'Set For Local' argument is now deprecated and will be removed in future. Set it to 'false', please.")))
		{
			EnsureOK(SubjectHandle.PushTrait<EParadigm::DefaultBlueprint>(TraitType, TraitData, PeerRole, bReliable));
		}
	}

	DECLARE_FUNCTION(execPushExtraSubjectTrait)
	{
		P_GET_STRUCT_REF(FSubjectHandle, SubjectHandle);
		P_GET_OBJECT(UScriptStruct, TraitType);
		Stack.StepCompiledIn<FStructProperty>(nullptr);
		const void* const SrcTraitAddr = Stack.MostRecentPropertyAddress;
		P_GET_UBOOL(bSetForLocal);
		P_GET_ENUM(EPeerRole, PeerRole);
		P_GET_UBOOL(bReliable);

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_PushExtraSubjectTrait(SubjectHandle, TraitType, SrcTraitAddr, bSetForLocal, PeerRole, bReliable);
		P_NATIVE_END;
	}

	/**
	 * Push an existing trait to an online subjective.
	 * 
	 * The client must own the subjective.
	 * 
	 * @param Subjective The subjective to push the trait to.
	 * @param TraitType The type of the trait to push.
	 * @param PeerRole The explicit peer role specification.
	 * This may needed to be specified when on a listen server mode and
	 * you want to push traits to the clients that are also in the traitmark permit.
	 * @param bReliable Should reliable channel be used for the transaction.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Push Trait (Subjective)",
					  KeyWords = "subjective network replicate",
					  UnsafeDuringActorConstruction,
					  AdvancedDisplay = "PeerRole"))
	static void
	PushSubjectiveTrait(                            TScriptInterface<ISubjective> Subjective,
						UPARAM(DisplayName="Type")  UScriptStruct*                TraitType,
													const EPeerRole               PeerRole = EPeerRole::None,
													const bool                    bReliable = true)
	{
		if (ensureMsgf((Subjective != nullptr) && (TraitType != nullptr),
					   TEXT("The subjective to push a trait to must be valid along with the trait type.")))
		{
			EnsureOK(Subjective->PushTrait<EParadigm::DefaultBlueprint>(TraitType, PeerRole, bReliable));
		}
	}

	/**
	 * Push an additional trait to an online subjective.
	 * 
	 * The client must own the subjective.
	 * 
	 * @param Subjective The subjective to push the trait to.
	 * @param TraitType The type of the trait to push.
	 * @param TraitData The trait to push.
	 * @param bSetForLocal Also set the trait for the local version.
	 * @param PeerRole The explicit peer role specification.
	 * This may needed to be specified when on a listen server mode and
	 * you want to push traits to the clients that are also in the traitmark permit.
	 * @param bReliable Should reliable channel be used for the transaction.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Subjective",
			  Meta = (DisplayName = "Push Extra Trait (Subjective)",
			  		  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "subjective network replicate",
					  UnsafeDuringActorConstruction,
					  AdvancedDisplay = "PeerRole"))
	static void
	PushExtraSubjectiveTrait(                            TScriptInterface<ISubjective> Subjective,
							 UPARAM(DisplayName="Type")  UScriptStruct*                TraitType,
							 UPARAM(DisplayName="Trait") const FGenericStruct&         TraitData,
							 							 const bool                    bSetForLocal = false,
														 const EPeerRole               PeerRole = EPeerRole::None,
														 const bool                    bReliable = true)
	{
		checkNoEntry();
	}

	FORCEINLINE static void
	Generic_PushExtraSubjectiveTrait(TScriptInterface<ISubjective> Subjective,
									 UScriptStruct* const          TraitType,
									 const void* const             TraitData,
									 const bool                    bSetForLocal = false,
									 const EPeerRole               PeerRole = EPeerRole::None,
									 const bool                    bReliable = true)
	{
		if (ensureAlwaysMsgf(!bSetForLocal, TEXT("The 'Set For Local' argument is now deprecated and will be removed in future. Set it to 'false', please.")))
		{
			if (ensureMsgf((Subjective != nullptr) && (TraitType != nullptr) && (TraitData != nullptr),
						TEXT("The subjective to push a trait to must be valid along with the trait.")))
			{
				EnsureOK(Subjective->PushTrait<EParadigm::DefaultBlueprint>(TraitType, TraitData, PeerRole, bReliable));
			}
		}
	}

	DECLARE_FUNCTION(execPushExtraSubjectiveTrait)
	{
		P_GET_TINTERFACE(ISubjective, Subjective);
		P_GET_OBJECT(UScriptStruct, TraitType);
		Stack.StepCompiledIn<FStructProperty>(nullptr);
		const void* const SrcTraitAddr = Stack.MostRecentPropertyAddress;
		P_GET_UBOOL(bSetForLocal);
		P_GET_ENUM(EPeerRole, PeerRole);
		P_GET_UBOOL(bReliable);

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_PushExtraSubjectiveTrait(Subjective, TraitType, SrcTraitAddr, bSetForLocal, PeerRole, bReliable);
		P_NATIVE_END;
	}

	/**
	 * Push an existing trait to a remote version of the subject
	 * through the bearer.
	 * 
	 * This function may be used to send a trait to a single specific client only.
	 * The subject will be brought online automatically if needed.
	 * 
	 * @param NetworkBearer The network bearer to push through.
	 * @param SubjectHandle The subject to push the trait for.
	 * @param TraitType The type of the trait to push.
	 * Must not be a null.
	 * @param bReliable Should reliable channel be used.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Networking",
			  Meta = (DisplayName = "Push Trait (Bearer)",
					  KeyWords = "subject send single client",
					  UnsafeDuringActorConstruction))
	static void
	PushSubjectTraitThroughBearer(
		UPARAM(DisplayName="Bearer")  UNetworkBearerComponent* NetworkBearer,
		UPARAM(DisplayName="Subject") const FSubjectHandle&    SubjectHandle,
		UPARAM(DisplayName="Type")    UScriptStruct*           TraitType,
		                              const bool               bReliable = true)
	{
		if (ensureMsgf(NetworkBearer != nullptr,
					   TEXT("A valid network bearer must be used in order to push the '%s' trait through it. "
							"Was it somehow destroyed already?"), *TraitType->GetName())
			&&
			ensureMsgf(SubjectHandle.IsValid(),
					   TEXT("A valid subject handle must be provided in order to push the '%s' trait for it. "
							"Was it despawned already?"), *TraitType->GetName())
			&&
			ensureMsgf(TraitType != nullptr,
					   TEXT("The valid trait type to push through the bearer must be provided.")))
		{
			EnsureOK(NetworkBearer->PushSubjectTrait<EParadigm::DefaultBlueprint>(SubjectHandle, TraitType, bReliable));
		}
	}

	/**
	 * Push an additional trait to a remote version of the subject
	 * through the bearer.
	 * 
	 * This function may be used to send a trait to a single specific client only.
	 * The subject will be brought online automatically if needed.
	 * 
	 * @param NetworkBearer The network bearer to push through.
	 * @param SubjectHandle The subject to push the trait for.
	 * @param TraitType The type of the trait to push.
	 * Must not be a null.
	 * @param TraitData The data of the trait to push.
	 * Must not e a null.
	 * @param bReliable Should reliable channel be used.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk,
			  BlueprintInternalUseOnly,
			  Category = "Apparatus|Networking",
			  Meta = (DisplayName = "Push Extra Trait (Bearer)",
					  CustomStructureParam = "TraitData",
					  AutoCreateRefTerm = "TraitData",
					  KeyWords = "subject send single client",
					  UnsafeDuringActorConstruction))
	static void
	PushExtraSubjectTraitThroughBearer(
		UPARAM(DisplayName="Bearer")  UNetworkBearerComponent* NetworkBearer,
		UPARAM(DisplayName="Subject") const FSubjectHandle&    SubjectHandle,
		UPARAM(DisplayName="Type")    UScriptStruct*           TraitType,
		UPARAM(DisplayName="Trait")   const FGenericStruct&    TraitData,
									  const bool               bReliable = true);

	FORCEINLINE static void
	Generic_PushExtraSubjectTraitThroughBearer(
								  UNetworkBearerComponent* NetworkBearer,
								  const FSubjectHandle&    SubjectHandle,
								  UScriptStruct* const     TraitType,
								  const void* const        TraitData,
								  const bool               bReliable)
	{
		if (ensureMsgf(NetworkBearer,
					   TEXT("A valid network bearer must be used in order to push the extra '%s' trait through it. "
							"Was it somehow destroyed already?"), *TraitType->GetName())
			&&
			ensureMsgf(SubjectHandle,
					   TEXT("A valid subject handle must be provided in order to push the extra '%s' trait for it. "
							"Was it despawned already?"), *TraitType->GetName()))
		{
			EnsureOK(NetworkBearer->PushSubjectTrait<EParadigm::DefaultBlueprint>(SubjectHandle, TraitType, TraitData, bReliable));
		}
	}

	DECLARE_FUNCTION(execPushExtraSubjectTraitThroughBearer)
	{
		P_GET_OBJECT(UNetworkBearerComponent, NetworkBearer);
		P_GET_STRUCT_REF(FSubjectHandle, SubjectHandle);
		P_GET_OBJECT(UScriptStruct, TraitType);
		Stack.StepCompiledIn<FStructProperty>(nullptr);
		const void* const SrcTraitAddr = Stack.MostRecentPropertyAddress;
		P_GET_UBOOL(bReliable);

		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_PushExtraSubjectTraitThroughBearer(
			NetworkBearer,
			SubjectHandle,
			TraitType,
			SrcTraitAddr,
			bReliable);

		P_NATIVE_END;
	}

#pragma endregion Networking

	/**
	 * Disable a detail for the subject.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Disable Detail",
					  KeyWords = "subject delete remove erase -",
					  UnsafeDuringActorConstruction))
	static void
	DisableSubjectDetail(UPARAM(Ref) FSubjectHandle& SubjectHandle,
						 TSubclassOf<UDetail>        DetailClass,
						 const bool                  bDisableMultiple = false)
	{
		auto Subjective = SubjectHandle.GetSubjective();
		if (LIKELY(ensure(Subjective)))
		{
			EnsureOK(Subjective->DisableDetail<EParadigm::DefaultBlueprint>(DetailClass, bDisableMultiple));
		}
	}

	/**
	 * Get the subjective of the subject (if any).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Get Subjective",
					  KeyWords = "subject",
					  UnsafeDuringActorConstruction))
	static FORCEINLINE TScriptInterface<ISubjective>
	GetSubjectSubjective(const FSubjectHandle& SubjectHandle)
	{
		return SmartCast<TScriptInterface<ISubjective>>(SubjectHandle);
	}

	/**
	 * Check if the subject contains a detail.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure,
			  Category = "Apparatus|Subject",
			  Meta = (DisplayName = "Has Detail",
					  KeyWords = "subject check find does contains",
					  UnsafeDuringActorConstruction))
	static bool
	HasSubjectDetail(const FSubjectHandle& SubjectHandle,
					 TSubclassOf<UDetail>  DetailClass)
	{
		return SubjectHandle.HasDetail(DetailClass);
	}

	/**
	 * Despawn a subject from the machine.
	 * 
	 * If the subject is already despawned,
	 * nothing is performed.
	 * 
	 * @param SubjectHandle The subject to despawn.
	 */
	UFUNCTION(BlueprintCallable,
			  Category = "Apparatus|Machine",
			  Meta = (DisplayName = "Despawn",
					  KeyWords = "subject remove destroy entity -",
					  UnsafeDuringActorConstruction))
	static void
	DespawnMachineSubject(const FSubjectHandle& SubjectHandle)
	{
		EnsureOK(SubjectHandle.Despawn<EParadigm::DefaultBlueprint>());
	}

}; // class UApparatusFunctionLibrary


FORCEINLINE bool
UApparatusFunctionLibrary::AdvanceChain(UObject*    WorldContextObject,
										const int32 ChainId,
										AMechanism* Mechanism)
{
	if (LIKELY(Mechanism == nullptr))
	{
		Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
	}
	if (ensure(Mechanism))
	{
		return Mechanism->ObtainChain(ChainId)->Advance();
	}
	return false;
}

FORCEINLINE void
UApparatusFunctionLibrary::RemoveFromSubjectHandles4(UPARAM(Ref) FSubjectHandles4& SubjectHandles, FSubjectHandle SubjectHandle)
{
	SubjectHandles.Remove(SubjectHandle);
}

FORCEINLINE void
UApparatusFunctionLibrary::RemoveFromSubjectHandles8(UPARAM(Ref) FSubjectHandles8& SubjectHandles, FSubjectHandle SubjectHandle)
{
	SubjectHandles.Remove(SubjectHandle);
}

FORCEINLINE void
UApparatusFunctionLibrary::RemoveFromSubjectHandles16(UPARAM(Ref) FSubjectHandles16& SubjectHandles, FSubjectHandle SubjectHandle)
{
	SubjectHandles.Remove(SubjectHandle);
}

FORCEINLINE void
UApparatusFunctionLibrary::GetSubjectiveDetails(
	TScriptInterface<ISubjective> Subjective,
	TArray<UDetail*>&             OutDetails,
	const bool                    bIncludeDisabled)
{
	Subjective->GetDetails(bIncludeDisabled, OutDetails);
}

FORCEINLINE TArray<UDetail*> UApparatusFunctionLibrary::FindSubjectiveDetails(
	TScriptInterface<ISubjective> Subjective, TSubclassOf<UDetail> DetailClass,
	const bool bIncludeDisabled)
{
	TArray<UDetail*> DetailsOut;
	Subjective->GetDetails(DetailClass, DetailsOut, bIncludeDisabled);
	return MoveTemp(DetailsOut);
}

FORCEINLINE FTraitmark
UApparatusFunctionLibrary::MakeTraitmark(const TArray<UScriptStruct*>& Traits)
{
	return FTraitmark(Traits);
}

FORCEINLINE FDetailmark
UApparatusFunctionLibrary::MakeDetailmark(const TArray<TSubclassOf<UDetail>>& Details)
{
	return FDetailmark(Details);
}

FORCEINLINE FFingerprint
UApparatusFunctionLibrary::MakeFingerprint(
	const TArray<UScriptStruct*>&       Traits,
	const TArray<TSubclassOf<UDetail>>& Details,
	const int32                         Flagmark)
{
	return FFingerprint(Traits, Details, Flagmark);
}

FORCEINLINE FFilter
UApparatusFunctionLibrary::MakeFilter(
	const TArray<UScriptStruct*>&       Traits,
	const TArray<TSubclassOf<UDetail>>& Details,
	const TArray<UScriptStruct*>&       ExcludedTraits,
	const TArray<TSubclassOf<UDetail>>& ExcludedDetails,
	const int32                         Flagmark,
	const int32                         ExcludingFlagmark)
{
	return FFilter(Traits, Details,
				   ExcludedTraits, ExcludedDetails,
				   (EFlagmark)Flagmark, (EFlagmark)ExcludingFlagmark);
}

FORCEINLINE FFingerprint& UApparatusFunctionLibrary::AddDetailToFingerprint(
	FFingerprint &Fingerprint, TSubclassOf<UDetail> DetailClass)
{
	return Fingerprint += DetailClass;
}

FORCEINLINE FFingerprint&
UApparatusFunctionLibrary::RemoveDetailFromFingerprint(
	FFingerprint& Fingerprint, TSubclassOf<UDetail> DetailClass)
{
	return Fingerprint -= DetailClass;
}

FORCEINLINE void
UApparatusFunctionLibrary::MechanismEnchain(UObject*       WorldContextObject,
											const FFilter& Filter,
											int32&         OutChainId,
											AMechanism*    Mechanism)
{
	if (LIKELY(Mechanism == nullptr))
	{
		Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
	}
	if (ensure(Mechanism))
	{
		Mechanism->Enchain(Filter);
	}
}

FORCEINLINE void
UApparatusFunctionLibrary::MechanismEnchainHalted(UObject*             WorldContextObject,
												  UPARAM(Ref) FFilter& Filter,
												  int32&               OutChainId,
												  AMechanism*          Mechanism)
{
	const auto FlagmarkSave = Filter.GetFlagmark();
	const auto ExcludingFlagmarkSave = Filter.GetFlagmark();
	Filter.Exclude(FM_Booted);
	if (LIKELY(Mechanism == nullptr))
	{
		Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
	}
	const auto Chain = Mechanism->Enchain(Filter);
	Filter.SetFlagmark(FlagmarkSave);
	Filter.SetExcludingFlagmark(ExcludingFlagmarkSave);
	OutChainId = Chain->GetId();
}

FORCEINLINE void
UApparatusFunctionLibrary::MechanismEnchainBooted(UObject*             WorldContextObject,
												  UPARAM(Ref) FFilter& Filter,
												  int32&               OutChainId,
												  AMechanism*          Mechanism)
{
	const auto FlagmarkSave = Filter.GetFlagmark();
	const auto ExcludingFlagmarkSave = Filter.GetFlagmark();
	Filter.Include(FM_Booted);
	if (LIKELY(Mechanism == nullptr))
	{
		Mechanism = UMachine::ObtainMechanism(WorldContextObject->GetWorld());
	}
	if (ensure(Mechanism))
	{
		const auto Chain = Mechanism->Enchain(Filter);
		Filter.SetFlagmark(FlagmarkSave);
		Filter.SetExcludingFlagmark(ExcludingFlagmarkSave);
		OutChainId = Chain->GetId();
	}
	else
	{
		OutChainId = FChain::InvalidId;
	}
}
