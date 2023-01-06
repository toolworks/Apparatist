/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Paradigm.h
 * Created: 2022-10-10 13:36:08
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
/**
 * @file 
 * @brief The execution paradigm functionality.
 */

#pragma once

#include <algorithm>

#include "CoreMinimal.h"


// Forward declarations:
enum class EApparatusStatus : int8;

/**
 * The type of paradigm for executing.
 */
enum class EParadigm
{
	/**
	 * "Not an execution paradigm" identifier.
	 * 
	 * Also serves the purpose of an automatic paradigm derivation.
	 */
	None = 0,

	/**
	 * A mnemonic synonym for EParadigm::None.
	 */
	Auto = 0,

	/**
	 * A flag to return statuses instead
	 * of firing assertion failures.
	 */
	Polite = 1,

	/**
	 * The paradigm options mask.
	 */
	OptionsMask = 0x1,

	/**
	 * @internal The safety paradigm used internally
	 * within the engine. Not for the public usage at all.
	 */
	Internal = 0x1 << 1,

	/**
	 * Harsh internal paradigm.
	 */
	HarshInternal = Internal,

	/**
	 * Internal harsh version.
	 */
	InternalHarsh = HarshInternal,

	/**
	 * Polite internal version.
	 */
	PoliteInternal = Polite | Internal,

	/**
	 * Internal polite version.
	 */
	InternalPolite = Internal | Polite,

	/**
	 * Allows for direct access to the data
	 * even through non-solid subject handles.
	 */
	Unsafe = 0x2 << 1,

	/**
	 * Harsh unsafe version.
	 */
	HarshUnsafe = Unsafe,

	/**
	 * Unsafe harsh version.
	 */
	UnsafeHarsh = HarshUnsafe,

	/**
	 * Polite unsafe version.
	 */
	PoliteUnsafe = Polite | Unsafe,

	/**
	 * Unsafe polite version.
	 */
	UnsafePolite = Unsafe | Polite,

	/**
	 * Disallowing direct data
	 * access for non-solid subjects.
	 */
	Safe = 0x3 << 1,

	/**
	 * Harsh safe is just safe.
	 */
	HarshSafe = Safe,

	/**
	 * Safe harsh synonym.
	 */
	SafeHarsh = HarshSafe,

	/**
	 * Polite safe synonym.
	 */
	PoliteSafe = Polite | Safe,

	/**
	 * Safe polite synonym.
	 */
	SafePolite = Safe | Polite,

	/**
	 * The safety level mask.
	 */
	LevelMask = 0x3 << 1,

	/**
	 * The default paradigm for public APIs.
	 */
	Default = HarshSafe,

	/**
	 * The default paradigm for non-public APIs.
	 */
	DefaultInternal = HarshInternal,

	/**
	 * The default paradigm for maximum dynamic (virtual) APIs.
	 */
	DefaultPortable = SafePolite,

	/**
	 * The default paradigm for Blueprint APIs.
	 */
	DefaultBlueprint = SafePolite
}; //-enum class EParadigm

ENUM_CLASS_FLAGS(EParadigm)

/**
 * Combine two paradigm into a single one.
 * 
 * The safest level will get propagated.
 * 
 * @param A The first paradigm to combine.
 * @param B The second paradigm to combine.
 * @return The resulting paradigm.
 */
constexpr FORCEINLINE EParadigm
operator+(const EParadigm A, const EParadigm B)
{
	return std::max(A & EParadigm::LevelMask, B & EParadigm::LevelMask)
		 | (A & EParadigm::Polite) | (B & EParadigm::Polite);
}

/**
 * Accumulate to a paradigm.
 * 
 * The safest level will get propagated.
 * 
 * @param A The first paradigm to combine and receive the result.
 * @param B The second paradigm to accumulate.
 * @return Returns the first operand itself.
 */
FORCEINLINE EParadigm&
operator+=(EParadigm& A, const EParadigm B)
{
	A = A + B;
	return A;
}

/**
 * The execution paradigm mark.
 * 
 * Used to identify the paradigm enum-like
 * types.
 */
struct FParadigmIndicator
{};

/**
 * A type wrap for the paradigm enumeration.
 * 
 * This is to transform the enumeration constant
 * to a type name.
 */
template < EParadigm Paradigm >
struct TParadigm : public FParadigmIndicator
{
	/**
	 * The type of the paradigm enumeration.
	 */
	typedef EParadigm Type;

