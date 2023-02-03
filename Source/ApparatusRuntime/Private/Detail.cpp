/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Detail.cpp
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

#include "Detail.h"

#include "Machine.h"
#include "SubjectiveActorComponent.h"


void UDetail::SetEnabled(const bool bState)
{
	// Check if anything should be changed at all:
	if (bEnabled == bState)
	{
		return;
	}

	bEnabled = bState;

	ISubjective* const Owner = GetOwner();
	if (LIKELY(Owner && Owner->IsRegistered()))
	{
		FFingerprint& Fingerprint = Owner->GetFingerprintRef();
		auto* const Mechanism = GetMechanism();
		if (bState)
		{
			Fingerprint.Add(this->GetClass());

			/// Process the change deeply within the mechanism...
			if (LIKELY(Mechanism))
			{
				Mechanism->ActualizeBelt(Owner);
			}
		}
		else
		{
			// The detail has to be silenced.
			// Do not alter the fingerprint (or trigger signaling),
			// if there is some other detail of a compatible type
			// still present in the subjective...
			bool bHasOther = false;
			for (UDetail* Detail : Owner->GetDetailsRef())
			{
				if (UNLIKELY(Detail == nullptr)) continue;
				if (UNLIKELY(Detail == this)) continue;
				if (!Detail->IsEnabled()) continue;
				if (!Detail->IsA(this->GetClass())) continue;
				bHasOther = true;
				break;
			}

			if (!bHasOther)
			{
				// No other compatible detail was found.
				// The fingerprint must be changed:
				Fingerprint.Remove(this->GetClass());

				// Process the change deeply within the mechanism...
				if (LIKELY(Mechanism))
				{
					Mechanism->ActualizeBelt(Owner);
				}
			}
		}

		// Call the events now.
		if (bState)
		{
			Activated();
		}
		else
		{
			Deactivated();
		}
	}
}

TScriptInterface<ISubjective>
UDetail::BP_GetOwner() const
{
	return TScriptInterface<ISubjective>(Cast<UObject>(GetOwner()));
}
