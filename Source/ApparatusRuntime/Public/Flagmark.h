/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Flagmark.h
 * Created: 2021-10-25 17:37:30
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

#include <More/type_traits>

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "More/EnumClassFlags.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define FLAGMARK_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"

#ifdef FLAGMARK_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Flagmark.generated.h"


/**
 * Flagmark bit identifiers.
 */
UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFlagmarkBit : uint8
{
	/**
	 * Subject (re)moved from the chunk.
	 */
	Stale = 0x0 UMETA(Hidden),

	/**
	 * An initialized subject.
	 */
	Booted = 0x1,

	/**
	 * Networked subject.
	 */
	Online = 0x2,

	/**
	 * Editor-spawned subject.
	 */
	Editor = 0x3 UMETA(Hidden),

	/**
	 * Participating in a deferred entity removal.
	 */
	DeferredDespawn = 0x4 UMETA(Hidden),

	/**
	 * The maximum system-level bit.
	 */
	SystemLevelMax = DeferredDespawn UMETA(Hidden),

	/**
	 * User flag A.
	 */
	A = 0x5,

	/**
	 * The first user-level flag available (A).
	 * 
	 */
	FirstUserLevel = A UMETA(Hidden),

	/**
	 * User flag B.
	 */
	B,

	/**
	 * User flag C.
	 */
	C,

	/**
	 * User flag D.
	 */
	D,

	/**
	 * User flag E.
	 */
	E,

	/**
	 * User flag F.
	 */
	F,

	/**
	 * User flag G.
	 */
	G,

	/**
	 * User flag H.
	 */
	H,

	/**
	 * User flag I.
	 */
	I,

	/**
	 * User flag J.
	 */
	J,

	/**
	 * User flag K.
	 */
	K,

	/**
	 * User flag L.
	 */
	L,

	/**
	 * User flag M.
	 */
	M,

	/**
	 * User flag N.
	 */
	N,

	/**
	 * User flag O.
	 */
	O,

	/**
	 * User flag P.
	 */
	P,

	/**
	 * User flag Q.
	 */
	Q,

	/**
	 * User flag R.
	 */
	R,

	/**
	 * User flag S.
	 */
	S,

	/**
	 * User flag T.
	 */
	T,

	/**
	 * User flag U.
	 */
	U,

	/**
	 * User flag V.
	 */
	V,

	/**
	 * User flag W.
	 */
	W,

	/**
	 * User flag X.
	 */
	X,

	/**
	 * User flag Y.
	 */
	Y,

	/**
	 * User flag Z.
	 */
	Z,

	/**
	 * The maximum flag available.
	 * Seems like sign-bit flag is not supported.
	 */
	Max = Z UMETA(Hidden),

}; // enum class EFlagmarkBit

/**
 * Get the boot flagmark bit class.
 */
inline APPARATUSRUNTIME_API UEnum*
GetFlagmarkBitClass()
{
	static UEnum* EnumType = FindObject<UEnum>(nullptr, TEXT("/Script/ApparatusRuntime.EFlagmarkBit"), true);
	check(EnumType);
	return EnumType;
}

/**
 * Convert a flagmark bit enum variable to a string.
 */
FORCEINLINE FString
ToString(const EFlagmarkBit FlagmarkBit)
{
	const UEnum* const EnumPtr = GetFlagmarkBitClass();
	if (!EnumPtr)
	{
		return FString("<Invalid>");
	}

	return EnumPtr->GetNameStringByValue((int64)FlagmarkBit);
}

/**
 * Check if the flag is a system one
 * and not allowed to be set by the user.
 * 
 * @param Flag The flag to examine.
 * @return The state of examination.
 */
constexpr FORCEINLINE bool
IsSystemLevel(const EFlagmarkBit Flag)
{
	return Flag <= EFlagmarkBit::SystemLevelMax;
}

/**
 * Check if the flag is a user one
 * and is allowed to be set by the user.
 * 
 * @param Flag The flag to examine.
 * @return The state of examination.
 */
constexpr FORCEINLINE bool
IsUserLevel(const EFlagmarkBit Flag)
{
	return Flag >= EFlagmarkBit::FirstUserLevel;
}

