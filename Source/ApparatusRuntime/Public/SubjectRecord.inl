/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectRecord.inl
 * Created: 2023-01-05 19:20:30
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

inline
FSubjectRecord::FSubjectRecord(const FConstSubjectHandle& Subject,
							   const EFlagmark            FlagmarkMask/*=FM_AllUserLevel*/)
{
	const auto& Info = Subject.GetInfo();
	const auto Chunk = Info.GetChunk();
	Traits.Reserve(Chunk->TraitLinesNum());
	for (int32 i = 0; i < Chunk->TraitLinesNum(); ++i)
	{
		Traits.Add(FTraitRecord(Chunk->TraitLineTypeAt(i),
								Chunk->TraitPtrAt(Info.SlotIndex, i)));
	}
	Flagmark    = (int32)(Info.GetFlagmark() & FlagmarkMask);
	FingerprintCache = Info.GetFingerprint();
	FingerprintCache.SetFlagmark((EFlagmark)Flagmark);
	FingerprintCache.RemoveAllDetails();
}

inline
FSubjectRecord::FSubjectRecord(const FConstSubjectHandle& Subject,
							   const FFilter&             Mask)
{
	const auto& Info = Subject.GetInfo();
	const auto Chunk = Info.GetChunk();
	for (int32 i = 0; i < Chunk->TraitLinesNum(); ++i)
	{
		const auto TraitType = Chunk->TraitLineTypeAt(i);
		check(TraitType != nullptr);
		if (!Mask.Includes(TraitType))
		{
			continue;
		}
		Traits.Add(FTraitRecord(Chunk->TraitLineTypeAt(i),
								Chunk->TraitPtrAt(Info.SlotIndex, i)));
	}
	Flagmark    = (int32)(Info.GetFlagmark() & Mask.GetFlagmark());
	FingerprintCache.SetFlagmark((EFlagmark)Flagmark);
	FingerprintCache.SetTraitmark(Mask.GetTraitmark());
}

template < typename AllocatorT >
inline void
FSubjectRecord::SerializeBin(const TArray<UScriptStruct*, AllocatorT>& InTraitTypes,
							 const EFlagmark                           InFlagmark,
							 FArchive&                                 Archive)
{
	checkf(Archive.IsLoading(), TEXT("When using a predefined list of trait types, the archive should be a loading one."));

	Traits.Reset(InTraitTypes.Num());
	FingerprintCache.Reset(InFlagmark);

	Flagmark = (int32)InFlagmark;

	for (int32 i = 0; i < InTraitTypes.Num(); ++i)
	{
		const auto TraitType = InTraitTypes[i];
		if (UNLIKELY(TraitType == nullptr)) continue;
		FingerprintCache.Add(TraitType);
		auto& Trait = Traits.Add(TraitType);
		TraitType->SerializeBin(Archive, Trait.GetData());
	}
}
