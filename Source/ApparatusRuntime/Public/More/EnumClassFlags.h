/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: EnumClassFlags.h
 * Created: 2021-10-26 09:20:51
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
 * @brief Additional utilities for Unreal Engine's enum routines macros.
 */

#pragma once

#include "EnumClassFlags.h"


/**
 * Operators with individual bit functionality.
 * 
 * @param Enum The type of enum bit flags.
 * @param Bit The type of the individual bits enum.
 */
#define MORE_ENUM_CLASS_FLAGS(Enum, Bit)                                                                                                                              \
	using Enum##Type = __underlying_type(Enum);                                                                                                                       \
	inline           Enum  FlagToEnum(const Bit Flag)            { return (Enum)((__underlying_type(Enum))1 << (int)Flag); }                                          \
	inline           Enum& operator-=(Enum& Lhs, const Enum Rhs) { return Lhs = (Enum)( (__underlying_type(Enum))Lhs & ~((__underlying_type(Enum))Rhs) ); }           \
	inline constexpr Enum  operator- (Enum  Lhs, const Enum Rhs) { return (Enum)( (__underlying_type(Enum))Lhs & ~((__underlying_type(Enum))Rhs) ); }                 \
	inline           Enum& operator|=(Enum& Lhs, const Bit  Rhs) { return Lhs = (Enum)( (__underlying_type(Enum))Lhs | ((__underlying_type(Enum))1 << (int)Rhs) ); }  \
	inline           Enum& operator&=(Enum& Lhs, const Bit  Rhs) { return Lhs = (Enum)( (__underlying_type(Enum))Lhs & ((__underlying_type(Enum))1 << (int)Rhs) ); }  \
	inline           Enum& operator^=(Enum& Lhs, const Bit  Rhs) { return Lhs = (Enum)( (__underlying_type(Enum))Lhs ^ ((__underlying_type(Enum))1 << (int)Rhs) ); }  \
	inline constexpr Enum  operator| (Enum  Lhs, const Bit  Rhs) { return (Enum)( (__underlying_type(Enum))Lhs | ((__underlying_type(Enum))1 << (int)Rhs) ); }        \
	inline constexpr Enum  operator& (Enum  Lhs, const Bit  Rhs) { return (Enum)( (__underlying_type(Enum))Lhs & ((__underlying_type(Enum))1 << (int)Rhs) ); }        \
	inline constexpr Enum  operator^ (Enum  Lhs, const Bit  Rhs) { return (Enum)( (__underlying_type(Enum))Lhs ^ ((__underlying_type(Enum))1 << (int)Rhs) ); }        \
	inline           Enum& operator+=(Enum& Lhs, const Bit  Rhs) { return Lhs = (Enum)( (__underlying_type(Enum))Lhs |   (__underlying_type(Enum))1 << (int)Rhs ); }  \
	inline           Enum& operator-=(Enum& Lhs, const Bit  Rhs) { return Lhs = (Enum)( (__underlying_type(Enum))Lhs & ~((__underlying_type(Enum))1 << (int)Rhs) ); } \
	inline constexpr Enum  operator- (Enum  Lhs, const Bit  Rhs) { return (Enum)( (__underlying_type(Enum))Lhs & ~((__underlying_type(Enum))1 << (int)Rhs) ); }       \
	inline constexpr Enum  operator+ (Enum  Lhs, const Bit  Rhs) { return (Enum)( (__underlying_type(Enum))Lhs |  ((__underlying_type(Enum))1 << (int)Rhs) ); }       \
	inline constexpr Enum  operator| (const Bit Lhs, const Bit Rhs) { return (Enum)( ((__underlying_type(Enum))1 << (int)Lhs) | ((__underlying_type(Enum))1 << (int)Rhs) ); } \
	inline constexpr Enum  operator& (const Bit Lhs, const Bit Rhs) { return (Enum)( ((__underlying_type(Enum))1 << (int)Lhs) & ((__underlying_type(Enum))1 << (int)Rhs) ); }

/**
 * Set or clear an enum flag.
 * 
 * @tparam EnumT The type of enum to set.
 * @tparam BitT The bit type to set.
 * @param Flags The flags to alter.
 * @param Bit The bit to change.
 * @param bState The state to set.
 */
template < typename EnumT, typename BitT >
FORCEINLINE EnumT&
EnumSetFlag(EnumT& Flags, const BitT Bit, const bool bState = true)
{
	if (bState)
	{
		return Flags += Bit;
	}
	else
	{
		return Flags -= Bit;
	}
}

/**
 * Enum with a set/cleared bit. The original enum is not changed.
 * 
 * @tparam EnumT The type of enum to set.
 * @tparam BitT The bit type to set.
 * @param Flags The flags to alter.
 * @param Bit The bit to change.
 * @param bState The state to set.
 */
template < typename EnumT, typename BitT >
constexpr FORCEINLINE EnumT
EnumWithFlagSet(const EnumT Flags,
				const BitT  Bit,
				const bool  bState = true)
{
	return bState ? Flags + Bit : Flags - Bit;
}

/**
 * Enum with a toggled bit. The original enum is not changed.
 * 
 * @tparam EnumT The type of enum to toggle within.
 * @tparam BitT The bit type to toggle.
 * @param Flags The flags to alter.
 * @param Bit The bit to toggle.
 */
template < typename EnumT, typename BitT >
constexpr FORCEINLINE EnumT
EnumWithFlagToggled(const EnumT Flags,
					const BitT  Bit)
{
	return Flags ^ Bit;
}

/**
 * Examine if the enum has a certain flag set.
 * 
 * @tparam EnumT The type of enum to set.
 * @tparam BitT The bit type to set.
 * @param Flags The flags to alter.
 * @param Bit The flagmark bit to examine.
 * @return true The bit is set within the flags.
 * @return false The bit is cleared within the flags.
 */
template < typename EnumT, typename BitT >
constexpr FORCEINLINE bool
EnumHasFlag(const EnumT Flags, const BitT Bit)
{
	return (((__underlying_type(EnumT))Flags) & ((__underlying_type(EnumT))1 << (int)Bit));
}