/**
 * The universal flagmark functionality.
 * 
 * The flagmarks are checked during the iterating procedure,
 * but the checks themselves are pretty trivial.
 */
UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EFlagmark
{
	/**
	 * No flags set.
	 */
	FM_None = 0x0 UMETA(DisplayName = "None", Hidden),

	/**
	 * (Re)moved subject.
	 */
	FM_Stale = 1 << (int)EFlagmarkBit::Stale UMETA(Hidden),

	/**
	 * An initialized subject.
	 */
	FM_Booted = 1 << (int)EFlagmarkBit::Booted UMETA(DisplayName = "Booted"),

	/**
	 * Networked subject.
	 */
	FM_Online = 1 << (int)EFlagmarkBit::Online UMETA(DisplayName = "Online"),

	/**
	 * Editor-spawned subject.
	 */
	FM_Editor = 1 << (int)EFlagmarkBit::Editor UMETA(Hidden),

	/**
	 * Participating in a deferred removal process.
	 */
	FM_DeferredDespawn = 1 << (int)EFlagmarkBit::DeferredDespawn UMETA(Hidden),

	/**
	 * A mask with all the system flags set.
	 */
	FM_AllSystemLevel = FM_Stale | FM_Booted | FM_Online | FM_Editor | FM_DeferredDespawn UMETA(Hidden),

	/**
	 * User flag A.
	 */
	FM_A = 1 << (int)EFlagmarkBit::A UMETA(DisplayName = "A"),

	/**
	 * User flag B.
	 */
	FM_B = 1 << (int)EFlagmarkBit::B UMETA(DisplayName = "B"),

	/**
	 * User flag C.
	 */
	FM_C = 1 << (int)EFlagmarkBit::C UMETA(DisplayName = "C"),

	/**
	 * User flag D.
	 */
	FM_D = 1 << (int)EFlagmarkBit::D UMETA(DisplayName = "D"),

	/**
	 * User flag E.
	 */
	FM_E = 1 << (int)EFlagmarkBit::E UMETA(DisplayName = "E"),

	/**
	 * User flag F.
	 */
	FM_F = 1 << (int)EFlagmarkBit::F UMETA(DisplayName = "F"),

	/**
	 * User flag G.
	 */
	FM_G = 1 << (int)EFlagmarkBit::G UMETA(DisplayName = "G"),

	/**
	 * User flag H.
	 */
	FM_H = 1 << (int)EFlagmarkBit::H UMETA(DisplayName = "H"),

	/**
	 * User flag I.
	 */
	FM_I = 1 << (int)EFlagmarkBit::I UMETA(DisplayName = "I"),

	/**
	 * User flag J.
	 */
	FM_J = 1 << (int)EFlagmarkBit::J UMETA(DisplayName = "J"),

	/**
	 * User flag K.
	 */
	FM_K = 1 << (int)EFlagmarkBit::K UMETA(DisplayName = "K"),

	/**
	 * User flag L.
	 */
	FM_L = 1 << (int)EFlagmarkBit::L UMETA(DisplayName = "L"),

	/**
	 * User flag M.
	 */
	FM_M = 1 << (int)EFlagmarkBit::M UMETA(DisplayName = "M"),

	/**
	 * User flag N.
	 */
	FM_N = 1 << (int)EFlagmarkBit::N UMETA(DisplayName = "N"),

	/**
	 * User flag O.
	 */
	FM_O = 1 << (int)EFlagmarkBit::O UMETA(DisplayName = "O"),

	/**
	 * User flag P.
	 */
	FM_P = 1 << (int)EFlagmarkBit::P UMETA(DisplayName = "P"),

	/**
	 * User flag Q.
	 */
	FM_Q = 1 << (int)EFlagmarkBit::Q UMETA(DisplayName = "Q"),

	/**
	 * User flag R.
	 */
	FM_R = 1 << (int)EFlagmarkBit::R UMETA(DisplayName = "R"),

	/**
	 * User flag S.
	 */
	FM_S = 1 << (int)EFlagmarkBit::S UMETA(DisplayName = "S"),

	/**
	 * User flag T.
	 */
	FM_T = 1 << (int)EFlagmarkBit::T UMETA(DisplayName = "T"),

	/**
	 * User flag U.
	 */
	FM_U = 1 << (int)EFlagmarkBit::U UMETA(DisplayName = "U"),

	/**
	 * User flag V.
	 */
	FM_V = 1 << (int)EFlagmarkBit::V UMETA(DisplayName = "V"),

	/**
	 * User flag W.
	 */
	FM_W = 1 << (int)EFlagmarkBit::W UMETA(DisplayName = "W"),

	/**
	 * User flag X.
	 */
	FM_X = 1 << (int)EFlagmarkBit::X UMETA(DisplayName = "X"),

	/**
	 * User flag Y.
	 */
	FM_Y = 1 << (int)EFlagmarkBit::Y UMETA(DisplayName = "Y"),

	/**
	 * User flag Z.
	 */
	FM_Z = 1 << (int)EFlagmarkBit::Z UMETA(DisplayName = "Z"),

	/**
	 * A mask with all the user-level flags set.
	 */
	FM_AllUserLevel = FM_A | FM_B | FM_C | FM_D | FM_E |
					  FM_F | FM_G | FM_H | FM_I | FM_J |
					  FM_K | FM_L | FM_M | FM_N | FM_O |
					  FM_P | FM_Q | FM_R | FM_S | FM_T |
					  FM_U | FM_V | FM_W | FM_X | FM_Y | FM_Z UMETA(Hidden),

	/**
	 * A mask with all the flags set.
	 */
	FM_All = FM_AllSystemLevel | FM_AllUserLevel UMETA(Hidden)

}; // enum EFlagmark

