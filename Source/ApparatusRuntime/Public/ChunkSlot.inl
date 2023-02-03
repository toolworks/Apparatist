/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ChunkSlot.inl
 * Created: 2022-04-16 16:57:07
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

FORCEINLINE bool
FChunkSlot::IsMoved(UChunk* const OrigChunk) const
{
	return IsStale() && (SubjectId != FSubjectInfo::InvalidId) &&
			(GetInfo().GetChunk() != OrigChunk);
}

FORCEINLINE FChunkSlot&
FChunkSlot::operator=(FSubjectInfo* const InInfo)
{
	if (LIKELY(InInfo))
	{
		if (LIKELY(InInfo->GetId() != SubjectId))
		{
			check(IsStale());
			check(InInfo->IsValid())
			SubjectId = InInfo->GetId();
		}
	}
	else
	{
		SubjectId = FSubjectInfo::InvalidId;
	}
	Fingerprint.SetFlag(EFlagmarkBit::Stale, SubjectId == FSubjectInfo::InvalidId);
	return *this;
}

FORCEINLINE FChunkSlot&
FChunkSlot::operator=(const FCommonSubjectHandle& InHandle)
{
	if (LIKELY(InHandle.IsValid()))
	{
		if (LIKELY(InHandle.GetId() != SubjectId))
		{
			check(IsStale());
			SubjectId = InHandle.GetId();
		}
	}
	else
	{
		SubjectId = FSubjectInfo::InvalidId;
	}
	Fingerprint.SetFlag(EFlagmarkBit::Stale, SubjectId == FSubjectInfo::InvalidId);
	return *this;
}

template < typename SubjectHandleT >
FORCEINLINE SubjectHandleT
FChunkSlot::GetHandle() const
{
	if (LIKELY(SubjectId != FSubjectInfo::InvalidId))
	{
		return UMachine::GetSubjectInfo(SubjectId).template GetHandle<SubjectHandleT>();
	}

	return SubjectHandleT();
}

FORCEINLINE const FSubjectInfo&
FChunkSlot::GetInfo() const
{
	return UMachine::GetSubjectInfo(SubjectId);
}

FORCEINLINE FSubjectInfo&
FChunkSlot::GetInfo()
{
	return UMachine::GetSubjectInfo(SubjectId);
}

FORCEINLINE const FSubjectInfo*
FChunkSlot::FindInfo() const
{
	return UMachine::FindSubjectInfo(SubjectId);
}

FORCEINLINE FSubjectInfo*
FChunkSlot::FindInfo()
{
	return UMachine::FindSubjectInfo(SubjectId);
}
