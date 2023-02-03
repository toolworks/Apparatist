/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusCustomVersion.cpp
 * Created: 2021-07-14 10:47:40
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

#include "ApparatusCustomVersion.h"

#include "Serialization/CustomVersion.h"


const FGuid FApparatusCustomVersion::GUID(0x5E1714CD, 0x484E2951, 0x707A89A7, 0x9302AB78);

FCustomVersionRegistration GRegisterApparatusCustomVersion(
	FApparatusCustomVersion::GUID,
	FApparatusCustomVersion::LatestVersion,
	TEXT("ApparatusVer"));
