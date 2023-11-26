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
 * The state of currently being coupled.
 * 
 * This is only used if the UBubbleCageComponent::bDecoupleViaTrait is set.
 */
USTRUCT(Category = "BubbleCage")
struct APPARATISTRUNTIME_API FCoupling
{
	GENERATED_BODY()
};

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
	 * The largest radius among all the bubbles.
	 * 
	 * Used for the coupling candidates detection.
	 */
	float LargestRadius = 0.0f;

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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Volume", Meta = (AllowPrivateAccess))
	float CellSize = 1;

	/**
	 * Inverse (1/x) cell size cached for performance.
	 */
	float InvCellSizeCache = 1;

	/**
	 * The total size of the cage in number of cells.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Volume", Meta = (AllowPrivateAccess))
	FIntVector Size;

	/**
	 * The bounds of the cage in world units.
	 * 
	 * These are precalculated at start.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Volume", Meta = (AllowPrivateAccess))
	mutable FBox Bounds;

	/**
	 * The decoupling algorithm will be run in parallel through a dedicated trait.
	 * 
	 * This will in turn result in copying of the coupling subjects.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Performance", Meta = (AllowPrivateAccess))
	bool bDecoupleViaTrait = false;


	bool bInitialized = false;

	/**
	 * All of the cells of the cage.
	 */
	TArray<FBubbleCageCell> Cells;

	/**
	 * The indices of the cells that are currently occupied by the subjects.
	 */
	TQueue<int32, EQueueMode::Mpsc> OccupiedCells;

	struct FCouplingEntry
	{
		FSubjectHandle Subject;

		FLocated* Located = nullptr;

		FBubbleSphere* BubbleSphere = nullptr;

		FORCEINLINE
		FCouplingEntry()
		{}

		FORCEINLINE
		FCouplingEntry(FSubjectHandle InSubject, FLocated* InLocated, FBubbleSphere* InBubbleSphere)
		  : Subject(InSubject), Located(InLocated), BubbleSphere(InBubbleSphere)
		{}
	};

	/**
	 * All the subjects that are actually coupling with each other and need decoupling.
	 */
	TQueue<FCouplingEntry, EQueueMode::Mpsc> CoupledSubjects;

	/**
	 * Initialize the internal cells array.
	 */
	void
	DoInitializeCells()
	{
		Cells.Reset(); // Make sure there are no cells.
		if (ensureAlwaysMsgf((int64)Size.X * (int64)Size.Y * (int64)Size.Z < (int64)TNumericLimits<int32>::Max(),
							 TEXT("The '%s' bubble cage has too many cells in it. Please, decrease its corresponding size in cells."),
							 *GetName()))
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

	/**
	 * Initialize the defaults for the bubble cage component.
	 */
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

	/**
	 * Convert a global 3D location to a position within the cage.
	 * 
	 * No bounding checks are performed.
	 */
	FORCEINLINE FIntVector
	WorldToCage(FVector Point) const
	{
		Point -= Bounds.Min;
		Point *= InvCellSizeCache;
		return FIntVector(FMath::FloorToInt(Point.X),
						  FMath::FloorToInt(Point.Y),
						  FMath::FloorToInt(Point.Z));
	}

	/**
	 * Convert a global 3D location to a position within the bounds.
	 * 
	 * No bounding checks are performed.
	 */
	FORCEINLINE FVector
	WorldToBounded(const FVector& Point) const
	{
		return Point - Bounds.Min;
	}

	/**
	 * Convert a cage-local 3D location to a position within the cage.
	 * 
	 * No bounding checks are performed.
	 */
	FORCEINLINE FIntVector
	BoundedToCage(FVector Point) const
	{
		Point *= InvCellSizeCache;
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
								const float DistanceSqr = Delta.SizeSquared();
								if (FMath::Square(OtherBubbleSphere.Radius) > DistanceSqr)
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
	int32
	GetOverlapping(const FVector&          Location,
				   const FFilter&          Filter,
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
						// Negative filtering can't be performed here,
						// since the cell's fingerprint includes a sum of internals.
						if (NeighbourCell.Fingerprint.Matches(Filter.GetFingerprint()))
						{
							for (int32 t = 0; t < NeighbourCell.Subjects.Num(); ++t)
							{
								const auto OtherBubble = NeighbourCell.Subjects[t];
								if (LIKELY(OtherBubble.Matches(Filter)))
								{
									const auto OtherBubbleSphere =
										OtherBubble.GetTrait<FBubbleSphere>();
									const auto OtherLocation =
										OtherBubble.GetTrait<FLocated>().GetLocation();
									const auto Delta = Location - OtherLocation;
									const float DistanceSqr = Delta.SizeSquared();
									if (FMath::Square(OtherBubbleSphere.Radius) > DistanceSqr)
									{
										OutOverlappers.Add(OtherBubble);
									}
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
								const auto DistanceSqr = Delta.SizeSquared();
								if (FMath::Square(Radius + OtherBubbleSphere.Radius) > DistanceSqr)
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
				   const FFilter&          Filter,
				   TArray<FSubjectHandle>& OutOverlappers) const
	{
		if (UNLIKELY(Radius == 0))
		{
			return GetOverlapping(Location, Filter, OutOverlappers);
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
						// Negative filtering can't be performed here,
						// since the cell's fingerprint includes a sum of internals.
						if (NeighbourCell.Fingerprint.Matches(Filter.GetFingerprint()))
						{
							for (int32 t = 0; t < NeighbourCell.Subjects.Num(); ++t)
							{
								const auto OtherBubble = NeighbourCell.Subjects[t];
								if (LIKELY(OtherBubble.Matches(Filter)))
								{
									const auto OtherBubbleSphere =
										OtherBubble.GetTrait<FBubbleSphere>();
									const auto OtherLocation =
										OtherBubble.GetTrait<FLocated>().GetLocation();
									const auto Delta = Location - OtherLocation;
									const auto DistanceSqr = Delta.SizeSquared();
									if (FMath::Square(Radius + OtherBubbleSphere.Radius) > DistanceSqr)
									{
										OutOverlappers.Add(OtherBubble);
									}
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
	FORCEINLINE  TArray<FSubjectHandle>
	GetOverlapping(const FVector& Location,
				   const float    Radius = 0.0f)
	{
		TArray<FSubjectHandle> Overlappers;
		GetOverlapping(Location, Radius, Overlappers);
		return MoveTemp(Overlappers);
	}

	/**
	 * Get overlapping bubbles within the within a radius and a filter.
	 * 
	 * @param Location The location to track.
	 * @param Radius The radius of overlapping.
	 * @param Filter The filter to narrow by.
	 * @return The overlapped bubbles.
	 */
	FORCEINLINE TArray<FSubjectHandle>
	GetOverlapping(const FVector& Location,
				   const float    Radius,
				   const FFilter& Filter)
	{
		TArray<FSubjectHandle> Overlappers;
		GetOverlapping(Location, Radius, Filter, Overlappers);
		return MoveTemp(Overlappers);
	}

	/**
	 * Get a position within the cage by an index of the cell.
	 */
	FORCEINLINE FIntVector
	GetCellPointByIndex(int32 Index) const
	{
		int32 z = Index / (Size.X * Size.Y);
		int32 LayerPadding = Index - (z * Size.X * Size.Y);

		return FIntVector(LayerPadding / Size.X, LayerPadding % Size.X , z);
	}

	/* Get the index of the cage cell. */
	FORCEINLINE int32
	GetIndexAt(const FIntVector& CellPoint) const
	{
		return GetIndexAt(CellPoint.X, CellPoint.Y, CellPoint.Z);
	}

	/* Get the index of the cell by the world position. */
	FORCEINLINE int32
	GetIndexAt(const FVector& Point) const
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
	 * Update the cage, filling it with existing bubbles.
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
			auto& Cell = Cells[CellIndex];
			Cell.Subjects.Empty();
			Cell.Fingerprint.Reset();
		}
		
		// Use atomic for a thread safety:
		std::atomic<float> AtomicLargestRadius{0};

		// Occupy the cage cells...
		static const auto Filter = FFilter::Make<FLocated, FBubbleSphere>();
		Mechanism->EnchainSolid(Filter)->OperateConcurrently(
		[&](FSolidSubjectHandle  Subject,
			const FLocated&      Located,
			FBubbleSphere&       BubbleSphere)
		{
			const auto Location = Located.Location;
			if (UNLIKELY(!IsInside(Location)))
			{
				Subject.DespawnDeferred();
				return;
			}
			{
				// Solve the largest radius...
				auto Largest = AtomicLargestRadius.load(std::memory_order_relaxed);
				if (Largest < BubbleSphere.Radius)
				{
					while (!AtomicLargestRadius.compare_exchange_weak(Largest, BubbleSphere.Radius, std::memory_order_relaxed))
					{
						if (Largest >= BubbleSphere.Radius)
						{
							break;
						}
					}
				}
			}

			BubbleSphere.CellIndex = GetIndexAt(Location);
			{
				auto& Cell = Cells[BubbleSphere.CellIndex];
				Cell.Lock();
				const auto Index = Cell.Subjects.Add((FSubjectHandle)Subject);
				Cell.Fingerprint.Add(Subject.GetFingerprint());
				Cell.Unlock();
				if (Index == 0)
				{
					OccupiedCells.Enqueue(BubbleSphere.CellIndex);
				}
			}			
		}, ThreadsCount);

		LargestRadius = AtomicLargestRadius.load(std::memory_order_relaxed);
	}

	template < bool bUseTrait >
	void
	DoDecouple()
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_BubbleCage_Decouple);

		const auto Mechanism = GetMechanism();

		static const auto Filter = FFilter::Make<FLocated, FBubbleSphere>();
		// Detect collisions...
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_BubbleCage_DetectCollisions);
			CoupledSubjects.Empty();
			Mechanism->EnchainSolid(Filter)->OperateConcurrently(
			[&](FSolidSubjectHandle Bubble,
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
									const auto OtherBubble = (FSolidSubjectHandle)NeighbourCell.Subjects[t];
									if (LIKELY(OtherBubble && (OtherBubble != Bubble)))
									{
										const auto& OtherBubbleSphere =
											OtherBubble.GetTraitRef<FBubbleSphere>();
										const auto OtherLocation =
											OtherBubble.GetTraitRef<FLocated>().GetLocation();
										const auto Delta = Location - OtherLocation;
										auto Distance = Delta.SizeSquared();
										const auto NeededDistance = BubbleSphere.Radius + OtherBubbleSphere.Radius;
										if (Distance < FMath::Square(NeededDistance))
										{
											Distance = FMath::Sqrt(Distance);
											const float DistanceDelta = NeededDistance - Distance;
											const float Strength = BubbleSphere.DecoupleProportion /
															(BubbleSphere.DecoupleProportion + OtherBubbleSphere.DecoupleProportion);
											// We're hitting a neighbor.
											if (UNLIKELY(Distance <= SMALL_NUMBER))
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
											if (BubbleSphere.AccumulatedDecoupleCount++ == 0)
											{
												if (bUseTrait) // Compile-time branch
												{
													Bubble.SetTraitDeferred(FCoupling{});
												}
												else
												{
													CoupledSubjects.Enqueue(FCouplingEntry((FSubjectHandle)Bubble, &Located, &BubbleSphere));
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}, ThreadsCount);
		}

		// Decouple...
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_BubbleCage_DecoupleThroughLocations);
			if (bUseTrait) // Compile-time branch.
			{
				Mechanism->OperateConcurrently(
				[&](FSolidSubjectHandle Subject,
					FLocated&           Located,
					FBubbleSphere&      BubbleSphere,
					const FCoupling&)
				{
					Located.Location += BubbleSphere.AccumulatedDecouple /
										BubbleSphere.AccumulatedDecoupleCount;
					BubbleSphere.AccumulatedDecouple = FVector::ZeroVector;
					BubbleSphere.AccumulatedDecoupleCount = 0;
					Subject.RemoveTraitDeferred<FCoupling>();

					if (UNLIKELY(!IsInside(Located.Location)))
					{
						Subject.DespawnDeferred();
						return;
					}

					const auto NewCellIndex = GetIndexAt(Located.Location);
					if (BubbleSphere.CellIndex != NewCellIndex)
					{
						auto& FormerCell = Cells[BubbleSphere.CellIndex];
						FormerCell.Lock();
						FormerCell.Subjects.Remove((FSubjectHandle)Subject);
						FormerCell.Unlock();
						auto& NewCell = Cells[NewCellIndex];
						NewCell.Lock();
						const auto Index = NewCell.Subjects.Add((FSubjectHandle)Subject);
						NewCell.Fingerprint.Add(Subject.GetFingerprint());
						NewCell.Unlock();
						BubbleSphere.CellIndex = NewCellIndex;
						if (Index == 0)
						{
							OccupiedCells.Enqueue(NewCellIndex);
						}
					}
				}, ThreadsCount);
			}
			else
			{
				FCouplingEntry Coupling;
				while (CoupledSubjects.Dequeue(Coupling))
				{
					if (Coupling.Subject && (Coupling.BubbleSphere->AccumulatedDecoupleCount > 0)) // Can already be handled and even despawned.
					{
						auto& Located      = *Coupling.Located;
						auto& BubbleSphere = *Coupling.BubbleSphere;
						check(BubbleSphere.AccumulatedDecoupleCount > 0);
						Located.Location += BubbleSphere.AccumulatedDecouple /
											BubbleSphere.AccumulatedDecoupleCount;
						BubbleSphere.AccumulatedDecouple = FVector::ZeroVector;
						BubbleSphere.AccumulatedDecoupleCount = 0;

						if (UNLIKELY(!IsInside(Located.Location)))
						{
							// We can't despawn normally here, since it will
							// screw up the direct trait references in the queue.
							Coupling.Subject.DespawnDeferred();
							continue;
						}

						const auto NewCellIndex = GetIndexAt(Located.Location);
						if (BubbleSphere.CellIndex != NewCellIndex)
						{
							// Not using locks here, cause we're in a single-threaded mode...
							auto& FormerCell = Cells[BubbleSphere.CellIndex];
							FormerCell.Subjects.Remove(Coupling.Subject);
							auto& NewCell = Cells[NewCellIndex];
							const auto Index = NewCell.Subjects.Add(Coupling.Subject);
							NewCell.Fingerprint.Add(Coupling.Subject.GetFingerprint());
							BubbleSphere.CellIndex = NewCellIndex;
							if (Index == 0)
							{
								OccupiedCells.Enqueue(NewCellIndex);
							}
						}
					}
				}
				Mechanism->ApplyDeferreds();
			}
		}
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
		if (bDecoupleViaTrait)
		{
			DoDecouple<true>();
		}
		else
		{
			DoDecouple<false>();
		}
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
	}
};