ENUM_CLASS_FLAGS(EFlagmark)

MORE_ENUM_CLASS_FLAGS(EFlagmark, EFlagmarkBit)

/**
 * Check if the type is actually a flagmark.
 */
template < typename T >
constexpr FORCEINLINE bool
IsFlagmarkType()
{
	return std::is_same<T, EFlagmark>::value;
}

/**
 * Check if a flagmark contains any system-level flags.
 * 
 * System-level flags can't be set by the user.
 * 
 * @param InFlagmark The flagmark to examine.
 * @return The state of examination.
 */
constexpr FORCEINLINE bool
HasSystemLevelFlags(const EFlagmark InFlagmark)
{
	return EnumHasAnyFlags(InFlagmark, FM_AllSystemLevel);
}

/**
 * Check if a flagmark contains any user-level flags.
 * 
 * Only user-level flags can be set by the user.
 * 
 * @param InFlagmark The flagmark to examine.
 * @return The state of examination.
 */
constexpr FORCEINLINE bool
HasUserLevelFlags(const EFlagmark InFlagmark)
{
	return EnumHasAnyFlags(InFlagmark, FM_AllUserLevel);
}

/**
 * Compile-time flagmark specification indicator.
 */
struct APPARATUSRUNTIME_API FFlagmarkIndicator
{};

/**
 * Compile-time exclusion indicator.
 */
struct APPARATUSRUNTIME_API FExclusionIndicator
{};

/**
 * A flagmark 
 * Used within compile-time functionality.
 */
template < enum EFlagmark InFlagmark >
struct TFlagmark
  : public FFlagmarkIndicator
{
	/**
	 * The flagmark value that hold up all the bits set.
	 */
	static constexpr EFlagmark Value = InFlagmark;
}; //-struct TFlagmark

#pragma region Compile-Time Indicators
/// @name Compile-Time Indicators
/// @{

/**
 * No-flags compile-time indicator.
 */
using FFM_None = TFlagmark<FM_None>;

/**
 * Stale flag compile-time indicator.
 */
using FFM_Stale = TFlagmark<FM_Stale>;

/**
 * Booted flag compile-time indicator.
 */
using FFM_Booted = TFlagmark<FM_Booted>;

/**
 * Online flag compile-time indicator.
 */
using FFM_Online = TFlagmark<FM_Online>;

/**
 * Editor flag compile-time indicator.
 */
using FFM_Editor = TFlagmark<FM_Editor>;