	/**
	 * The value of the paradigm.
	 */
	static constexpr EParadigm Value = Paradigm;
};

using FPoliteParadigm = TParadigm<EParadigm::Polite>;

using FPoliteSafeParadigm = TParadigm<EParadigm::PoliteSafe>;

using FHarshSafeParadigm = TParadigm<EParadigm::HarshSafe>;

using FPoliteUnsafeParadigm = TParadigm<EParadigm::PoliteUnsafe>;

using FHarshUnsafeParadigm = TParadigm<EParadigm::HarshUnsafe>;

using FDefaultParadigm = TParadigm<EParadigm::Default>;

using FDefaultBlueprintParadigm = TParadigm<EParadigm::DefaultBlueprint>;

using FPortableParadigm = TParadigm<EParadigm::DefaultPortable>;

template < typename... List >
struct TParadigmAccumulator
{
	static constexpr EParadigm Value = EParadigm::None;

	static constexpr size_t Count = 0;
};

template < typename AnyT >
struct TParadigmAccumulator<AnyT>
{
	static constexpr EParadigm Value = EParadigm::None;

	static constexpr size_t Count = 0;
};

template < EParadigm Paradigm >
struct TParadigmAccumulator<TParadigm<Paradigm>>
{
	static constexpr EParadigm Value = Paradigm;

	static constexpr size_t Count = 1;
};

template < typename Head, typename... Tail >
struct TParadigmAccumulator<Head, Tail...>
{
	static constexpr EParadigm Value = TParadigmAccumulator<Head>::Value + TParadigmAccumulator<Tail...>::Value;

	static constexpr size_t Count = TParadigmAccumulator<Head>::Count + TParadigmAccumulator<Tail...>::Count;
};

/**
 * Combine all of the supplied paradigm types.
 * 
 * @tparam Ts The paradigm types to combine.
 * The non-paradigm types are silently ignored.
 * @param DefaultParadigm The default paradigm
 * @return The resulting paradigm.
 */
template < typename... Ts >
constexpr FORCEINLINE EParadigm
ParadigmCombine(const EParadigm DefaultParadigm = EParadigm::Default)
{
	using AccumulatedType = TParadigmAccumulator<Ts...>;
	if (AccumulatedType::Count == 0)
	{
		return DefaultParadigm;
	}
	return TParadigmAccumulator<Ts...>::Value;
}

/**
 * Check if the paradigm is polite.
 * 
 * @tparam Paradigm The paradigm to examine.
 * @return The state of examination.
 */
constexpr FORCEINLINE bool
IsPolite(const EParadigm Paradigm)
{
	return EnumHasAllFlags(Paradigm, EParadigm::Polite);
}

/**
 * Check if the paradigm type is polite.
 * 
 * @tparam ParadigmT The paradigm type to examine.
 * @return The state of examination.
 */
template < typename ParadigmT >
constexpr FORCEINLINE bool
IsPolite()
{
	return IsPolite(ParadigmT::Value);
}

/**
 * Check if the paradigm is harsh.
 * 
 * @tparam Paradigm The paradigm to examine.
 * @return The state of examination.
 */
constexpr FORCEINLINE bool
IsHarsh(const EParadigm Paradigm)
{
	return !IsPolite(Paradigm);
}

/**
 * Check if the paradigm type is harsh.
 * 
 * @tparam ParadigmT The paradigm type to examine.
 * @return The state of examination.
 */
template < typename ParadigmT >
constexpr FORCEINLINE bool
IsHarsh()
{
	return IsHarsh(ParadigmT::Value);
}

/**
 * Check if the paradigm is internal.
 * 
 * @param Paradigm The paradigm to examine.
 * @return The state of examination.
 */
constexpr FORCEINLINE bool
IsInternal(const EParadigm Paradigm)
{
	return ((Paradigm & EParadigm::LevelMask) <= EParadigm::Internal) && (Paradigm != EParadigm::Polite);
}

/**
 * Check if the paradigm type is internal.
 * 
 * @tparam ParadigmT The paradigm type to examine.
 * @return Check if the paradigm type is internal.
 */
template < typename ParadigmT >
constexpr FORCEINLINE bool
IsInternal()
{
	return IsInternal(ParadigmT::Value);
}

/**
 * Check if a paradigm an unsafe one.
 * 
 * @param Paradigm The paradigm to examine.
 * @return The status of examination.
 */
