/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: NetworkBearerComponent.inl
 * Created: 2022-04-16 22:20:28
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

FORCEINLINE AMechanism*
UNetworkBearerComponent::GetMechanism() const
{
	return UMachine::ObtainMechanism(GetWorld());
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
UNetworkBearerComponent::DoPushSubjectTrait(
	const uint32         SubjectNetworkId,
	UScriptStruct* const TraitType,
	const void* const    TraitData,
	EPeerRole            PeerRole/*=EPeerRole::Auto*/,
	const bool           bReliable/*=true*/)
{
	if (UNLIKELY(TraitType == nullptr))
	{
		return EApparatusStatus::Noop;
	}

	AssessCondition(Paradigm, TraitData != nullptr, EApparatusStatus::NullArgument);
	AssessCondition(Paradigm, SubjectNetworkId != FSubjectNetworkState::InvalidId, EApparatusStatus::InvalidArgument);

	const auto Actor = GetOwner();
	AssessCondition(Paradigm, Actor != nullptr, EApparatusStatus::InvalidState);

	const auto World = Actor->GetWorld();
	AssessCondition(Paradigm, World != nullptr, EApparatusStatus::InvalidState);

	const auto Connection = Actor->GetNetConnection();

	if (UNLIKELY(Connection == nullptr))
	{
		// We're in a listen server or standalone mode.
		AssessConditionFormat(
			Paradigm, Actor->IsNetMode(NM_ListenServer) || World->IsNetMode(NM_Standalone), EApparatusStatus::InvalidState,
			TEXT("The network bearer must be in a listen server/standalone mode when there's no connection for it."));

		const auto Mechanism = GetMechanism();
		AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);
		const auto Info = Mechanism->FindSubjectInfoByNetworkId(SubjectNetworkId);
		if (LIKELY(Info != nullptr))
		{
			// When we're in a listen server mode,
			// just set the trait directly:
			return Info->template SetTrait<Paradigm>(TraitType, TraitData);
		}
		else
		{
			// No subject found with the specified network identifier.
			// This should generally never happen though, but is ignored silently:
			return EApparatusStatus::Noop;
		}
	}

	const auto PackageMap = Connection->PackageMap;
	const auto TotalSizeBytes = TraitType->GetPropertiesSize();
	FNetBitWriter Writer(PackageMap, ((int64)TotalSizeBytes) * 8);
	TraitType->SerializeBin(Writer, const_cast<void* const>(TraitData));

	check(!World->IsNetMode(NM_Standalone));
	if (LIKELY(World->IsNetMode(NM_DedicatedServer) || World->IsNetMode(NM_ListenServer)))
	{
		if (LIKELY(bReliable))
		{
			ClientMechanismReceiveSubjectTrait(SubjectNetworkId, TraitType, *Writer.GetBuffer());
		}
		else
		{
			ClientMechanismReceiveSubjectTraitUnreliable(SubjectNetworkId, TraitType, *Writer.GetBuffer());
		}
	}
	else if (ensure(World->IsNetMode(NM_Client)))
	{
		if (LIKELY(bReliable))
		{
			ServerMechanismReceiveSubjectTrait(SubjectNetworkId, TraitType, *Writer.GetBuffer());
		}
		else
		{
			ServerMechanismReceiveSubjectTraitUnreliable(SubjectNetworkId, TraitType, *Writer.GetBuffer());
		}
	}

	return EApparatusStatus::Success;
}

FORCEINLINE void
UNetworkBearerComponent::ClientMechanismReceiveSubject_Implementation(const uint32 InSubjectNetworkId)
{
	auto* const Mechanism = GetMechanism();
	if (LIKELY(ensureMsgf(Mechanism,
						  TEXT("Network bearer must be within a mechanism in order to receive a subject."))))
	{
		check(Mechanism->GetWorld()->IsNetMode(NM_Client));
		auto* const Info = Mechanism->FindSubjectInfoByNetworkId(InSubjectNetworkId);
		if (UNLIKELY(Info))
		{
			return;
		}
		auto Subject = Mechanism->SpawnSubject();
		Info->ObtainNetworkState(InSubjectNetworkId);
	}
}