/**
 * Deferred despawn flag compile-time indicator.
 */
using FFM_DeferredDespawn = TFlagmark<FM_DeferredDespawn>;

/**
 * All system-level flags compile-time indicator.
 */
using FFM_AllSystemLevel = TFlagmark<FM_AllSystemLevel>;

/**
 * User A-flag compile-time indicator.
 */
using FFM_A = TFlagmark<FM_A>;

/**
 * User B-flag compile-time indicator.
 */
using FFM_B = TFlagmark<FM_B>;

/**
 * User C-flag compile-time indicator.
 */
using FFM_C = TFlagmark<FM_C>;

/**
 * User D-flag compile-time indicator.
 */
using FFM_D = TFlagmark<FM_D>;

/**
 * User E-flag compile-time indicator.
 */
using FFM_E = TFlagmark<FM_E>;

/**
 * User F-flag compile-time indicator.
 */
using FFM_F = TFlagmark<FM_F>;

/**
 * User G-flag compile-time indicator.
 */
using FFM_G = TFlagmark<FM_G>;

/**
 * User H-flag compile-time indicator.
 */
using FFM_H = TFlagmark<FM_H>;

/**
 * User I-flag compile-time indicator.
 */
using FFM_I = TFlagmark<FM_I>;

/**
 * User J-flag compile-time indicator.
 */
using FFM_J = TFlagmark<FM_J>;

/**
 * User K-flag compile-time indicator.
 */
using FFM_K = TFlagmark<FM_K>;

/**
 * User L-flag compile-time indicator.
 */
using FFM_L = TFlagmark<FM_L>;

/**
 * User M-flag compile-time indicator.
 */
using FFM_M = TFlagmark<FM_M>;

/**
 * User N-flag compile-time indicator.
 */
using FFM_N = TFlagmark<FM_N>;

/**
 * User O-flag compile-time indicator.
 */
using FFM_O = TFlagmark<FM_O>;

/**
 * User P-flag compile-time indicator.
 */
using FFM_P = TFlagmark<FM_P>;

/**
 * User Q-flag compile-time indicator.
 */
using FFM_Q = TFlagmark<FM_Q>;

/**
 * User R-flag compile-time indicator.
 */
using FFM_R = TFlagmark<FM_R>;

/**
 * User S-flag compile-time indicator.
 */
using FFM_S = TFlagmark<FM_S>;

/**
 * User T-flag compile-time indicator.
 */
using FFM_T = TFlagmark<FM_T>;

/**
 * User U-flag compile-time indicator.
 */
using FFM_U = TFlagmark<FM_U>;

/**
 * User V-flag compile-time indicator.
 */
using FFM_V = TFlagmark<FM_V>;

/**
 * User W-flag compile-time indicator.
 */
using FFM_W = TFlagmark<FM_W>;

/**
 * User X-flag compile-time indicator.
 */
using FFM_X = TFlagmark<FM_X>;

/**
 * User Y-flag compile-time indicator.
 */
using FFM_Y = TFlagmark<FM_Y>;

/**
 * User Z-flag compile-time indicator.
 */
using FFM_Z = TFlagmark<FM_Z>;

/**
 * All user-level flags compile-time indicator.
 */
using FFM_AllUserLevel = TFlagmark<FM_AllUserLevel>;

/**
 * All-flags-on compile-time indicator.
 */
using FFM_All = TFlagmark<FM_All>;

/// @}
#pragma endregion Compile-Time Indicators

/**
 * Safely provide a flagmark value for the entry.
 */
template< typename T,
		  bool     IsFlagmark = std::is_base_of<FFlagmarkIndicator, T>::value >
struct TFlagmarkGetter
{
	static constexpr EFlagmark Value = FM_None;

	static constexpr int Count = 0;
};

template< typename F >
struct TFlagmarkGetter<F, /*IsFlagmark=*/true>
{
	static constexpr EFlagmark Value = F::Value;

	static constexpr int Count = 1;
};

/**
 * Safely provide an excluding flagmark value for the entry.
 */
template< typename T,
		  bool     IsExclusion = std::is_base_of<FExclusionIndicator, T>::value >
