/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: CommonSubjectHandle.inl
 * Created: 2022-04-17 17:04:12
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

FORCEINLINE FSubjectInfo*
FCommonSubjectHandle::FindInfo() const
{
	if (UNLIKELY(Id == InvalidId))
	{
		// Checking the identifier is enough,
		// as generation is not checked to be valid:
		return nullptr;
	}

	// The subject infos are never deallocated,
	// so can use direct getter here:
	auto& Info = UMachine::GetSubjectInfo(Id);
	// Check if the handle is actually outdated:
	if (UNLIKELY(Generation != Info.Generation))
	{
		// Clear the state for faster later checks...
		const_cast<FCommonSubjectHandle*>(this)->Id = InvalidId;
		// It is enough to clean the id alone. Generation won't be
		// considered if the first is already invalid.
		return nullptr;
	}

	return &Info;
}

FORCEINLINE FSubjectInfo&
FCommonSubjectHandle::GetInfo() const
{
	check(Id != InvalidId);

	auto& Info = UMachine::GetSubjectInfo(Id);
	// Check if the handle is actually outdated:
	check(Generation == Info.Generation);
	// We do not invalidate ourselves here,
	// since we provide a crash instead.

	return Info;
}

FORCEINLINE uint32
FCommonSubjectHandle::CalcHash() const
{
	if (LIKELY(IsValid()))
	{
		return HashCombine(GetTypeHash(Id), GetTypeHash(Generation));
	}
	// Invalid handles are all the same and zero:
	return 0;
}

FORCEINLINE UChunk*
FCommonSubjectHandle::GetChunk() const
{
	const auto Info = FindInfo();
	if (LIKELY(Info))
	{
		return Info->Chunk;
	}
	return nullptr;
}

FORCEINLINE FFingerprint&
FCommonSubjectHandle::GetFingerprintRef() const
{
	return GetInfo().GetFingerprintRef();
}

FORCEINLINE const FFingerprint&
FCommonSubjectHandle::GetFingerprint() const
{
	return GetInfo().GetFingerprint();
}

FORCEINLINE AMechanism*
FCommonSubjectHandle::GetMechanism() const
{
	const auto Info = FindInfo();
	if (LIKELY(Info))
	{
		return Info->GetMechanism();
	}
	return nullptr;
}

FORCEINLINE ISubjective*
FCommonSubjectHandle::GetSubjective() const
{
	const auto Info = FindInfo();
	if (LIKELY(Info))
	{
		return Info->GetSubjective();
	}
	return nullptr;
}

FORCEINLINE UDetail*
FCommonSubjectHandle::GetDetail(TSubclassOf<UDetail> DetailClass) const
{
	const auto Subjective = GetSubjective();
	if (LIKELY(Subjective))
	{
		return Subjective->GetDetail(DetailClass);
	}
	return nullptr;
}

template < class D >
FORCEINLINE D*
FCommonSubjectHandle::GetDetail() const
{
	const auto Subjective = GetSubjective();
	if (LIKELY(Subjective))
	{
		return Subjective->template GetDetail<D>();
	}
	return nullptr;
}

FORCEINLINE bool
FCommonSubjectHandle::IsOnline() const
{
	const auto Info = FindInfo();
	if (LIKELY(Info))
	{
		return Info->IsOnline();
	}
	return false;
}

FORCEINLINE uint32
FCommonSubjectHandle::GetNetworkId() const
{
	const auto Info = FindInfo();
	if (LIKELY(Info))
	{
		return Info->GetNetworkId();
	}
	return FSubjectNetworkState::InvalidId;
}

FORCEINLINE bool
FCommonSubjectHandle::IsClientSide() const
{
	const auto Info = FindInfo();
	if (LIKELY(Info))
	{
		return Info->IsClientSide();
	}
	return false;
}

FORCEINLINE bool
FCommonSubjectHandle::IsServerSide() const
{
	const auto Info = FindInfo();
	if (LIKELY(Info))
	{
		return Info->IsServerSide();
	}
	return false;
}

FORCEINLINE UNetConnection*
FCommonSubjectHandle::GetConnectionPermit() const
{
	const auto Info = FindInfo();
	if (LIKELY(ensure(Info)))
	{
		return Info->GetConnectionPermit();
	}
	return nullptr;
}

FORCEINLINE const FTraitmark&
FCommonSubjectHandle::GetTraitmarkPermit() const
{
	const auto Info = FindInfo();
	if (LIKELY(ensure(Info)))
	{
		return Info->GetTraitmarkPermit();
	}
	return FTraitmark::Zero;
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, bool>
FCommonSubjectHandle::MarkBooted() const
{
	static_assert(IsInternal(Paradigm),
				  "Marking a subject as booted can only be done under an internal paradigm.");
	const auto Info = FindInfo();
	if (AvoidCondition(Paradigm, Info == nullptr))
	{
		return MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidState, false);
	}
	return Info->template MarkBooted<Paradigm>();
}