inline EApparatusStatus
UNetworkBearerComponent::PeerMechanismReceiveSubjectTrait_Implementation(
		const EPeerRole      PeerRole,
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	check(PeerRole != EPeerRole::None);
	check(TraitType != nullptr);

	auto* const Mechanism = GetMechanism();
	if (UNLIKELY(!ensureMsgf(Mechanism,
							 TEXT("Network bearer must be within a mechanism in order to receive a subject."))))
	{
		return EApparatusStatus::InvalidState;
	}

	const auto World = Mechanism->GetWorld();
	if (UNLIKELY(!ensureMsgf(World,
							 TEXT("A world context must be present in order to receive the subject trait."))))
	{
		return EApparatusStatus::InvalidState;
	}

	const auto Actor = GetOwner();
	if (UNLIKELY(!ensure(Actor)))
	{
		return EApparatusStatus::InvalidState;
	}
	const auto Connection = Actor->GetNetConnection();

	auto* Info = Mechanism->FindSubjectInfoByNetworkId(SubjectNetworkId);
	if (UNLIKELY(!Info))
	{
		// Seems like the subject is actually missing,
		// so try to spawn it if it's on a client...
		// Subject is missing on a client. Spawn a new one...
		auto Subject = Mechanism->SpawnSubject();
		Info = &Subject.GetInfo();
		Info->ObtainNetworkState(SubjectNetworkId);
	}

	check(Info != nullptr);

	const auto PackageMap = Connection ? Connection->PackageMap : nullptr;
	FNetBitReader Reader(PackageMap, const_cast<uint8*>(TraitData.GetData()),
						 ((int64)TraitData.Num()) * 8);
	TraitType->SerializeBin(Reader, Info->ObtainTraitPtr(TraitType));
	return EApparatusStatus::Success;
}

inline bool
UNetworkBearerComponent::PeerMechanismReceiveSubjectTrait_Validate(
		const EPeerRole      PeerRole,
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	check(PeerRole != EPeerRole::None);
	const auto Mechanism = GetMechanism();
	if (LIKELY(ensureMsgf(Mechanism,
						  TEXT("Network bearer must be within a mechanism in order to receive a subject."))))
	{
		if (UNLIKELY(!ensureMsgf(TraitType,
								 TEXT("No trait type provided to receive."))))
		{
			return false;
		}

		const auto Actor = GetOwner();
		if (UNLIKELY(!ensure(Actor)))
		{
			return false;
		}
		const auto Connection = Actor->GetNetConnection();

		auto* Info = Mechanism->FindSubjectInfoByNetworkId(SubjectNetworkId);
		if (UNLIKELY(!Info))
		{
			// Seems like the subject is actually missing,
			// so try to spawn it if it's on a client...
			if (!ensureMsgf((PeerRole == EPeerRole::Client || !Connection) && (Actor->IsNetMode(NM_Client) || Actor->IsNetMode(NM_ListenServer)),
							TEXT("Network subject identifier is inside an invalid network context to be spawned: %u"), SubjectNetworkId))
			{
				Report<EApparatusStatus::NoPermission>(
					TEXT("A remote-spawning of the subject through the '{0}' bearer is insecure and is not allowed on the server. "
						 "Happened due to receiving the '{1}' trait."), {GetName(), TraitType->GetName()});
				return false;
			}
		}

		// Check for the passes on the server...
		if ((PeerRole == EPeerRole::Server)
		 && (Actor->IsNetMode(NM_DedicatedServer) || Actor->IsNetMode(NM_ListenServer)))
		{
			const auto NetworkState = Info->ObtainNetworkState();
			if (UNLIKELY((Info->GetConnectionPermit() != Connection)
					  || !NetworkState->TraitmarkPermit.Contains(TraitType)))
			{
				return false;
			}
		}

		return TraitType->GetPropertiesSize() == TraitData.Num();
	}

	return false;
}

FORCEINLINE void
UNetworkBearerComponent::ServerMechanismReceiveSubjectTrait_Implementation(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	ensure(OK(PeerMechanismReceiveSubjectTrait_Implementation(EPeerRole::Server, SubjectNetworkId, TraitType, TraitData)));
}

FORCEINLINE bool
UNetworkBearerComponent::ServerMechanismReceiveSubjectTrait_Validate(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	return PeerMechanismReceiveSubjectTrait_Validate(EPeerRole::Server, SubjectNetworkId, TraitType, TraitData);
}

FORCEINLINE void
UNetworkBearerComponent::ServerMechanismReceiveSubjectTraitUnreliable_Implementation(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	ensure(OK(PeerMechanismReceiveSubjectTrait_Implementation(EPeerRole::Server, SubjectNetworkId, TraitType, TraitData)));
}