struct TExcludingFlagmarkGetter
{
	static constexpr EFlagmark Value = FM_None;

	static constexpr int Count = 0;
};

template< typename E >
struct TExcludingFlagmarkGetter<E, /*IsExclusion=*/true>
{
	static constexpr EFlagmark Value = TFlagmarkGetter<typename E::Type>::Value;

	static constexpr int Count = TFlagmarkGetter<typename E::Type>::Count;
};

/**
 * Accumulates all of the flagmarks at compile-time.
 * 
 * @tparam Ts The types to accumulate. Only the flagmark types get in to the results.
 */
template< typename... Fs >
struct TFlagmarkAccumulator;

template< >
struct TFlagmarkAccumulator<>
{
	static constexpr EFlagmark Value = FM_None;

	static constexpr int Count = 0;
};

template< typename Head, typename... Tail >
struct TFlagmarkAccumulator<Head, Tail...>
{
	static constexpr EFlagmark Value = TFlagmarkGetter<Head>::Value | TFlagmarkAccumulator<Tail...>::Value;

	static constexpr int Count = TFlagmarkGetter<Head>::Count + TFlagmarkAccumulator<Tail...>::Count;
};

/**
 * Accumulates all of the excluding flagmarks at compile-time.
 * 
 * @tparam Ts The types to accumulate. Only the excluding flagmark types get in to the results.
 */
template< typename... Ts >
struct TExcludingFlagmarkAccumulator;

template< >
struct TExcludingFlagmarkAccumulator<>
{
	static constexpr EFlagmark Value = FM_None;

	static constexpr int Count = 0;
};

template< typename Head, typename... Tail >
struct TExcludingFlagmarkAccumulator<Head, Tail...>
{
	static constexpr EFlagmark Value = TExcludingFlagmarkGetter<Head>::Value | TExcludingFlagmarkAccumulator<Tail...>::Value;

	static constexpr int Count = TExcludingFlagmarkGetter<Head>::Count + TExcludingFlagmarkAccumulator<Tail...>::Count;
};

/**
 * Exclude a certain component or flagmark.
 * 
 * @tparam T The type of entity to exclude: trait, detail, flagmark.
 */
template< typename T >
struct TExclude : public FExclusionIndicator
{
	typedef T Type;
};

/**
 * Get the flagmark's Unreal Engine's reflective class object.
 */
inline APPARATUSRUNTIME_API UEnum*
GetFlagmarkClass()
{
	static UEnum* EnumType = FindObject<UEnum>(nullptr, TEXT("/Script/ApparatusRuntime.EFlagmark"), true);
	checkf(EnumType != nullptr, TEXT("The flagmark class is a system one and must always be present."));
	return EnumType;
}

/**
 * Convert a flagmark enum variable to a string representation.
 * 
 * @param Flagmark The flagmark to convert to a string.
 * @return The string representation of the flagmark.
 */
APPARATUSRUNTIME_API FString
ToString(const EFlagmark Flagmark);

/**
 * Hash the boot filter.
 */
FORCEINLINE uint32
GetTypeHash(const EFlagmark Flagmark)
{
	return GetTypeHash((EFlagmarkType)Flagmark);
}

/**
 * Serialize a boot filter to an archive.
 */
FORCEINLINE FArchive&
operator<<(FArchive& Archive, EFlagmark& Flagmark)
{ 
	auto Raw = (uint64)Flagmark;
	Archive << Raw;
	
	if (Archive.IsLoading())
	{
		Flagmark = (EFlagmark)Raw;
	}

	return Archive;
}

/**
 * Check if the flagmark matches a flagmark.
 */
constexpr FORCEINLINE bool
Matches(const EFlagmark Flagmark,
		const EFlagmark Filter)
{
	return EnumHasAllFlags(Flagmark, Filter);
}

/**
 * Check if the flagmark matches an including and excluding flagmark.
 */
constexpr FORCEINLINE bool
Matches(const EFlagmark Flagmark,
		const EFlagmark Filter,
		const EFlagmark ExcludingFilter)
{
	return Matches(Flagmark, Filter) &&
		   !EnumHasAnyFlags(Flagmark, ExcludingFilter);
}
