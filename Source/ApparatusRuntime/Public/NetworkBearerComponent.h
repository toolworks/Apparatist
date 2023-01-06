/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: NetworkBearerComponent.h
 * Created: 2021-10-16 19:55:40
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

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/MemberReference.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define NETWORK_BEARER_COMPONENT_H_SKIPPED_MACHINE_H
#endif

#include "SolidSubjectHandle.h"

#ifdef NETWORK_BEARER_COMPONENT_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "NetworkBearerComponent.generated.h"


// Forward declarations:
class AMechanism;
class UMachine;
class ISubjective;

/**
 * An actor component for handling the networking logic in a unified way.
 * 
 * This manages the main synchronization functionality
 * for the subjective-less subjects in an Unreal-compatible,
 * transparent way.
 * 
 * You can use the methods of this component directly to fine-grain
 * the traffic.
 */
UCLASS(ClassGroup = "Apparatus|Networking", Meta = (BlueprintSpawnableComponent))
class APPARATUSRUNTIME_API UNetworkBearerComponent
  : public UActorComponent
{
	GENERATED_BODY()

  private:

	friend struct FSubjectInfo;
	friend class AMechanism;
	friend class UMachine;
	friend class ISubjective;

  protected:

#pragma region Subject

	UFUNCTION(Client, Reliable)
	void
	ClientMechanismReceiveSubject(const uint32 InSubjectNetworkId);

	/**
	 * Receive a subject on a client.
	 * RPC implementation.
	 */
	void
	ClientMechanismReceiveSubject_Implementation(const uint32 InSubjectNetworkId);

#pragma endregion Subject

#pragma region Trait 

	/**
	 * Receive a subject trait on a peer.
	 * Generic RPC implementation.
	 */
	EApparatusStatus
	PeerMechanismReceiveSubjectTrait_Implementation(
		const EPeerRole      PeerRole,
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData);

	/**
	 * Validate the RPC call.
	 * Generic version.
	 */
	bool
	PeerMechanismReceiveSubjectTrait_Validate(
		const EPeerRole      PeerRole,
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData);

	UFUNCTION(Server, Reliable, WithValidation)
	void
	ServerMechanismReceiveSubjectTrait(
		const uint32         SubjectNetworkId,
		UScriptStruct*       TraitType,
		const TArray<uint8>& TraitData);

	bool
	ServerMechanismReceiveSubjectTrait_Validate(
		const uint32         SubjectNetworkId,
		UScriptStruct*       TraitType,
		const TArray<uint8>& TraitData);

	/**
	 * Receive a subject trait on a server.
	 * RPC implementation.
	 */
	void
	ServerMechanismReceiveSubjectTrait_Implementation(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData);

	UFUNCTION(Server, Unreliable, WithValidation)
	void
	ServerMechanismReceiveSubjectTraitUnreliable(
		const uint32         SubjectNetworkId,
		UScriptStruct*       TraitType,
		const TArray<uint8>& TraitData);

	bool
	ServerMechanismReceiveSubjectTraitUnreliable_Validate(
		const uint32         SubjectNetworkId,
		UScriptStruct*       TraitType,
		const TArray<uint8>& TraitData);

	/**
	 * Receive a subject trait on a server.
	 * RPC implementation.
	 */
	void
	ServerMechanismReceiveSubjectTraitUnreliable_Implementation(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData);

	UFUNCTION(Client, Reliable, WithValidation)
	void
	ClientMechanismReceiveSubjectTrait(
		const uint32         SubjectNetworkId,
		UScriptStruct*       TraitType,
		const TArray<uint8>& TraitData);

	bool
	ClientMechanismReceiveSubjectTrait_Validate(
		const uint32         SubjectNetworkId,
		UScriptStruct*       TraitType,
		const TArray<uint8>& TraitData);

	UFUNCTION(Client, Unreliable, WithValidation)
	void
	ClientMechanismReceiveSubjectTraitUnreliable(
		const uint32         SubjectNetworkId,
		UScriptStruct*       TraitType,
		const TArray<uint8>& TraitData);

	bool
	ClientMechanismReceiveSubjectTraitUnreliable_Validate(
		const uint32         SubjectNetworkId,
		UScriptStruct*       TraitType,
		const TArray<uint8>& TraitData);

	/**
	 * Receive a subject trait on a client.
	 * RPC implementation.
	 */
	void
	ClientMechanismReceiveSubjectTrait_Implementation(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const TArray<uint8>& TraitData);

#pragma endregion Trait

#pragma region Traits

	UFUNCTION(Client, Reliable)
	void
	ClientMechanismReceiveSubjectWithTraits(
		const uint32                  InSubjectNetworkId,
		const TArray<UScriptStruct*>& TraitsTypes,
		const TArray<uint8>&          TraitsData);

	/**
	 * Receive a subject along with its traits on a client.
	 * RPC implementation.
	 */
	void
	ClientMechanismReceiveSubjectWithTraits_Implementation(
		const uint32                  InSubjectNetworkId,
		const TArray<UScriptStruct*>& TraitsTypes,
		const TArray<uint8>&          TraitsData);

#pragma endregion Traits

#pragma region Broadcasting

	/**
	 * Broadcast a subject trait among all of
	 * the available peers through their bearers.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	static TOutcome<Paradigm>
	BroadcastTrait(UWorld* const        World,
				   const uint32         SubjectNetworkId,
				   UScriptStruct* const TraitType,
				   const void* const    TraitData,
				   EPeerRole            PeerRole = EPeerRole::Auto,
				   const bool           bReliable = true);

#pragma endregion Broadcasting

	/**
	 * Send a subject's network identifier to the client's mechanism.
	 */
	void
	PushSubjectToClient(const uint32 InSubjectNetworkId);

	/**
	 * Send a subject to the client's mechanism.
	 */
	void
	PushSubjectToClient(const FSolidSubjectHandle& Subject);

	/**
	 * Send a subject to the client's mechanism.
	 */
	void
	PushSubjectToClient(const uint32          NetworkId,
						const FSubjectRecord& SubjectRecord);

	/**
	 * Push an additional subject trait to its remote version.
	 * Internal low-level method.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectNetworkId The unique network identifier of the subject to push.
	 * @param TraitType The type of the trait to push.
	 * Must not be a @c nullptr.
	 * @param TraitData The data of the trait to push.
	 * Must not e a @c nullptr.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used for transaction.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DoPushSubjectTrait(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const void* const    TraitData,
		EPeerRole            PeerRole = EPeerRole::Auto,
		const bool           bReliable = true);

	/**
	 * Push an additional subject trait to its remote version.
	 * Internal low-level method.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectNetworkId The unique network identifier of the subject to push.
	 * @param TraitType The type of the trait to push.
	 * Must not be a @c nullptr.
	 * @param TraitData The data of the trait to push.
	 * Must not e a @c nullptr.
	 * @param bReliable Should reliable channel be used for transaction.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE auto
	DoPushSubjectTrait(
		const uint32         SubjectNetworkId,
		UScriptStruct* const TraitType,
		const void* const    TraitData,
		const bool           bReliable,
		EPeerRole            PeerRole = EPeerRole::Auto)
	{
		return DoPushSubjectTrait<Paradigm>(SubjectNetworkId, TraitType, TraitData, PeerRole, bReliable);
	}

	/**
	 * Push an additional subject trait to its remote version.
	 * Internal templated low-level method.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * @param SubjectNetworkId The unique network identifier of the subject to push.
	 * @param Trait The trait to push.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used for transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal, typename T = void >
	FORCEINLINE auto
	DoPushSubjectTrait(
		const uint32    SubjectNetworkId,
		const T&        Trait,
		const EPeerRole PeerRole = EPeerRole::Auto,
		const bool      bReliable = true)
	{
		return DoPushSubjectTrait<Paradigm>(SubjectNetworkId,
											T::StaticStruct(),
											static_cast<const void*>(&Trait),
											PeerRole, bReliable);
	}

	/**
	 * Push an additional subject trait to its remote version.
	 * Internal templated low-level method.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * @param SubjectNetworkId The unique network identifier of the subject to push.
	 * @param Trait The trait to push.
	 * @param bReliable Should reliable channel be used for transaction.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal, typename T = void >
	FORCEINLINE auto
	DoPushSubjectTrait(
		const uint32    SubjectNetworkId,
		const T&        Trait,
		const bool      bReliable,
		const EPeerRole PeerRole = EPeerRole::Auto)
	{
		return DoPushSubjectTrait<Paradigm>(SubjectNetworkId, Trait,
											static_cast<const void*>(&Trait),
											bReliable, PeerRole);
	}

  public:

	/**
	 * Push an additional trait to a remote version of the subject.
	 * 
	 * The subject will be brought online automatically if needed.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] SubjectHandle The handle of the subject to push to.
	 * Must be a valid handle.
	 * @param[in] TraitType The type of the trait to push.
	 * Must not be a @c nullptr.
	 * @param[in] TraitData The data of the trait to push.
	 * Must not e a @c nullptr.
	 * @param[in] bReliable Should reliable channel be used for transaction.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	PushSubjectTrait(
		const FSolidSubjectHandle& SubjectHandle,
		UScriptStruct* const       TraitType,
		const void* const          TraitData,
		const bool                 bReliable = true)
	{
		const auto Outcome = SubjectHandle.template BringOnline<Paradigm>();
		if (OK(Outcome))
		{
			return DoPushSubjectTrait<Paradigm>(SubjectHandle.GetNetworkId(),
												TraitType, TraitData,
												bReliable);
		}
		return Outcome;
	}

	/**
	 * Push an additional subject trait to its remote version.
	 * Templated version.
	 * 
	 * The subject will be brought online automatically if needed.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * Must be a trait type.
	 * @param[in] SubjectHandle The handle of the subject to push to.
	 * Must be a valid handle to a subject.
	 * @param[in] Trait The trait to push.
	 * @param[in] bReliable Should reliable channel be used for transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	PushSubjectTrait(
		const FSolidSubjectHandle SubjectHandle,
		const T&                  Trait,
		const bool                bReliable = true)
	{
		return PushSubjectTrait<Paradigm>(SubjectHandle,
										  T::StaticStruct(),
										  static_cast<const void*>(&Trait),
										  bReliable);
	}

	/**
	 * Push an existing subject trait to its remote version.
	 * Dynamically typed version.
	 * 
	 * @warning The subject won't be brought online automatically,
	 * because the method works on a constant subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] SubjectHandle The handle of the subject to push to.
	 * Must be a valid handle to a subject.
	 * @param[in] TraitType The type of the trait to push.
	 * Must not be a @c nullptr.
	 * @param[in] bReliable Should reliable channel be used for transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	PushSubjectTrait(const FConstSubjectHandle& SubjectHandle,
					 UScriptStruct* const       TraitType,
					 const bool                 bReliable = true)
	{
		const auto TraitPtr = SubjectHandle.template GetTraitPtr<Paradigm>(TraitType);
		if (AvoidError(Paradigm, TraitPtr))
		{
			return TraitPtr;
		}
		return DoPushSubjectTrait<Paradigm>(SubjectHandle.GetNetworkId(),
											TraitType,
											TraitPtr,
											bReliable);
	}

	/**
	 * Push an existing subject trait to its remote version.
	 * Statically typed version.
	 * 
	 * @warning The subject won't be brought online automatically,
	 * cause the method works on a constant subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * @param[in] SubjectHandle The handle of the subject to push to.
	 * Must be a valid handle to a subject.
	 * @param[in] bReliable Should reliable channel be used for transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE auto
	PushSubjectTrait(const FConstSubjectHandle& SubjectHandle,
					 const bool                 bReliable = true)
	{
		return PushSubjectTrait(SubjectHandle,
								T::StaticStruct(),
								bReliable);
	}

	/**
	 * Push an existing subject trait to its remote version.
	 * Statically typed default paradigm version.
	 * 
	 * @warning The subject won't be brought online automatically,
	 * cause the method works on a constant subject.
	 * 
	 * @tparam T The type of the trait to push.
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] SubjectHandle The handle of the subject to push to.
	 * Must be a valid handle to a subject.
	 * @param[in] bReliable Should reliable channel be used for transaction.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	PushSubjectTrait(const FConstSubjectHandle& SubjectHandle,
					 const bool                 bReliable = true)
	{
		return PushSubjectTrait<Paradigm, T>(SubjectHandle, bReliable);
	}

	/**
	 * Get the mechanism the bearer belongs to.
	 */
	AMechanism*
	GetMechanism() const;

	/**
	 * Construct a default network bearer instance.
	 */
	UNetworkBearerComponent();

}; //-class UNetworkBearerComponent

FORCEINLINE
UNetworkBearerComponent::UNetworkBearerComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