FORCEINLINE bool
UNetworkBearerComponent::ServerMechanismReceiveSubjectTraitUnreliable_Validate(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	return PeerMechanismReceiveSubjectTrait_Validate(EPeerRole::Server, SubjectNetworkId, TraitType, TraitData);
}

FORCEINLINE void
UNetworkBearerComponent::ClientMechanismReceiveSubjectTrait_Implementation(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	EnsureOK(PeerMechanismReceiveSubjectTrait_Implementation(EPeerRole::Client, SubjectNetworkId, TraitType, TraitData));
}

FORCEINLINE void
UNetworkBearerComponent::ClientMechanismReceiveSubjectTraitUnreliable_Implementation(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	EnsureOK(PeerMechanismReceiveSubjectTrait_Implementation(EPeerRole::Client, SubjectNetworkId, TraitType, TraitData));
}

FORCEINLINE bool
UNetworkBearerComponent::ClientMechanismReceiveSubjectTrait_Validate(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	return PeerMechanismReceiveSubjectTrait_Validate(EPeerRole::Client, SubjectNetworkId, TraitType, TraitData);
}

FORCEINLINE bool
UNetworkBearerComponent::ClientMechanismReceiveSubjectTraitUnreliable_Validate(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData)
{
	return PeerMechanismReceiveSubjectTrait_Validate(EPeerRole::Client, SubjectNetworkId, TraitType, TraitData);
}

FORCEINLINE void
UNetworkBearerComponent::ClientMechanismReceiveSubjectWithTraits_Implementation(
	const uint32                  InSubjectNetworkId,
	const TArray<UScriptStruct*>& TraitsTypes,
	const TArray<uint8>&          TraitsData)
{
	auto* const Mechanism = GetMechanism();
	if (UNLIKELY(!ensureMsgf(Mechanism,
							 TEXT("Subjective must be within a mechanism in order to receive a subject with traits."))))
	{
		return;
	}
	check(Mechanism->GetWorld()->IsNetMode(NM_Client));

	auto* const Actor = GetOwner();
	if (UNLIKELY(!Actor))
	{
		return;
	}
	auto* const Connection = Actor->GetNetConnection();
	const auto PackageMap = Connection ? Connection->PackageMap : nullptr;
	int32 TotalSizeBytes = 0;
	// Calculate the needed number of bytes...
	for (int32 i = 0; i < TraitsTypes.Num(); ++i)
	{
		auto* const TraitType = TraitsTypes[i];
		if (UNLIKELY(!ensureMsgf(TraitType,
								 TEXT("Missing a type specification. Does the client's version match the server's?"))))
		{
			return;
		}
		TotalSizeBytes += TraitType->GetPropertiesSize();
	}
	if (UNLIKELY(!ensureMsgf(TotalSizeBytes == TraitsData.Num(),
							 TEXT("The numbers of traits bytes do not match. Does the client's version match the server's?"))))
	{
		return;
	}

	// Deserialize the traits now...
	FSubjectRecord SubjectRecord;	
	FNetBitReader Reader(PackageMap, const_cast<uint8*>(TraitsData.GetData()),
						 ((int64)TotalSizeBytes) * 8);
	for (int32 i = 0; i < TraitsTypes.Num(); ++i)
	{
		const auto TraitType = TraitsTypes[i];
		const auto TraitData = SubjectRecord.ObtainTraitPtr(TraitType);;
		TraitType->SerializeBin(Reader, TraitData);
	}

	auto* Info = Mechanism->FindSubjectInfoByNetworkId(InSubjectNetworkId);
	if (UNLIKELY(!Info))
	{
		auto Subject = Mechanism->SpawnSubject();
		Info = &Subject.GetInfo();
		Info->ObtainNetworkState(InSubjectNetworkId);
	}

	verify(OK(Info->SetTraits(SubjectRecord, /*bLeaveRedundant=*/false)));
}