constexpr FORCEINLINE bool
IsUnsafe(const EParadigm Paradigm)
{
	return ((Paradigm & EParadigm::LevelMask) <= EParadigm::Unsafe) && (Paradigm != EParadigm::Polite);
}

/**
 * Check if the paradigm type is unsafe.
 * 
 * @tparam ParadigmT The paradigm type to examine. 
 * @return The state of examination.
 */
template < typename ParadigmT >
constexpr FORCEINLINE bool
IsUnsafe()
{
	return IsUnsafe(ParadigmT::Value);
}

/**
 * Check if a paradigm a safe one.
 * 
 * @param Paradigm The paradigm to examine.
 * @return The status of examination.
 */
constexpr FORCEINLINE bool
IsSafe(const EParadigm Paradigm)
{
	// Plain 'Polite' is considered as safe by default.
	return ((Paradigm & EParadigm::LevelMask) >= EParadigm::Safe) || (Paradigm == EParadigm::Polite);
}

/**
 * Check if a paradigm type is a safe one.
 * 
 * @tparam ParadigmT The paradigm type to examine.
 * @return The state of examination.
 */
template < typename ParadigmT >
constexpr FORCEINLINE bool
IsSafe()
{
	return IsSafe(ParadigmT::Value);
}

/**
 * Guarantee a paradigm to be polite.
 * 
 * @param Paradigm The paradigm to alter.
 * @return The resulting polite paradigm.
 */
constexpr FORCEINLINE EParadigm
MakePolite(const EParadigm Paradigm)
{
	return Paradigm | EParadigm::Polite;
}

/**
 * Guarantee a paradigm to be safe (or more).
 * 
 * @param Paradigm The paradigm to alter.
 * @return The resulting safe paradigm.
 */
constexpr FORCEINLINE EParadigm
MakeSafe(const EParadigm Paradigm)
{
	return IsSafe(Paradigm) ? Paradigm : (EParadigm::Safe | (Paradigm & EParadigm::OptionsMask));
}

/**
 * Guarantee a paradigm to be unsafe (or less).
 * 
 * @param Paradigm The paradigm to alter.
 * @return The resulting unsafe paradigm.
 */
constexpr FORCEINLINE EParadigm
MakeUnsafe(const EParadigm Paradigm)
{
	return IsUnsafe(Paradigm) ? Paradigm : (EParadigm::Unsafe | (Paradigm & EParadigm::OptionsMask));
}

/**
 * Guarantee a paradigm to be internal.
 * 
 * @param Paradigm The paradigm to alter.
 * @return The resulting internal paradigm.
 */
constexpr FORCEINLINE EParadigm
MakeInternal(const EParadigm Paradigm)
{
	return IsInternal(Paradigm) ? Paradigm : (EParadigm::Internal | (Paradigm & EParadigm::OptionsMask));
}

/**
 * Guarantee a paradigm to be polite.
 * 
 * @tparam ParadigmT The paradigm type to assess.
 */
template < typename ParadigmT >
using TPoliteParadigm = TParadigm<MakePolite(ParadigmT::Value)>;

/**
 * Guarantee a paradigm to be harsh.
 * 
 * @param Paradigm The paradigm to alter.
 * @return The resulting harsh paradigm.
 */
constexpr FORCEINLINE EParadigm
MakeHarsh(const EParadigm Paradigm)
{
	return Paradigm & (~EParadigm::Polite);
}

/**
 * Guarantee a paradigm to be harsh.
 * 
 * @tparam ParadigmT The paradigm type to assess.
 */
template < typename ParadigmT >
using THarshParadigm = TParadigm<MakeHarsh(ParadigmT::Value)>;

/**
 * Enable the method in unsafe mode only.
 * 
 * @tparam Paradigm The paradigm in question.
 * @tparam ResultT The type of the return value.
 */
template < EParadigm Paradigm, typename ResultT = EApparatusStatus >
using TEnableIfUnsafe = typename std::enable_if<IsUnsafe(Paradigm), ResultT>::type;

/**
 * Enable the method in unsafe mode or in case of a condition.
 * 
 * @tparam Paradigm The paradigm in question.
 * @tparam Condition The condition to also check for.
 * @tparam ResultT The type of the return value.
 */
template < EParadigm Paradigm, bool Condition, typename ResultT = EApparatusStatus >
using TEnableIfUnsafeOr = typename std::enable_if<IsUnsafe(Paradigm) || Condition, ResultT>::type;
