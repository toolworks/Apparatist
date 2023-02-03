/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectiveActorComponent.h
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
#include "Components/ActorComponent.h"
#include "Engine/MemberReference.h"

#include "Machine.h"

#include "SubjectiveActorComponent.generated.h"


/**
 * An actor component which is a subject containing the details.
 */
UCLASS(ClassGroup = "Apparatus", Meta = (BlueprintSpawnableComponent))
class APPARATUSRUNTIME_API USubjectiveActorComponent
  : public UActorComponent
  , public ISubjective
{
	GENERATED_BODY()

  private:

#pragma region Standard Subjective Block

	/**
	 * The list of traits.
	 */
	UPROPERTY(EditAnywhere, Category = Data,
			  Meta = (DisplayAfter = "Flagmark",
					  AllowPrivateAccess = "true"))
	TArray<FTraitRecord> Traits;

	/**
	 * The list of details.
	 */
	UPROPERTY(EditAnywhere, Instanced, Category = Data,
			  Meta = (DisplayAfter = "Traits",
			  		  AllowPrivateAccess = "true"))
	TArray<UDetail*> Details;

	/**
	 * An optional preferred belt for the subject to be placed in.
	 */
	UPROPERTY(EditAnywhere, Category = Optimization,
			  Meta = (AllowPrivateAccess = "true"))
	UBelt* PreferredBelt = nullptr;

	/**
	 * The flagmark of the subjective.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data,
			  Meta = (Bitmask, BitmaskEnum = "EFlagmark",
					  DisplayAfter = "PreferredBelt",
					  AllowPrivateAccess = "true"))
	int32 Flagmark = FM_None;

	/**
	 * The mechanism to use as a default one,
	 * when registering the subjective.
	 * 
	 * If not set, the default mechanism
	 * of the world will be used.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Context,
			  Meta = (AllowPrivateAccess = "true"))
	AMechanism* MechanismOverride = nullptr;

  protected:

	/**
	 * The traits allowed to be received on the server.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Networking,
			  Meta = (DisplayAfter = "Details",
					  AllowPrivateAccess = "true"))
	FTraitmark TraitmarkPermit;

	UPROPERTY(Replicated)
	uint32 SubjectNetworkId = FSubjectNetworkState::InvalidId;

	END_STANDARD_NETWORKED_SUBJECTIVE_PROPERTY_BLOCK(USubjectiveActorComponent)

#pragma endregion Standard Subjective Block

  public:

	// Sets default values for this component's properties
	USubjectiveActorComponent();

  protected:

	void
	GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	FORCEINLINE bool
	ShouldBeReplicated() const override
	{
		return GetIsReplicated();
	}

	UFUNCTION(Client, Reliable)
	void
	ClientReceiveNetworkId(const uint32 NetworkId);

	FORCEINLINE void
	ClientReceiveNetworkId_Implementation(const uint32 NetworkId)
	{
		ISubjective::ClientReceiveNetworkId_Implementation(NetworkId);
	}

	UFUNCTION(Server, Reliable)
	void
	ServerRequestNetworkId();

	FORCEINLINE void
	ServerRequestNetworkId_Implementation()
	{
		ISubjective::ServerRequestNetworkId_Implementation();
	}

#pragma region Reliable

	UFUNCTION(Server, Reliable, WithValidation)
	void
	ServerReceiveTrait(UScriptStruct*       TraitType,
					   const TArray<uint8>& TraitData);

	FORCEINLINE void
	ServerReceiveTrait_Implementation(UScriptStruct*       TraitType,
									  const TArray<uint8>& TraitData)
	{
		ISubjective::PeerReceiveTrait_Implementation(EPeerRole::Server, TraitType, TraitData);
	}

	FORCEINLINE bool
	ServerReceiveTrait_Validate(UScriptStruct*       TraitType,
								const TArray<uint8>& TraitData)
	{
		return ISubjective::PeerReceiveTrait_Validate(EPeerRole::Server, TraitType, TraitData);
	}

	UFUNCTION(Client, Reliable, WithValidation)
	void
	ClientReceiveTrait(UScriptStruct*       TraitType,
					   const TArray<uint8>& TraitData);

	FORCEINLINE void
	ClientReceiveTrait_Implementation(UScriptStruct*       TraitType,
									  const TArray<uint8>& TraitData)
	{
		ISubjective::PeerReceiveTrait_Implementation(EPeerRole::Client, TraitType, TraitData);
	}

	FORCEINLINE bool
	ClientReceiveTrait_Validate(UScriptStruct*       TraitType,
								const TArray<uint8>& TraitData)
	{
		return ISubjective::PeerReceiveTrait_Validate(EPeerRole::Client, TraitType, TraitData);
	}

#pragma endregion Reliable

#pragma region Unreliable

	UFUNCTION(Server, Unreliable, WithValidation)
	void
	ServerReceiveTraitUnreliable(UScriptStruct*       TraitType,
								 const TArray<uint8>& TraitData);

	FORCEINLINE void
	ServerReceiveTraitUnreliable_Implementation(UScriptStruct*       TraitType,
												const TArray<uint8>& TraitData)
	{
		ISubjective::PeerReceiveTrait_Implementation(EPeerRole::Server, TraitType, TraitData);
	}

	FORCEINLINE bool
	ServerReceiveTraitUnreliable_Validate(UScriptStruct*       TraitType,
										  const TArray<uint8>& TraitData)
	{
		return ISubjective::PeerReceiveTrait_Validate(EPeerRole::Server, TraitType, TraitData);
	}

	UFUNCTION(Client, Unreliable, WithValidation)
	void
	ClientReceiveTraitUnreliable(UScriptStruct*       TraitType,
								 const TArray<uint8>& TraitData);

	FORCEINLINE void
	ClientReceiveTraitUnreliable_Implementation(UScriptStruct*       TraitType,
												const TArray<uint8>& TraitData)
	{
		ISubjective::PeerReceiveTrait_Implementation(EPeerRole::Client, TraitType, TraitData);
	}

	FORCEINLINE bool
	ClientReceiveTraitUnreliable_Validate(UScriptStruct*       TraitType,
										  const TArray<uint8>& TraitData)
	{
		return ISubjective::PeerReceiveTrait_Validate(EPeerRole::Client, TraitType, TraitData);
	}

#pragma endregion Unreliable

	FORCEINLINE void
	AssignNetworkIdOnClient(const uint32 NetworkId) override
	{
		SubjectNetworkId = NetworkId;
		auto* const Owner = GetOwner();
		if (LIKELY(Owner && Owner->GetNetConnection()))
		{
			// Call the RPC on the client.
			ClientReceiveNetworkId(NetworkId);
		}
	}

	FORCEINLINE void
	ObtainNetworkIdFromServer() override
	{
		if (SubjectNetworkId >= FSubjectNetworkState::FirstId)
		{
			// Seems like already obtained via usual replication...
			return;
		}

		auto* const Owner = GetOwner();
		if (UNLIKELY(Owner == nullptr))
		{
			return;
		}

		if (Owner->IsNetMode(NM_DedicatedServer) || Owner->IsNetMode(NM_ListenServer))
		{
			check(Handle.IsValid());
			SubjectNetworkId = Handle.GetInfo().ObtainNetworkState()->ServerObtainId();
			return;
		}

		if (LIKELY(Owner->GetNetConnection() && Owner->IsNetMode(NM_Client)))
		{
			ServerRequestNetworkId();
		}
		else
		{
			UE_LOG(LogApparatus, Error,
				   TEXT("Impossible to get a network identifier for the '%s' subjective."),
				   *GetName());
		}
	}

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FORCEINLINE void
	Serialize(FArchive& Archive) override
	{
		ISubjective::DoStartSerialization(Archive);
		Super::Serialize(Archive);
		ISubjective::DoFinishSerialization(Archive);
	}

	FORCEINLINE TPortableOutcome<>
	DoPushTrait(const EParadigm      Paradigm,
				UScriptStruct* const TraitType,
				const EPeerRole      PeerRole = EPeerRole::Auto,
				const bool           bReliable = true) const override
	{
		if (IsHarsh(Paradigm))
		{
			return (TPortableOutcome<>)ISubjective::template DoPushTrait<EParadigm::HarshSafe>(
				this, TraitType,
				PeerRole,
				bReliable,
				&USubjectiveActorComponent::ServerReceiveTrait,
				&USubjectiveActorComponent::ClientReceiveTrait,
				&USubjectiveActorComponent::ServerReceiveTraitUnreliable,
				&USubjectiveActorComponent::ClientReceiveTraitUnreliable);
		}
		else
		{
			return ISubjective::template DoPushTrait<EParadigm::PoliteSafe>(
				this, TraitType,
				PeerRole,
				bReliable,
				&USubjectiveActorComponent::ServerReceiveTrait,
				&USubjectiveActorComponent::ClientReceiveTrait,
				&USubjectiveActorComponent::ServerReceiveTraitUnreliable,
				&USubjectiveActorComponent::ClientReceiveTraitUnreliable);
		}
	}

	FORCEINLINE TPortableOutcome<>
	DoPushTrait(const EParadigm      Paradigm,
				UScriptStruct* const TraitType,
				const void* const    TraitData,
				const bool           bSetForLocal = false,
				const EPeerRole      PeerRole = EPeerRole::Auto,
				const bool           bReliable = true) override
	{
		if (IsHarsh(Paradigm))
		{
			return (TPortableOutcome<>)ISubjective::template DoPushTrait<EParadigm::HarshSafe>(
				this, TraitType, TraitData,
				bSetForLocal, PeerRole,
				bReliable,
				&USubjectiveActorComponent::ServerReceiveTrait,
				&USubjectiveActorComponent::ClientReceiveTrait,
				&USubjectiveActorComponent::ServerReceiveTraitUnreliable,
				&USubjectiveActorComponent::ClientReceiveTraitUnreliable);
		}
		else
		{
			return ISubjective::template DoPushTrait<EParadigm::PoliteSafe>(
				this, TraitType, TraitData,
				bSetForLocal, PeerRole,
				bReliable,
				&USubjectiveActorComponent::ServerReceiveTrait,
				&USubjectiveActorComponent::ClientReceiveTrait,
				&USubjectiveActorComponent::ServerReceiveTraitUnreliable,
				&USubjectiveActorComponent::ClientReceiveTraitUnreliable);
		}
	}

  public:

	FORCEINLINE AActor*
	GetActor() const override
	{
		return GetOwner();
	}

	FORCEINLINE void
	NotifyHandleDespawned() override
	{
		ISubjective::NotifyHandleDespawned();
		auto* Actor = GetOwner();
		if (LIKELY(Actor))
		{
			Actor->Destroy();
		}
		else
		{
			DestroyComponent();
		}
	}



}; //-class USubjectiveActorComponent

FORCEINLINE
USubjectiveActorComponent::USubjectiveActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FORCEINLINE void
USubjectiveActorComponent::BeginPlay()
{
	Super::BeginPlay();

	Belt      = nullptr;
	SlotIndex = InvalidSlotIndex;

	DoRegister();
}

FORCEINLINE void
USubjectiveActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EnsureOK(DoUnregister<EParadigm::DefaultPortable>());

	check(Belt == nullptr);
	check(SlotIndex == InvalidSlotIndex);

	Super::EndPlay(EndPlayReason);
}