FORCEINLINE void
UNetworkBearerComponent::PushSubjectToClient(
	const FSolidSubjectHandle& Subject)
{
	const auto* const Info = Subject.FindInfo();
	if (UNLIKELY(!ensureMsgf(Info, TEXT("A subject to send the traits of must be valid."))))
	{
		return;
	}
	const auto* const Chunk = Info->Chunk;
	check(Chunk);
	auto* const Actor = GetOwner();
	if (UNLIKELY(!Actor))
	{
		return;
	}
	auto* const Connection = Actor->GetNetConnection();
	const auto PackageMap = Connection ? Connection->PackageMap : nullptr;
	// Calculate the total size now.
	int64 TotalSizeBytes = 0;
	for (int32 i = 0; i < Chunk->TraitLinesNum(); ++i)
	{
		const auto* const TraitType = Chunk->TraitLineTypeAt(i);
		TotalSizeBytes += TraitType->GetPropertiesSize();
	}
	FNetBitWriter Writer(PackageMap, TotalSizeBytes * 8);
	for (int32 i = 0; i < Chunk->TraitLinesNum(); ++i)
	{
		const auto TraitType = Chunk->TraitLineTypeAt(i);
		const auto* const TraitData = Chunk->TraitPtrAt(Info->SlotIndex, i);
		TraitType->SerializeBin(Writer, const_cast<void* const>(TraitData));
	}
	const auto NetworkId = Info->GetNetworkState()->GetId();
	check(NetworkId != FSubjectNetworkState::InvalidId);
	ClientMechanismReceiveSubjectWithTraits(
		NetworkId,
		TArray<UScriptStruct*>(Chunk->GetTraitmark().GetTraits()),
		*Writer.GetBuffer());
}

FORCEINLINE void
UNetworkBearerComponent::PushSubjectToClient(const uint32 InSubjectNetworkId)
{
	const auto Mechanism = GetMechanism();
	if (ensure(Mechanism))
	{
		const auto SubjectInfo = Mechanism->FindSubjectInfoByNetworkId(InSubjectNetworkId);
		if (LIKELY(SubjectInfo))
		{
			PushSubjectToClient(SubjectInfo->GetHandle<FSolidSubjectHandle>());
		}
	}
}

inline void
UNetworkBearerComponent::PushSubjectToClient(const uint32          NetworkId,
											 const FSubjectRecord& SubjectRecord)
{
	check(NetworkId != FSubjectNetworkState::InvalidId);
	auto* const Actor = GetOwner();
	if (UNLIKELY(!Actor))
	{
		return;
	}
	auto* const Connection = Actor->GetNetConnection();
	const auto PackageMap = Connection ? Connection->PackageMap : nullptr;
	// Calculate the total size now.
	int64 TotalSizeBytes = 0;
	TArray<UScriptStruct*> Types;
	Types.Reserve(SubjectRecord.TraitsNum());
	for (int32 i = 0; i < SubjectRecord.TraitsNum(); ++i)
	{
		auto* const TraitType = SubjectRecord.TraitAt(i).GetType();
		Types.Add(TraitType);
		TotalSizeBytes += TraitType->GetPropertiesSize();
	}
	FNetBitWriter Writer(PackageMap, TotalSizeBytes * 8);
	for (int32 i = 0; i < SubjectRecord.TraitsNum(); ++i)
	{
		const auto  Trait = SubjectRecord.TraitAt(i);
		const auto  TraitType = Trait.GetType();
		void* const TraitData = const_cast<void*>(Trait.GetData());
		TraitType->SerializeBin(Writer, TraitData);
	}
	ClientMechanismReceiveSubjectWithTraits(NetworkId, Types, *Writer.GetBuffer());
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
UNetworkBearerComponent::BroadcastTrait(UWorld* const        World,
										const uint32         SubjectNetworkId,
										UScriptStruct* const TraitType,
										const void* const    TraitData,
										EPeerRole            PeerRole/*=EPeerRole::Auto*/,
										const bool           bReliable/*=true*/)
{
	AssessCondition(Paradigm, World != nullptr, EApparatusStatus::NullArgument);
	if (UNLIKELY(TraitType == nullptr))
	{
		return EApparatusStatus::Noop;
	}
	bool bChange = false;
	AssessCondition(Paradigm, TraitData != nullptr, EApparatusStatus::NullArgument);
	// On a server, send the packed networked subject to all the clients in the same world.
	// On a client there will be only one player controller to use that will be used to transfer the trait...
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		const auto PlayerController = Iterator->Get();
		if (LIKELY(PlayerController))
		{
			const auto NetworkBearer = PlayerController->FindComponentByClass<UNetworkBearerComponent>();
			if (LIKELY(NetworkBearer != nullptr))
			{
				AssessOK(Paradigm, NetworkBearer->template DoPushSubjectTrait<Paradigm>(SubjectNetworkId, TraitType, TraitData, PeerRole, bReliable));
				bChange = true;
			}
		}
	}
	return bChange ? EApparatusStatus::Success : EApparatusStatus::Noop;
}
