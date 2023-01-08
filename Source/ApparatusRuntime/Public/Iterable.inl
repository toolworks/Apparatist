/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Iterable.inl
 * Created: 2022-04-16 22:42:47
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

template < bool bInSolid >
inline EApparatusStatus
IIterable::Lock() const
{
	const auto Mechanism = GetOwner();
	checkf(Mechanism != nullptr, TEXT("Iterables without an owning mechanism can't be locked."));
	check(Count >= 0);

	const auto Object = CastChecked<UObject>(this);
	LocksCountValueType FormerValue;
	if (bInSolid) // Compile-time branch.
	{
		FormerValue = LocksCount.fetch_sub(1, std::memory_order_relaxed);
		checkf(FormerValue <= 0,
			   TEXT("The solidity must match during the locking of the '%s' iterable."),
			   *(CastChecked<UObject>(this)->GetName()));
	}
	else
	{
		FormerValue = LocksCount.fetch_add(1, std::memory_order_relaxed);
		checkf(FormerValue >= 0,
			   TEXT("The liquidity must match during the locking of the '%s' iterable."),
			   *(CastChecked<UObject>(this)->GetName()));
	}

	if (FormerValue == 0)
	{
		FScopeLock Lock(&LockageMutex);
		if (bInSolid) // Compile-time branch.
		{
			Mechanism->Solidify();
		}
		else
		{
			Mechanism->Liquify();
		}
		IterableCount = Count;
	}
	return EApparatusStatus::Success;
}

template < bool bInSolid >
inline EApparatusStatus
IIterable::Unlock() const
{
	const auto Mechanism = GetOwner();
	checkf(Mechanism != nullptr, TEXT("Iterables without an owning mechanism can't be locked."));
	check(Count >= 0);

	const auto Object = CastChecked<UObject>(this);
	if (bInSolid) // Compile-time branch.
	{
		const auto FormerValue = LocksCount.fetch_add(1, std::memory_order_relaxed);
		checkf(FormerValue < 0,
			   TEXT("The solidity must match during the unlocking of the '%s' iterable."),
			   *(CastChecked<UObject>(this)->GetName()));
		{
			if (FormerValue == -1)
			{
				FScopeLock Lock(&LockageMutex);
				Mechanism->Desolidify();
				verify(OK(DoUnlock(bInSolid)));
				IterableCount = -1;
			}
		}
	}
	else
	{
		const auto FormerValue = LocksCount.fetch_sub(1, std::memory_order_relaxed);
		checkf(FormerValue > 0,
			   TEXT("The liquidity must match during the unlocking of the '%s' iterable."),
			   *(CastChecked<UObject>(this)->GetName()));
		{
			if (FormerValue == 1)
			{
				FScopeLock Lock(&LockageMutex);
				Mechanism->Deliquify();
				verify(OK(DoUnlock(bInSolid)));
				IterableCount = -1;
			}
		}
	}
	return EApparatusStatus::Success;
}
