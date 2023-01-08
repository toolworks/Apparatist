/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: ApparatusCustomVersion.h
 * Created: 2021-07-14 10:41:26
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
/**
 * @file 
 * @brief Apparatus serialization versioning.
 */

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

/**
 * Custom serialization version for assets/classes
 * in the ApparatusRuntime module.
 */
struct APPARATUSRUNTIME_API FApparatusCustomVersion
{
	enum Type
	{
		/// Before any version changes were made in the plugin
		Initial = 0,
		
		/// The first Apparatus version with networking support.
		Online,

		/// Thread-safe flagmarks introduced globally.
		AtomicFlagmarks,

		/// The newer (and far more versatile) trait record functionality.
		TraitRecordOverhaul,

		// New versions here...

		// End mark:
		VersionPlusOne,

		// The latest version so far:
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

  private:

	FApparatusCustomVersion() {}
};
