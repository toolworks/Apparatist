/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ScriptStructArray.h
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
#include "Kismet/KismetSystemLibrary.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define SCRIPT_STRUCT_ARRAY_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"

#ifdef SCRIPT_STRUCT_ARRAY_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "ScriptStructArray.generated.h"


/**
 * Dynamically-typed container of structs.
 * 
 * Unlike TArray<T> which is statically (compile-time) typed
 * you can use this container with a runtime-defined struct
 * type to store an array of those.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FScriptStructArray
{
	GENERATED_BODY()

  private:

	/**
	 * The structs data.
	 * 
	 * This is typed dynamically to a type of struct
	 * defined by the FScriptStructArray::ElementType.
	 */
	uint8* Data = nullptr;

	/**
	 * The type of the elements in the array.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Array,
			  Meta = (AllowPrivateAccess = "true"))
	UScriptStruct* ElementType = nullptr;

	/**
	 * The current number of elements in the array.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Array,
			  Meta = (AllowPrivateAccess = "true"))
	int32 Count = 0;

	/**
	 * The total capacity of the array.
	 * 
	 * This is the maximum number of elements
	 * that can be stored within the array
	 * without any additional allocations.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Array,
			  Meta = (AllowPrivateAccess = "true"))
	int32 Capacity = 0;

	FORCEINLINE int32
	CalcSlackGrow(const int32 NewCount) const
	{
		check(ElementType);
		return DefaultCalculateSlackGrow(NewCount, Capacity, ElementType->GetStructureSize(), /*bAllowQuantize=*/true);
	}

	FORCEINLINE int32
	CalcSlackGrow() const
	{
		return CalcSlackGrow(Count);
	}

	FORCEINLINE int32
	CalcSlackShrink(const int32 NewCount) const
	{
		check(ElementType);
		return DefaultCalculateSlackShrink(NewCount, Capacity, ElementType->GetStructureSize(), /*bAllowQuantize=*/true);
	}

	FORCEINLINE int32
	CalcSlackShrink() const
	{
		return CalcSlackShrink(Count);
	}

	FORCEINLINE int32
	CalcSlackReserve(const int32 NewCount) const
	{
		check(ElementType);
		return DefaultCalculateSlackReserve(NewCount, ElementType->GetStructureSize(), /*bAllowQuantize=*/true);
	}


	FORCEINLINE int32
	CalcSlackReserve() const
	{
		return CalcSlackReserve(Count);
	}

	/**
	 * Get an element data at a specified index.
	 * 
	 * May also return uninitialized data.
	 * 
	 * @param Index The index of an element to get the data of.
	 * @return The data of the element struct.
	 */
	FORCEINLINE void*
	MemoryAt(const int32 Index)
	{
		check(Index >= 0);
		check(Index < Capacity);
		check(ElementType);
		// Use SIZE_T cast to be able to address more memory:
		return (void*)(Data + (SIZE_T)ElementType->GetStructureSize() * (SIZE_T)Index);
	}

	/**
	 * Get an element data at a specified index.
	 * Constant version.
	 * 
	 * May also return uninitialized data.
	 * 
	 * @param Index The index of an element to get the data of.
	 * @return The read-only data of the element struct.
	 */
	FORCEINLINE const void*
	MemoryAt(const int32 Index) const
	{
		check(Index >= 0);
		check(Index < Capacity);
		check(ElementType);
		// Use SIZE_T cast to be able to address more memory:
		return (const void*)(Data + (SIZE_T)ElementType->GetStructureSize() * (SIZE_T)Index);
	}

  public:

  	/**
	 * Get the type of the elements in the array.
	 */
	FORCEINLINE UScriptStruct*
	GetElementType() const
	{
		return ElementType;
	}

	/**
	 * Get the size of a single element.
	 * 
	 * @return The size of the element in bytes.
	 */
	FORCEINLINE int32
	GetElementSize() const
	{
		if (!ElementType) return 0;
		return ElementType->GetStructureSize();
	}

	/**
	 * Get the current number of elements currently in the array.
	 */
	FORCEINLINE int32
	Num() const
	{
		return Count;
	}

	/**
	 * Get the maximum number of elements that can
	 * be stored in the array without any additional
	 * allocations.
	 */
	FORCEINLINE int32
	Max() const
	{
		return Capacity;
	}

	/**
	 * Get an element data at specified index.
	 * 
	 * @param Index The index of an element to get the data of.
	 * @return The initialized data of the element struct.
	 */
	FORCEINLINE void*
	At(const int32 Index)
	{
		check(Index < Count);
		return MemoryAt(Index);
	}

	/**
	 * Get an element data at specified index.
	 * Constant version.
	 * 
	 * @param Index The index of an element to get the data of.
	 * @return The initialized data of the element struct.
	 */
	FORCEINLINE const void*
	At(const int32 Index) const
	{
		check(Index < Count);
		return MemoryAt(Index);
	}

	/**
	 * Get an element reference at a specific index.
	 * 
	 * @tparam T The type of the element to cast to. Used for extra safety.
	 * @param Index The index of an element to get the data of.
	 * @return The initialized data of the element struct.
	 */
	template < typename T >
	FORCEINLINE T&
	RefAt(const int32 Index)
	{
		check(Index < Count);
		check(GetElementType() == T::StaticStruct());
		return *(static_cast<T*>(MemoryAt(Index)));
	}

	/**
	 * Get an element reference at a specific index.
	 * Constant version.
	 * 
	 * @tparam T The type of the element to cast to. Used for extra safety.
	 * @param Index The index of an element to get the data of.
	 * @return The initialized data of the element struct.
	 */
	template < typename T >
	FORCEINLINE const T&
	RefAt(const int32 Index) const
	{
		check(Index < Count);
		check(GetElementType() == T::StaticStruct());
		return *(static_cast<const T*>(MemoryAt(Index)));
	}

	/**
	 * Get an element reference at a specific index.
	 * 
	 * @tparam T The type of the element to cast to. Used for extra safety.
	 * @param Index The index of an element to get the data of.
	 * @return The initialized data of the element struct.
	 */
	template < typename T >
	FORCEINLINE T*
	PtrAt(const int32 Index)
	{
		check(Index < Count);
		check(GetElementType() == T::StaticStruct());
		return static_cast<T*>(MemoryAt(Index));
	}

	/**
	 * Get an element reference at a specific index.
	 * Constant version.
	 * 
	 * @tparam T The type of the element to cast to. Used for extra safety.
	 * @param Index The index of an element to get the data of.
	 * @return The initialized data of the element struct.
	 */
	template < typename T >
	FORCEINLINE const T*
	PtrAt(const int32 Index) const
	{
		check(Index < Count);
		check(GetElementType() == T::StaticStruct());
		return static_cast<const T*>(MemoryAt(Index));
	}

	/**
	 * Get an element data at specified index.
	 * 
	 * @param Index The index of an element to get the data of.
	 *
	 * @return The initialized data of the element struct.
	 */
	FORCEINLINE void*
	operator[](const int32 Index)
	{
		return At(Index);
	}

	/**
	 * Get a read-only element data at specified index.
	 * 
	 * @param Index The index of an element to get the data of.
	 *
	 * @return The read-only initialized data of the element struct.
	 */
	FORCEINLINE const void*
	operator[](const int32 Index) const
	{
		return At(Index);
	}

	/**
	 * Reserve the space for a certain amount of elements.
	 * 
	 * @param InCapacity The new capacity to guarantee.
	 * @return Was anything actually changed?
	 */
	FORCEINLINE bool
	Reserve(const int32 InCapacity)
	{
		if (Capacity >= InCapacity)
		{
			return false;
		}
		check(ElementType);
		Capacity = InCapacity;
		Data = (uint8*)Apparatus_ReallocArray(Data, Capacity, ElementType->GetStructureSize());
		return true;
	}

	/**
	 * Append a new struct element with default-initialized value.
	 * 
	 * @return A pointer to the data of the added element. 
	 */
	FORCEINLINE void*
	AppendDefaulted()
	{
		check(ElementType);
		check(Count < TNumericLimits<int32>::Max());
		if (Count == Capacity)
		{
			// We need to increase the capacity...
			const int32 NewCapacity = CalcSlackGrow(Count + 1);
			Reserve(NewCapacity);
		}
		void* r = MemoryAt(Count);
		ElementType->InitializeStruct(r);
		Count += 1;
		return r;
	}

	/**
	 * Append several new struct elements with default-initialized values.
	 * 
	 * @param InCount The total number of elements to append.
	 * @return A pointer to the data of the first added element. 
	 */
	FORCEINLINE void*
	AppendDefaulted(const int32 InCount)
	{
		check(InCount >= 0);
		if (UNLIKELY(InCount == 0)) return nullptr;
		check(ElementType);
		check(Count <= TNumericLimits<int32>::Max() - InCount); // Comparison with overflow protection.
		const int32 NewCount = Count + InCount;
		if (NewCount > Capacity)
		{
			// We need to increase the capacity...
			const int32 NewCapacity = CalcSlackGrow(NewCount);
			Reserve(NewCapacity);
		}
		void* r = MemoryAt(Count);
		ElementType->InitializeStruct(r, InCount);
		Count += InCount;
		return r;
	}

	/**
	 * Clear the array without changing the allocated space, but
	 * only if the passed capacity is not larger than the present one.
	 * 
	 * @param InCapacity The new capacity to guarantee.
	 */
	FORCEINLINE void
	Reset(const int32 InCapacity = 0)
	{
		if (ElementType && Count)
		{
			ElementType->DestroyStruct(Data, Count);
			Count = 0;
		}
		Reserve(InCapacity);
	}

	/**
	 * Remove the elements from the end of the container.
	 * 
	 * @param InCount The number of elements to remove.
	 * @param bAllowShrinking Is capacity shrinking allowed?
	 */
	FORCEINLINE void
	Pop(const int32 InCount = 1,
	    const bool  bAllowShrinking = true)
	{
		if (!InCount) return;
		checkf(InCount <= Count,
			   TEXT("The number of elements to pop must be lower or equal to existing: %d > %d"),
			   InCount, Count);
		check(ElementType);

		const int32 NewCount = Count - InCount;
		ElementType->DestroyStruct(At(NewCount), InCount);
		Count = NewCount;
		if (bAllowShrinking)
		{
			const int32 NewCapacity = CalcSlackShrink();
			if (Capacity != NewCapacity)
			{
				Capacity = NewCapacity;
				Data = (uint8*)Apparatus_ReallocArray(Data, NewCapacity, ElementType->GetStructureSize());
			}
		}
	}

	/**
	 * Set the container with an element type and a capacity.
	 * 
	 * @param InElementType The new element type.
	 * @param InCapacity The new capacity to guarantee.
	 */
	FORCEINLINE void
	Set(UScriptStruct* const InElementType,
		const int32          InCapacity = 0)
	{
		check(InElementType || (InCapacity == 0));
		check(InCapacity >= 0);
		if (ElementType != InElementType)
		{
			// The types of the elements differ.
			// Destroy the former elements...
			if (ElementType && Count)
			{
				ElementType->DestroyStruct(Data, Count);
			}
			ElementType = InElementType;
			Data = (uint8*)Apparatus_ReallocArray(Data, InCapacity, ElementType->GetStructureSize());
			Count = 0;
			Capacity = InCapacity;
			check(Count <= Capacity);
			return;
		}
		// The array element types are the same.
		// Just reset the capacity:
		Reset(InCapacity);
	}

	FORCEINLINE void
	Set(const FScriptStructArray& Array)
	{
		if (UNLIKELY(this == std::addressof(Array))) return;
		if (ElementType != Array.ElementType)
		{
			// The types of the elements differ.
			// Destroy the former elements, then
			// reallocate their data and initialize-copy it...
			if (ElementType && Count)
			{
				ElementType->DestroyStruct(Data, Count);
			}
			ElementType = Array.ElementType;
			Data = (uint8*)Apparatus_ReallocArray(Data, Array.Capacity, ElementType->GetStructureSize());
			ElementType->InitializeStruct(Data, Array.Count);
			ElementType->CopyScriptStruct(Data, Array.Data, Array.Count);
			Count = Array.Count;
			Capacity = Array.Capacity;
			check(Count <= Capacity);
			return;
		}
		// The array element types are the same...
		if (!ElementType) return;
		if (Count == Array.Count)
		{
			// The count is the same. Just copy then:
			ElementType->CopyScriptStruct(Data, Array.Data, Count);
		}
		else
		{
			if (Count > Array.Count)
			{
				// More than enough elements are initialized. Destroy
				// the excessive ones and copy the remaining...
				ElementType->DestroyStruct(At(Array.Count), Count - Array.Count);
				ElementType->CopyScriptStruct(Data, Array.Data, Array.Count);
				Count = Array.Count;
			}
			else
			{
				// Not enough elements in the array. Make sure the capacity
				// is enough and copy all of the elements...
				Reserve(Array.Count);
				ElementType->InitializeStruct(At(Count), Array.Count - Count);
				ElementType->CopyScriptStruct(Data, Array.Data, Array.Count);
				Count = Array.Count;
			}
		}
	}

	FORCEINLINE
	FScriptStructArray()
	{
		check(!ElementType);
		check(Count == 0);
		check(Capacity == 0);
		check(!Data);
	}

	FORCEINLINE
	FScriptStructArray(UScriptStruct* InElementType, int32 InCapacity = 0)
	  : ElementType(InElementType)
	  , Capacity(InCapacity)
	{
		check(ElementType);
		check(Count >= 0);
		check(Capacity >= 0);
		Data = (uint8*)Apparatus_MallocArray(Capacity, ElementType->GetStructureSize());
	}

	FORCEINLINE
	FScriptStructArray(const FScriptStructArray& Array)
	  : ElementType(Array.ElementType)
	  , Count(Array.Count)
	  , Capacity(Array.Capacity)
	{
		check(ElementType);
		check(Count >= 0);
		check(Capacity >= 0);
		Data = (uint8*)Apparatus_MallocArray(Capacity, ElementType->GetStructureSize());
		ElementType->InitializeStruct(Data, Count);
		ElementType->CopyScriptStruct(Data, Array.Data, Count);
	}

	/**
	 * Construct with moving an array.
	 */
	FORCEINLINE
	FScriptStructArray(FScriptStructArray&& Array)
	  : Data(Array.Data)
	  , ElementType(Array.ElementType)
	  , Count(Array.Count)
	  , Capacity(Array.Capacity)
	{
		Array.ElementType = nullptr;
		Array.Count = 0;
		Array.Capacity = 0;
		Array.Data = nullptr;
	}

	FORCEINLINE
	~FScriptStructArray()
	{
		if (ElementType && Data)
		{
			ElementType->DestroyStruct(Data, Count);
			FMemory::Free(Data);
		}
	}

	/**
	 * Copy an element at a specific index to a struct data.
	 * 
	 * @param ElementIndex The index of an element to copy.
	 * @param OutData A receiving struct data pointer. Must have enough allocated space.
	 * @param bDataInitialized Is the receiving data actually initialized?
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	ElementAt(const int32 ElementIndex,
			  void* const OutData,
			  const bool  bDataInitialized = true) const
	{
		const void* const Src = At(ElementIndex);
		if (UNLIKELY(Src == OutData))
		{
			return EApparatusStatus::Noop;
		}
		if (UNLIKELY(!bDataInitialized))
		{
			ElementType->InitializeStruct(OutData);
		}
		ElementType->CopyScriptStruct(OutData, Src);
		return EApparatusStatus::Success;
	}

	/**
	 * Swap an element at a specific index to a struct data.
	 * 
	 * @param ElementIndex The index of the element to swap.
	 * @param OtherData The other struct data pointer. Must have enough allocated space.
	 * @param bDataInitialized Is the receiving data actually initialized?
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	SwapElementWith(const int32 ElementIndex,
					void* const OtherData,
					const bool  bDataInitialized = true)
	{
		void* const ElementData = At(ElementIndex);
		if (UNLIKELY(ElementData == OtherData)) return EApparatusStatus::Noop;
		if (UNLIKELY(!bDataInitialized))
		{
			ElementType->InitializeStruct(OtherData);
		}
		FMemory::Memswap(ElementData, OtherData, GetElementSize());
		return EApparatusStatus::Success;
	}

	/**
	 * Copies an element at a specific index to a struct.
	 * Templated version.
	 * 
	 * @tparam T The type of the element to copy. Must match the type of the array elements.
	 * @param ElementIndex The index of an element to copy.
	 * @param OutStruct A receiving struct.
	 * @param bStructInitialized Is the receiving struct actually initialized?
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE EApparatusStatus
	ElementAt(const int32 ElementIndex,
			  T&          OutStruct,
			  const bool  bStructInitialized = true) const
	{
		check(ElementType == T::StaticStruct());
		return ElementAt(ElementIndex, (void* const)&OutStruct, bStructInitialized);
	}

	/**
	 * The type of gathering to
	 * use during the collection.
	 */
	enum EGatheringType
	{
		/**
		 * Clear the output buffer before collecting.
		 */
		Clear,

		/**
		 * Append to the existing elements of the outputting buffer.
		 */
		Collect
	};

	/**
	 * Copy the elements of the array to a normal array.
	 * 
	 * @tparam T The type of elements of the array.
	 * Must match the current element type.
	 * @tparam AllocatorT The allocator used within an array.
	 * @param OutArray The array to output the structs to.
	 * @param GatheringType The type of gathering to apply.
	 * @return The status of the operation. 
	 */
	template < typename T, typename AllocatorT >
	EApparatusStatus
	CopyTo(TArray<T, AllocatorT>& OutArray, const EGatheringType GatheringType = Clear) const
	{
		auto Status = EApparatusStatus::Noop;
		if (GatheringType == Clear)
		{
			if (UNLIKELY(OutArray.Num() > 0))
			{
				Status = EApparatusStatus::Success;
				OutArray.Reset(Count);
			}
		}
		else
		{
			OutArray.Reserve(OutArray.Num() + Count);
		}
		if (UNLIKELY(Count == 0))
		{
			return Status;
		}
		check(ElementType == T::StaticStruct());
		// TODO: This can perhaps be optimized for plain data.
		for (int32 i = 0; i < Count; ++i)
		{
			StatusAccumulate(Status, ElementAt(i, OutArray.AddDefaulted_GetRef()));
		}
		return Status;
	}

	/**
	 * Gets a copy of an element at a specific index
	 * returning it to the caller.
	 * 
	 * @tparam T The type of the element to copy. Must match the type of the array elements.
	 * @param ElementIndex The index of an element to copy.
	 * @return The copy of the element.
	 */
	template < typename T >
	FORCEINLINE T
	ElementAt(const int32 ElementIndex) const
	{
		check(ElementType == T::StaticStruct());
		T Copy;
		ElementAt(ElementIndex, Copy);
		return MoveTempIfPossible(Copy);
	}

	/**
	 * Sets an element at a specific index to an initialized struct data.
	 * 
	 * @param[in] ElementIndex The index of an element to set.
	 * @param[in] InData An initialized struct data to set with.
	 */
	FORCEINLINE EApparatusStatus
	SetElementAt(const int32       ElementIndex,
				 const void* const InData)
	{
		check(InData != nullptr);
		const auto Dst = At(ElementIndex);
		if (UNLIKELY(Dst == InData)) return EApparatusStatus::Noop;
		check(ElementType != nullptr);
		ElementType->CopyScriptStruct(Dst, InData);
		return EApparatusStatus::Success;
	}

	/**
	 * Sets an element at a specific index to an initialized struct data.
	 * 
	 * @tparam T The type of the element to set.
	 * Must match the element type of the array.
	 * @param[in] ElementIndex The index of an element to set.
	 * @param[in] InElement An initialized struct data to set with.
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE EApparatusStatus
	SetElementAt(const int32 ElementIndex,
				 const T&    InElement)
	{
		const auto Dst = At(ElementIndex);
		if (UNLIKELY(Dst == std::addressof(InElement))) return EApparatusStatus::Noop;
		check(GetElementType() == T::StaticStruct());
		ElementType->CopyScriptStruct(Dst, static_cast<const void*>(&InElement));
		return EApparatusStatus::Success;
	}

	/**
	 * Sets an element at a specific index to an initialized struct data.
	 * 
	 * @tparam T The type of the element to set.
	 * Must match the element type of the array.
	 * @param[in] ElementIndex The index of an element to set.
	 * @param[in] InElement An initialized struct data to set with.
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE EApparatusStatus
	SetElementAt(const int32    ElementIndex,
				 const T* const InElement)
	{
		const auto Dst = At(ElementIndex);
		if (UNLIKELY(Dst == InElement)) return EApparatusStatus::Noop;
		check(GetElementType() == T::StaticStruct());
		ElementType->CopyScriptStruct(Dst, static_cast<const void*>(InElement));
		return EApparatusStatus::Success;
	}

	/**
	 * Swap two elements in the array.
	 */
	FORCEINLINE void
	Swap(const int32 FirstIndexToSwap,
		 const int32 SecondIndexToSwap)
	{
		if (UNLIKELY(FirstIndexToSwap == SecondIndexToSwap))
		{
			return;
		}
		check(ElementType);
		void* const First  = At(FirstIndexToSwap);
		void* const Second = At(SecondIndexToSwap);
		check(First != Second);
		FMemory::Memswap(First, Second, GetElementSize());
	}

	/**
	 * Clear an element at a specific index by finalizing and
	 * initializing it again.
	 * 
	 * @param ElementIndex The index of an element to clear.
	 */
	FORCEINLINE void
	ClearElementAt(const int32 ElementIndex)
	{
		check(ElementType);
		void* const Dst = At(ElementIndex);
		ElementType->ClearScriptStruct(Dst);
	}

	/**
	 * Clone an element at a specified index.
	 * 
	 * @param ElementIndex The index of an element to clone.
	 * @return The index of the newly added element.
	 */
	FORCEINLINE int32
	CloneElementAt(const int32 ElementIndex)
	{
		check(ElementType);
		check(Count < TNumericLimits<int32>::Max());

		const int32 NewCount = Count + 1;
		if (UNLIKELY(NewCount > Capacity))
		{
			// We need to increase the capacity...
			const int32 NewCapacity = CalcSlackGrow(NewCount);
			Reserve(NewCapacity);
		}
		const void* const Src = At(ElementIndex);
		void* const New = MemoryAt(Count);
		ElementType->InitializeStruct(New);
		ElementType->CopyScriptStruct(New, Src);
		return Count++;
	}

	/**
	 * Move an array.
	 */
	inline FScriptStructArray&
	operator=(FScriptStructArray&& Array)
	{
		if (ElementType && Data)
		{
			ElementType->DestroyStruct(Data, Count);
		}
		if (Data)
		{
			FMemory::Free(Data);
		}

		ElementType = Array.ElementType;
		Count       = Array.Count;
		Capacity    = Array.Capacity;
		Data        = Array.Data;

		Array.ElementType = nullptr;
		Array.Count       = 0;
		Array.Capacity    = 0;
		Array.Data        = nullptr;

		return *this;
	}

	/**
	 * Set the array equal to an another one.
	 * 
	 * @param Array An array to copy.
	 * @return Returns itself.
	 */
	FORCEINLINE FScriptStructArray&
	operator=(const FScriptStructArray& Array)
	{
		Set(Array);
		return *this;
	}

}; //-struct FScriptStructArray
