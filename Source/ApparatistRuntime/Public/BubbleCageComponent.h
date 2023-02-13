/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageComponent.h
 * Created: 2023-02-02 15:38:49
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include <atomic>

#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"

#include "MechanicalActorComponent.h"

#include "BubbleCageCell.h"
#include "BubbleSphere.h"
#include "Located.h"

#include "BubbleCageComponent.generated.h"


#define BUBBLE_DEBUG 0

/**
 * A simple and performant collision detection and decoupling for spheres.
 */
UCLASS(Category = "BubbleCage")
class APPARATISTRUNTIME_API UBubbleCageComponent
  : public UMechanicalActorComponent
{
	GENERATED_BODY()

  private:

	/**
	 * The largest radius among the bubbles.
	 * 
	 * Used for neighbour detection.
	 */
	float LargestRadius = 0;

  public:

	void
	BeginDestroy() override
	{		
		Super::BeginDestroy();
	}

	/* Initialize the internal state. */
	void InitializeInternalState();

	/**
	 * The number of threads to use for the concurrent processing.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Performance")
	int32 ThreadsCount = 4;

  private:

	/**
	 * The size (width, height, depth) of a single cell of the cage in world units.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Volume", Meta = (AllowPrivateAccess = "true"))
	float CellSize = 1;

	/**
	 * The total size of the cage in number of cells.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Volume", Meta = (AllowPrivateAccess = "true"))
	FIntVector Size;

	/**
	 * The bounds of the cage in world units.
	 * 
	 * These are precalculated at start.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Volume", Meta = (AllowPrivateAccess = "true"))
	mutable FBox Bounds;

	bool bInitialized = false;

	/**
	 * All of the cells of the cage.
	 */
	TArray<FBubbleCageCell> Cells;

	/**
	 * The indices of the cells that are currently occupied by the subjects.
	 */
	TQueue<int32, EQueueMode::Mpsc> OccupiedCells;

	/**
	 * Initialize the internal cells array.
	 */
	void
	DoInitializeCells()
	{
		Cells.Reset(); // Make sure there are no cells.
		if (ensureAlwaysMsgf((int64)Size.X * (int64)Size.Y * (int64)Size.Z < (int64)TNumericLimits<int32>::Max(),
							TEXT("The '%s' bubble cage has too many cells in it. Please, decrease its corresponding size in cells.")))
		{
			Cells.AddDefaulted(Size.X * Size.Y * Size.Z);
		}
	}

#pragma region UActorComponent

	void
	InitializeComponent() override;

#pragma endregion UActorComponent


  public:

#pragma region Debugging
	/// @name Debugging
	/// @{

#if WITH_EDITORONLY_DATA

	/**
	 * Should individual cage cells be drawn in the debugging mode.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debugging")
	bool bDebugDrawCageCells = false;

#endif // WITH_EDITORONLY_DATA

	/// @}
#pragma endregion Debugging

	/* Sets default values for the actor's properties. */
	UBubbleCageComponent();

	/**
	 * Get the size of a single cell in global units.
	 */
	const auto
	GetCellSize() const
	{
		return CellSize;
	}

	/**
	 * Get the size of the cage in cells among each axis.
	 */
	const FIntVector&
	GetSize() const
	{
		return Size;
	}

	/**
	 * Get the global bounds of the cage in world units.
	 */
	const FBox&
	GetBounds() const
	{
		if (bInitialized)
		{
			return Bounds;
		}
		const auto Extents = CellSize * FVector(Size) * 0.5f;
		const auto Actor = GetOwner();
		const auto Location = (Actor != nullptr) ? Actor->GetActorLocation() : FVector::ZeroVector;
		Bounds.Min = Location - Extents;
		Bounds.Max = Location + Extents;
		return Bounds;
	}

	void
	BeginPlay() override
	{
		DoInitializeCells();
	}

	/* Convert world 3d-location to position in the cage. No checks. */
	FORCEINLINE FIntVector
	WorldToCage(FVector Point) const
	{
		Point -= Bounds.Min;
		Point /= CellSize;
		return FIntVector(FMath::FloorToInt(Point.X),
						  FMath::FloorToInt(Point.Y),
						  FMath::FloorToInt(Point.Z));
	}

	/* Get the index of the cage cell. */
	FORCEINLINE int32
	GetIndexAt(int32 X, int32 Y, int32 Z) const
	{
		X = FMath::Clamp(X, 0, Size.X - 1);
		Y = FMath::Clamp(Y, 0, Size.Y - 1);
		Z = FMath::Clamp(Z, 0, Size.Z - 1);
		return X + Size.X * (Y + Size.Y * Z);
	}

	/**
	 * Get overlapping spheres for the specified location.
	 */
	int32
	GetOverlapping(const FVector&          Location,
				   TArray<FSubjectHandle>& OutOverlappers) const
	{
		OutOverlappers.Reset();
		const auto Range = FVector(LargestRadius);
		const auto CagePosMin = WorldToCage(Location - Range);
		const auto CagePosMax = WorldToCage(Location + Range);
		for (auto i = CagePosMin.X; i <= CagePosMax.X; ++i)
		{
			for (auto j = CagePosMin.Y; j <= CagePosMax.Y; ++j)
			{
				for (auto k = CagePosMin.Z; k <= CagePosMax.Z; ++k)
				{
					const auto NeighbourCellPos = FIntVector(i, j, k);
					if (LIKELY(IsInside(NeighbourCellPos)))
					{
						const auto& NeighbourCell = At(NeighbourCellPos);
						for (int32 t = 0; t < NeighbourCell.Subjects.Num(); ++t)
						{
							const auto OtherBubble = NeighbourCell.Subjects[t];
							if (LIKELY(OtherBubble))
							{
								const auto OtherBubbleSphere =
									OtherBubble.GetTrait<FBubbleSphere>();
								const auto OtherLocation =
									OtherBubble.GetTrait<FLocated>().GetLocation();
								const auto Delta = Location - OtherLocation;
								const float Distance = Delta.Size();
								if (OtherBubbleSphere.Radius > Distance)
								{
									OutOverlappers.Add(OtherBubble);
								}
							}
						}
					}
				}
			}
		}
		return OutOverlappers.Num();
	}

	/**
	 * Get overlapping spheres for the specified location.
	 */
	UFUNCTION(BlueprintCallable)
	int32
	GetOverlapping(const FVector&          Location,
				   const float             Radius,
				   TArray<FSubjectHandle>& OutOverlappers) const
	{
		if (UNLIKELY(Radius == 0))
		{
			return GetOverlapping(Location, OutOverlappers);
		}

		OutOverlappers.Reset();
		const auto Range = FVector(Radius + LargestRadius);
		const auto CagePosMin = WorldToCage(Location - Range);
		const auto CagePosMax = WorldToCage(Location + Range);
		for (auto i = CagePosMin.X; i <= CagePosMax.X; ++i)
		{
			for (auto j = CagePosMin.Y; j <= CagePosMax.Y; ++j)
			{
				for (auto k = CagePosMin.Z; k <= CagePosMax.Z; ++k)
				{
					const auto NeighbourCellPos = FIntVector(i, j, k);
					if (LIKELY(IsInside(NeighbourCellPos)))
					{
						const auto& NeighbourCell = At(NeighbourCellPos);
						for (int32 t = 0; t < NeighbourCell.Subjects.Num(); ++t)
						{
							const auto OtherBubble = NeighbourCell.Subjects[t];
							if (LIKELY(OtherBubble))
							{
								const auto OtherBubbleSphere =
									OtherBubble.GetTrait<FBubbleSphere>();
								const auto OtherLocation =
									OtherBubble.GetTrait<FLocated>().GetLocation();
								const auto Delta = Location - OtherLocation;
								const float Distance = Delta.Size();
								const float DistanceDelta = (Radius + OtherBubbleSphere.Radius) - Distance;
								if (DistanceDelta > 0)
								{
									OutOverlappers.Add(OtherBubble);
								}
							}
						}
					}
				}
			}
		}
		return OutOverlappers.Num();
	}

	/**
	 * Get overlapping bubbles within the cage.
	 * 
	 * @param Location The location to track.
	 * @param Radius The radius of overlapping.
	 * @return The overlapped bubbles.
	 */
	TArray<FSubjectHandle>
	GetOverlapping(const FVector& Location,
				   const float    Radius = 0.0f)
	{
		TArray<FSubjectHandle> Overlappers;
		GetOverlapping(Location, Radius, Overlappers);
		return MoveTemp(Overlappers);
	}

	/* Get position in the cage by index of the cell*/
	FORCEINLINE FIntVector
	GetCellPointByIndex(int32 Index) const
	{
		int32 z = Index / (Size.X * Size.Y);
		int32 layerPadding = Index - (z * Size.X * Size.Y);

		return FIntVector(layerPadding / Size.X, layerPadding % Size.X , z);
	}

	/* Get the index of the cage cell. */
	int32 GetIndexAt(const FIntVector& CellPoint) const
	{
		return GetIndexAt(CellPoint.X, CellPoint.Y, CellPoint.Z);
	}

	/* Get the index of the cell by the world position. */
	int32 GetIndexAt(FVector Point) const
	{
		return GetIndexAt(WorldToCage(Point));
	}

	/* Get subjects in a specific cage cell. Const version. */
	FORCEINLINE const FBubbleCageCell&
	At(const int32 X, const int32 Y, const int32 Z) const
	{
		return Cells[GetIndexAt(X, Y, Z)];
	}

	/* Get subjects in a specific cage cell. */
	FORCEINLINE FBubbleCageCell&
	At(const int32 X, const int32 Y, const int32 Z)
	{
		return Cells[GetIndexAt(X, Y, Z)];
	}

	/* Check if the cage point is inside the cage. */
	FORCEINLINE bool
	IsInside(const FIntVector& CellPoint) const
	{
		return (CellPoint.X >= 0) && (CellPoint.X < Size.X) &&
			   (CellPoint.Y >= 0) && (CellPoint.Y < Size.Y) &&
			   (CellPoint.Z >= 0) && (CellPoint.Z < Size.Z);
	}

	/* Check if the world point is inside the cage. */
	FORCEINLINE bool
	IsInside(const FVector& WorldPoint) const
	{
		return IsInside(WorldToCage(WorldPoint));
	}

	/* Get subjects in a specific cage cell by position in the cage. */
	FORCEINLINE FBubbleCageCell&
	At(const FIntVector& CellPoint)
	{
		return At(CellPoint.X, CellPoint.Y, CellPoint.Z);
	}

	/* Get subjects in a specific cage cell by position in the cage. */
	FORCEINLINE const FBubbleCageCell&
	At(const FIntVector& CellPoint) const
	{
		return At(CellPoint.X, CellPoint.Y, CellPoint.Z);
	}

	/* Get subjects in a specific cage cell by world 3d-location. */
	FORCEINLINE FBubbleCageCell&
	At(FVector Point)
	{
		return At(WorldToCage(Point));
	}

	/* Get subjects in a specific cage cell by world 3d-location. */
	FORCEINLINE const FBubbleCageCell&
	At(FVector Point) const
	{
		return At(WorldToCage(Point));
	}

	/* Get a box shape representing a cell by position in the cage. */
	FORCEINLINE FBox
	BoxAt(const FIntVector& CellPoint)
	{
		const FVector Min = Bounds.Min + FVector(CellPoint) * CellSize;
		FBox Box(Min, Min + FVector(CellSize));
		return MoveTemp(Box);
	}

	/* Get a box shape representing a cell by world 3d-location. */
	FORCEINLINE FBox
	BoxAt(const FVector Point)
	{
		return BoxAt(WorldToCage(Point));
	}

	/**
	 * Update the cage filling it with existing bubbles.
	 */
	UFUNCTION(BlueprintCallable)
	void
	Update()
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_BubbleCage_Update);

		const auto Mechanism = GetMechanism();

		// Clear-up the cage...
		int32 CellIndex;
		while (OccupiedCells.Dequeue(CellIndex))
		{
			Cells[CellIndex].Subjects.Empty();
		}

		LargestRadius = 0;

		// Occupy the cage cells...
		static const auto Filter = FFilter::Make<FLocated, FBubbleSphere>();
		// An atomic flag used for syncing the access to the
		// subjects collection:
		std::atomic_flag CollectingLock = ATOMIC_FLAG_INIT;
		Mechanism->EnchainSolid(Filter)->OperateConcurrently([=, &CollectingLock]
		(FSolidSubjectHandle  Subject,
		 const FLocated&      Located,
		 const FBubbleSphere& BubbleSphere)
		{
			if (LargestRadius < BubbleSphere.Radius)
			{
				LargestRadius = BubbleSphere.Radius;
			}
			const auto Location = Located.Location;
			if (UNLIKELY(!IsInside(Location)))
			{
				Subject.DespawnDeferred();
				return;
			}
			if (LargestRadius < BubbleSphere.Radius)
			{
				LargestRadius = BubbleSphere.Radius;
			}
			const auto CellIndex = GetIndexAt(Location);
			while (CollectingLock.test_and_set(std::memory_order_acquire));
 			Cells[CellIndex].Subjects.Add((FSubjectHandle)Subject);
			CollectingLock.clear(std::memory_order_release);
#if BUBBLE_DEBUG
			const FBox CellBox = BoxAt(Location);
			DrawDebugBox(GetWorld(),
						 CellBox.GetCenter(),
						 CellBox.GetExtent(), FColor::Yellow, false,
						 0, 0, 2);
			DrawDebugSphere(GetWorld(),
							Location,
							BubbleSphere.Radius, 12, FColor::Cyan, false, 0, 0, 1);
			DrawDebugString(
				GetWorld(), CellBox.GetCenter(),
				FString::Format(TEXT("Cell #{0}"),
								FStringFormatOrderedArguments(CellIndex)));
#endif
			OccupiedCells.Enqueue(CellIndex);
		}, ThreadsCount);
	}

	/**
	 * Decouple the bubbles within the cage.
	 * 
	 * @note You may actually have to call Update() before and after this method,
	 * to re-fill the cage accordingly.
	 */
	UFUNCTION(BlueprintCallable)
	void
	Decouple()
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_BubbleCage_Decouple);

		const auto Mechanism = GetMechanism();

		static const auto Filter = FFilter::Make<FLocated, FBubbleSphere>();
		// Detect collisions...
		Mechanism->EnchainSolid(Filter)->OperateConcurrently([=]
		(FSolidSubjectHandle Bubble,
		 FLocated&           Located,
		 FBubbleSphere&      BubbleSphere)
		{
			if (UNLIKELY(BubbleSphere.DecoupleProportion <= 0.0f)) return;
			const auto Location = Located.Location;
			const auto Range = FVector(BubbleSphere.Radius + LargestRadius);
			const auto CagePosMin = WorldToCage(Location - Range);
			const auto CagePosMax = WorldToCage(Location + Range);
			for (auto i = CagePosMin.X; i <= CagePosMax.X; ++i)
			{
				for (auto j = CagePosMin.Y; j <= CagePosMax.Y; ++j)
				{
					for (auto k = CagePosMin.Z; k <= CagePosMax.Z; ++k)
					{
						const auto NeighbourCellPos = FIntVector(i, j, k);
						if (LIKELY(IsInside(NeighbourCellPos)))
						{
							const auto& NeighbourCell = At(NeighbourCellPos);
							for (int32 t = 0; t < NeighbourCell.Subjects.Num(); ++t)
							{
								const auto OtherBubble = NeighbourCell.Subjects[t];
								if (LIKELY(OtherBubble && (OtherBubble != Bubble)))
								{
									const auto OtherBubbleSphere =
										OtherBubble.GetTrait<FBubbleSphere>();
									const auto OtherLocation =
										OtherBubble.GetTrait<FLocated>().GetLocation();
									const auto Delta = Location - OtherLocation;
									const float Distance = Delta.Size();
									const float DistanceDelta =
										(BubbleSphere.Radius + OtherBubbleSphere.Radius) - Distance;
									const float Strength = BubbleSphere.DecoupleProportion /
														  (BubbleSphere.DecoupleProportion + OtherBubbleSphere.DecoupleProportion);
									if (DistanceDelta > 0)
									{
										// We're hitting a neighbor.
										if (UNLIKELY(Distance <= 0.01f))
										{
											// The distance is too small to get the direction.
											// Use the ids to get the direction.
											if (Bubble.GetId() > OtherBubble.GetId())
											{
												BubbleSphere.AccumulatedDecouple +=
													FVector::LeftVector * DistanceDelta *
													Strength;
											}
											else
											{
												BubbleSphere.AccumulatedDecouple +=
													FVector::RightVector * DistanceDelta *
													Strength;
											}
										}
										else
										{
											BubbleSphere.AccumulatedDecouple +=
												(Delta / Distance) * DistanceDelta *
												Strength;
										}
										BubbleSphere.AccumulatedDecoupleCount += 1;
									}
								}
							}
						}
					}
				}
			}
		}, ThreadsCount);

		// Decouple...
		Mechanism->EnchainSolid(Filter)->OperateConcurrently([=]
		(FLocated& Located, FBubbleSphere& BubbleSphere)
		{
			if (BubbleSphere.AccumulatedDecoupleCount > 0)
			{
				Located.Location += BubbleSphere.AccumulatedDecouple /
									BubbleSphere.AccumulatedDecoupleCount;
				BubbleSphere.AccumulatedDecouple = FVector::ZeroVector;
				BubbleSphere.AccumulatedDecoupleCount = 0;
			}
		}, ThreadsCount);
	}

	/**
	 * Re-register and decouple the bubbles.
	 * 
	 * The bubbles get updated within the cage after
	 * the decoupling phase.
	 */
	UFUNCTION(BlueprintCallable)
	void
	Evaluate()
	{
		Update();
		Decouple();
		Update();
	}
};
