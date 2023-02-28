/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageCell.h
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

#pragma once

#include "CoreMinimal.h"

#include "Machine.h"
#include "SubjectHandles8.h"

#include "BubbleCageCell.generated.h"


/**
 * Struct representing a one grid cell.
 */
USTRUCT(BlueprintType, Category = "BubbleCage")
struct APPARATISTRUNTIME_API FBubbleCageCell
{
	GENERATED_BODY()

  private:

	mutable std::atomic<bool> LockFlag{false};

  public:

  	void Lock() const
	{
		while (LockFlag.exchange(true, std::memory_order_acquire));
	}

  	void Unlock() const
	{
		LockFlag.store(false, std::memory_order_release);
	}

	/**
	 * The list of subjects with their centers within this cage cell.
	 * 
	 * Only a single cell can contain an individual subject at a time.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BubbleCage")
	FSubjectHandles8 Subjects;

	/**
	 * The accumulated fingerprint of all subjects within this cell.
	 * 
	 * This can actually be more inclusive than actually is,
	 * since each bubble will be matched individually.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BubbleCage")
	FFingerprint Fingerprint;

	FBubbleCageCell()
	{}

	FBubbleCageCell(const FBubbleCageCell& Cell)
	{
		LockFlag.store(Cell.LockFlag.load());
		Subjects = Cell.Subjects;
		Fingerprint = Cell.Fingerprint;
	}

	FBubbleCageCell& operator=(const FBubbleCageCell& Cell)
	{
		LockFlag.store(Cell.LockFlag.load());
		Subjects = Cell.Subjects;
		Fingerprint = Cell.Fingerprint;
		return *this;
	}
};
