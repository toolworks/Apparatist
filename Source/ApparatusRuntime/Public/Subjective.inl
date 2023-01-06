/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Subjective.inl
 * Created: 2022-04-16 19:43:45
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

FORCEINLINE_DEBUGGABLE void
ISubjective::DoRegister()
{
	const auto Object = CastChecked<UObject>(this);
	if (UNLIKELY(Object->HasAnyFlags(RF_ClassDefaultObject)))
	{
		return;
	}
	checkf(!bUnregistered, TEXT("The '%s' subjective was already unregistered and can't be registered again."),
		   *Object->GetName());

	AMechanism* const MechanismOverride = this->GetMechanismOverridePropRef();
	const auto World = Object->GetWorld();
	if (LIKELY(World))
	{
		if (MechanismOverride
		 && ensureMsgf(MechanismOverride->GetWorld() == World,
		 			   TEXT("The mechanism override for the '%s' subjective must be within the same '%s' world."),
					   *Object->GetName(), *World->GetName()))
		{
			EnsureOK(MechanismOverride->RegisterSubjective(this));
		}
		else
		{
			EnsureOK(UMachine::RetainInstance()->ObtainMechanism(World)->RegisterSubjective(this));
		}
	}
	else
	{
		if (MechanismOverride)
		{
			EnsureOK(MechanismOverride->RegisterSubjective(this));
		}
		else
		{
			const auto Package = Object->GetPackage();
			if (LIKELY(Package))
			{
				EnsureOK(UMachine::RetainInstance()->ObtainMechanism(Package)->RegisterSubjective(this));
			}
		}
	}
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
TOutcome<Paradigm>
ISubjective::DoUnregister()
{
	if (UNLIKELY(bUnregistered))
	{
		return EApparatusStatus::Noop;
	}

	// This must be set beforehand for the
	// correct nested behavior:
	bUnregistered = true;

	const auto Mechanism = GetMechanism();
	if (LIKELY(Mechanism))
	{
		return Mechanism->template UnregisterSubjective<Paradigm>(this);
	}
	else
	{
		// The mechanism is no longer available.
		// Reset the state manually.
		Handle.ResetHandle();
		TakeBeltSlot(nullptr);
	}

	return EApparatusStatus::Success;
}

template < EParadigm Paradigm/*=EParadigm::DefaultPortable*/ >
FORCEINLINE TOutcome<Paradigm>
ISubjective::Unregister()
{
	return DoUnregister<Paradigm>();
}

FORCEINLINE void
ISubjective::DoStartSerialization(FArchive& Archive)
{
	const auto Info = Handle.FindInfo();
	if (Info && Archive.IsSaving())
	{
		auto& Records = GetTraitRecordsRef();
		Records.Reset();
		const auto Chunk = Info->Chunk;
		for (int32 i = 0; i < Chunk->TraitLinesNum(); ++i)
		{
			Records.Add({Chunk->TraitLineTypeAt(i),
						 Chunk->TraitPtrAt(Info->SlotIndex, i)});
		}
	}
}

FORCEINLINE void
ISubjective::DoFinishSerialization(FArchive& Archive)
{
	const auto Info = Handle.FindInfo();
	if (Info && Archive.IsLoading())
	{
		auto& Records = GetTraitRecordsRef();
		Info->RemoveAllTraits();
		for (int32 i = 0; i < Records.Num(); ++i)
		{
			const auto& Trait = Records[i];
			const auto Type = Trait.GetType();
			if (LIKELY(Type))
			{
				Info->SetTrait(Type, Trait.GetData());
			}
		}
	}
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/, class S/*=void*/ >
inline TOutcome<Paradigm>
ISubjective::DoPushTrait(const S* const       Subjective,
						 UScriptStruct* const TraitType,
						 EPeerRole            PeerRole,
						 const bool           bReliable,
						 void (S::*ServerReceiveTraitRPC)(UScriptStruct*       TraitType,
						 								  const TArray<uint8>& TraitData),
						 void (S::*ClientReceiveTraitRPC)(UScriptStruct*       TraitType,
						 								  const TArray<uint8>& TraitData),
						 void (S::*ServerReceiveTraitUnreliableRPC)(UScriptStruct*       TraitType,
						 											const TArray<uint8>& TraitData),
						 void (S::*ClientReceiveTraitUnreliableRPC)(UScriptStruct*       TraitType,
						 											const TArray<uint8>& TraitData))
{
	check(Subjective != nullptr);

	if (UNLIKELY(TraitType == nullptr))
	{
		return EApparatusStatus::Noop;
	}

	check(ServerReceiveTraitRPC != nullptr);
	check(ClientReceiveTraitRPC != nullptr);
	check(ServerReceiveTraitUnreliableRPC != nullptr);
	check(ClientReceiveTraitUnreliableRPC != nullptr);

	const auto Actor = Subjective->GetActor();
	AssessCondition(Paradigm, Actor != nullptr, EApparatusStatus::InvalidState);
	const auto Connection = Actor->GetNetConnection();

	const auto TraitData = Subjective->template GetTraitPtr<EParadigm::HarshInternal>(TraitType);
	AssessCondition(Paradigm, TraitData != nullptr, EApparatusStatus::Missing);

	const auto MutableSubjective = const_cast<S*>(Subjective);

	if (LIKELY(Connection != nullptr))
	{
		// There's some owning connection for the subject...

		const auto PackageMap = Connection->PackageMap;
		FNetBitWriter Writer(PackageMap, ((int64)TraitType->GetStructureSize()) * 8);
		TraitType->SerializeBin(Writer, const_cast<void* const>(TraitData));

		if (Actor->IsNetMode(NM_DedicatedServer) || Actor->IsNetMode(NM_ListenServer))
		{
			const auto PlayerController = Cast<APlayerController>(Actor);
			if (PlayerController)
			{
				// An actor is actually a player controller.
				// Push to it individually:
				if (bReliable)
				{
					(MutableSubjective->*ClientReceiveTraitRPC)(TraitType, *Writer.GetBuffer());
				}
				else
				{
					(MutableSubjective->*ClientReceiveTraitUnreliableRPC)(TraitType, *Writer.GetBuffer());
				}
				return EApparatusStatus::Success;
			}
			else
			{
				// Even though there's an owning connection,
				// if it's not a player controller,
				// broadcast the trait to all the available clients:
				return UNetworkBearerComponent::BroadcastTrait<Paradigm>(
							Actor->GetWorld(), Subjective->GetHandle().GetNetworkId(),
							TraitType, TraitData, EPeerRole::Server, bReliable);
			}
		}
		else if (Actor->IsNetMode(NM_Client))
		{
			if (bReliable)
			{
				(MutableSubjective->*ServerReceiveTraitRPC)(TraitType, *Writer.GetBuffer());
			}
			else
			{
				(MutableSubjective->*ServerReceiveTraitUnreliableRPC)(TraitType, *Writer.GetBuffer());
			}
			return EApparatusStatus::Success;
		}
	}
	else
	{
		// No owning connection for the subjective,
		// So we might actually be on the server here.
		// Try to detect the operating mode first...
		if (PeerRole == EPeerRole::Auto)
		{
			if (Actor->IsNetMode(NM_ListenServer))
			{
				PeerRole = Subjective->GetTraitmarkPermit().Contains(TraitType) ? EPeerRole::Client : EPeerRole::Server;
			}
			else if (Actor->IsNetMode(NM_DedicatedServer))
			{
				PeerRole = EPeerRole::Server;
			}
			else if (Actor->IsNetMode(NM_Standalone))
			{
				PeerRole = EPeerRole::Client;
			}
			else if (Actor->IsNetMode(NM_Client))
			{
				// A client without an owning connection makes no sense.
				PeerRole = EPeerRole::None;
			}
		}

		if (PeerRole == EPeerRole::Server)
		{
			// Broadcast the trait among all clients:
			AssessConditionFormat(
				Paradigm, Subjective->GetHandle().IsOnline(), EApparatusStatus::InvalidState,
				TEXT("The internal handle for the '%s' subjective must denote a valid online subject. "
					 "Is the subjective marked for replication?"),
				*Subjective->GetName());
			return UNetworkBearerComponent::BroadcastTrait<Paradigm>(
					Actor->GetWorld(), Subjective->GetHandle().GetNetworkId(),
					TraitType, TraitData, EPeerRole::Server, bReliable);
		}
		else if (PeerRole == EPeerRole::Client)
		{
			// We're acting as a client on a server, so can check
			// for the permission right here...
			AssessConditionFormat(Paradigm, Subjective->GetTraitmarkPermit().Contains(TraitType),
								  EApparatusStatus::NoPermission,
								  TEXT("The '%s' trait is not allowed to be set on the '%s' subjective since "
									   "it didn't pass the traitmark permit."),
								  *(TraitType->GetName()), *(Actor->GetName()));
			// There's nothing else we should do here,
			// since this version is pushing an existing trait
			// which is already in the subject.
		}
	}

	return EApparatusStatus::Noop;
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/, class S/*=void*/ >
inline TOutcome<Paradigm>
ISubjective::DoPushTrait(S* const             Subjective,
						 UScriptStruct* const TraitType,
						 const void* const    TraitData,
						 bool                 bSetForLocal,
						 EPeerRole            PeerRole,
						 const bool           bReliable,
						 void (S::*ServerReceiveTraitRPC)(UScriptStruct*       TraitType,
						 								  const TArray<uint8>& TraitData),
						 void (S::*ClientReceiveTraitRPC)(UScriptStruct*       TraitType,
						 								  const TArray<uint8>& TraitData),
						 void (S::*ServerReceiveTraitUnreliableRPC)(UScriptStruct*       TraitType,
						 											const TArray<uint8>& TraitData),
						 void (S::*ClientReceiveTraitUnreliableRPC)(UScriptStruct*       TraitType,
						 											const TArray<uint8>& TraitData))
{
	AssessCondition(Paradigm, Subjective != nullptr, EApparatusStatus::NullArgument);

	if (UNLIKELY(TraitType == nullptr))
	{
		return EApparatusStatus::Noop;
	}
	
	AssessConditionFormat(Paradigm, !bSetForLocal, EApparatusStatus::InvalidArgument,
						  TEXT("The 'bSetForLocal' argument is deprecated now. Please, set it to 'false' and set the '%s' trait explicitly."),
						  *(TraitType->GetName()))
	AssessCondition(Paradigm, TraitData != nullptr, EApparatusStatus::NullArgument);
	check(ServerReceiveTraitRPC != nullptr);
	check(ClientReceiveTraitRPC != nullptr);
	check(ServerReceiveTraitUnreliableRPC != nullptr);
	check(ClientReceiveTraitUnreliableRPC != nullptr);

	const auto Actor = Subjective->GetActor();
	AssessCondition(Paradigm, Actor != nullptr, EApparatusStatus::InvalidState);
	const auto Connection = Actor->GetNetConnection();

	if (LIKELY(Connection))
	{
		// There's some owning connection for the subject...

		const auto PackageMap = Connection->PackageMap;
		FNetBitWriter Writer(PackageMap, ((int64)TraitType->GetStructureSize()) * 8);
		TraitType->SerializeBin(Writer, const_cast<void* const>(TraitData));

		if (Actor->IsNetMode(NM_DedicatedServer) || Actor->IsNetMode(NM_ListenServer))
		{
			const auto PlayerController = Cast<APlayerController>(Actor);
			if (PlayerController)
			{
				// A subjective actor is actually a player controller.
				// Push to it individually then...
				if (bReliable)
				{
					(Subjective->*ClientReceiveTraitRPC)(TraitType, *Writer.GetBuffer());
				}
				else
				{
					(Subjective->*ClientReceiveTraitUnreliableRPC)(TraitType, *Writer.GetBuffer());
				}
				return EApparatusStatus::Success;
			}
			else
			{
				// Even though there's an owning connection,
				// if it's not a player controller,
				// broadcast the trait to all the available clients:
				return UNetworkBearerComponent::BroadcastTrait<Paradigm>(
							Actor->GetWorld(), Subjective->GetHandle().GetNetworkId(),
							TraitType, TraitData, EPeerRole::Server, bReliable);
			}
		}
		else if (Actor->IsNetMode(NM_Client))
		{
			if (bReliable)
			{
				(Subjective->*ServerReceiveTraitRPC)(TraitType, *Writer.GetBuffer());
			}
			else
			{
				(Subjective->*ServerReceiveTraitUnreliableRPC)(TraitType, *Writer.GetBuffer());
			}
			return EApparatusStatus::Success;
		}
	}
	else
	{
		// No owning connection for the subjective,
		// so this might actually be a server.
		// Try to detect the operating mode first...
		if (PeerRole == EPeerRole::Auto)
		{
			if (Actor->IsNetMode(NM_ListenServer))
			{
				PeerRole = Subjective->GetTraitmarkPermit().Contains(TraitType) ? EPeerRole::Client : EPeerRole::Server;
			}
			else if (Actor->IsNetMode(NM_DedicatedServer))
			{
				PeerRole = EPeerRole::Server;
			}
			else if (Actor->IsNetMode(NM_Standalone))
			{
				PeerRole = EPeerRole::Client;
			}
			else if (Actor->IsNetMode(NM_Client))
			{
				// A client without a connection should do nothing.
				PeerRole = EPeerRole::None;
			}
		}

		if (PeerRole == EPeerRole::Server)
		{
			// Broadcast the trait among all clients:
			AssessConditionFormat(
				Paradigm, Subjective->GetHandle().IsOnline(), EApparatusStatus::InvalidState,
				TEXT("The internal handle for the '%s' subjective must denote a valid online subject. "
					 "Is the subjective marked for replication?"),
				*Subjective->GetName());
			return UNetworkBearerComponent::BroadcastTrait<Paradigm>(
					Actor->GetWorld(), Subjective->GetHandle().GetNetworkId(),
					TraitType, TraitData, EPeerRole::Server, bReliable);
		}
		else if (PeerRole == EPeerRole::Client)
		{
			// We're acting as a client on a server, so can check
			// for the permission right here...
			AssessConditionFormat(Paradigm, Subjective->GetTraitmarkPermit().Contains(TraitType),
								  EApparatusStatus::NoPermission,
								  TEXT("The '%s' trait is not allowed to be set on the '%s' subjective since "
									   "it didn't pass the traitmark permit."),
								  *(Actor->GetName()), *(TraitType->GetName()));

			// Just set the trait like in an offline mode:
			return Subjective->GetHandle().template SetTrait<Paradigm>(TraitType, TraitData);
		}
	}

	return EApparatusStatus::Noop;
}

FORCEINLINE void
ISubjective::ClientReceiveNetworkId_Implementation(const uint32 NetworkId)
{
	GetHandle().GetInfo().ObtainNetworkState(NetworkId);
}

FORCEINLINE void
ISubjective::ServerRequestNetworkId_Implementation()
{
	auto NetworkState = GetHandle().GetInfo().ObtainNetworkState();
	AssignNetworkIdOnClient(NetworkState->ServerObtainId());
}

FORCEINLINE void
ISubjective::PeerReceiveTrait_Implementation(EPeerRole            PeerRole,
											 UScriptStruct* const TraitType,
											 const TArray<uint8>& TraitData)
{
	check(PeerRole != EPeerRole::None);
	check(TraitType != nullptr);

	const auto Actor = GetActor();
	check(Actor != nullptr);

	const auto Connection = Actor->GetNetConnection();
	const auto PackageMap = Connection ? Connection->PackageMap : nullptr;
	FStructOnScope TraitTemp(TraitType);
	FNetBitReader Reader(PackageMap,
						 const_cast<uint8*>(TraitData.GetData()),
						 ((uint64)TraitType->GetStructureSize()) * 8);
	TraitType->SerializeBin(Reader, static_cast<void*>(TraitTemp.GetStructMemory()));

	const auto Info = Handle.FindInfo();
	if (UNLIKELY(!Info))
	{
		check(PeerRole == EPeerRole::Client);
		// The subject is not yet spawned on a client.
		// Set the trait record at this moment.
		DoSetTraitRecord(TraitType, static_cast<const void*>(TraitTemp.GetStructMemory()));
		return;
	}

	check(Info->IsOnline() || PeerRole == EPeerRole::Client);

	Info->SetTrait(TraitType, static_cast<const void*>(TraitTemp.GetStructMemory()));
}

FORCEINLINE bool
ISubjective::PeerReceiveTrait_Validate(EPeerRole            PeerRole,
									   UScriptStruct* const TraitType,
									   const TArray<uint8>& TraitData)
{
	check(PeerRole != EPeerRole::None);

	if (UNLIKELY(TraitType == nullptr))
	{
		Report<EApparatusStatus::NoPermission>(
			TEXT("A trait's type must be provided to receive a trait for the '{0}' subjective."),
			{CastChecked<UObject>(this)->GetName()});
		return false;
	}

	const auto Info = Handle.FindInfo();
	if (UNLIKELY(!Info || !Info->IsOnline()))
	{
		return ensureAlwaysMsgf(PeerRole == EPeerRole::Client, 
								TEXT("Only the client '%s' subjective can receive the '%s' trait when its subject state is incomplete or offline."),
								*CastChecked<UObject>(this)->GetName(), *TraitType->GetName());
	}

	const auto NetworkState = Info->ObtainNetworkState();
	if (UNLIKELY((PeerRole == EPeerRole::Server) && !NetworkState->TraitmarkPermit.Contains(TraitType)))
	{
		Report<EApparatusStatus::NoPermission>(
			TEXT("The '{0}' trait is not allowed to be pushed to the server's version of the '{1}' subjective. "
				 "Have you set up the correct traitmark permit? Is server trying to be compromised by the client?"),
			{TraitType->GetName(), CastChecked<UObject>(this)->GetName()});
		return false;
	}

	if (UNLIKELY(TraitType->GetPropertiesSize() != TraitData.Num()))
	{
		Report<EApparatusStatus::NoPermission>(
			TEXT("The size of the '{0}' trait pushed to the '{1}' subjective doesn't match the peer's version. "
				 "Does the client's version match the server's? Is server trying to be compromised by the client?"),
			{TraitType->GetName(), CastChecked<UObject>(this)->GetName()});
		return false;
	}

	return ensureAlwaysMsgf(GetActor() != nullptr,
							TEXT("The actor context must be available on the '%s' subjective in order to receive the '%s' trait."),
							*CastChecked<UObject>(this)->GetName(), *TraitType->GetName());
}
