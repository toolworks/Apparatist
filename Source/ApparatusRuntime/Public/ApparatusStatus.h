/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusStatus.h
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
/**
 * @file 
 * @brief The Apparatus status reporting facility.
 */

#pragma once

#include <cstddef>

#include "CoreMinimal.h"
#include "UObject/Class.h"

#include "More/type_traits"
#include "ApparatusRuntime.h"


// Uncomment to debug Apparatus:
//#define APPARATUS_VERSBOSE

// Forward declarations:
template < typename PayloadT = void >
struct THarshOutcome;

template < typename PayloadT = void >
struct TPoliteOutcome;

/**
 * The general Apparatus operation status type.
 *
 * Apparatus uses its own status reporting mechanism
 * where applicable.
 * Values greater than or equal to EApparatusStatus::Success
 * are variations of success, lesser than - are errors.
 *
 * @remark Report() functions should be used to report the status.
 * 
 * @see Report()
 */
enum class EApparatusStatus : int8
{
	/**
	 * Nothing was actually performed, but is still considered to be a success.
	 */
	Noop = 0,

	/**
	 * Success status. The "no-error" identifier.
	 */
	Success = 1,

	/**
	 * The delivered item was actually reused.
	 */
	Reused = 2,

	/**
	 * Success with no items collected status.
	 * 
	 * The operation has completed successfully
	 * but there were no items gathered.
	 */
	NoItems = 3,

	/**
	 * Deferred action status.
	 * 
	 * The operation has completed successfully
	 * but the actual changes are deferred
	 * to happen at some later time.
	 */
	Deferred = 4,

	/*
	 * TODO: Examine this idea further.
	 * The operation is actually already happening
	 * and we're already part of it.
	 * 
	 * This status is returned when preventing a recursion.
	 */
	// InProgress = 5,

	/**
	 * General error identifier.
	 */
	Error = -1,

	/**
	 * One of the arguments' value was invalid.
	 */
	InvalidArgument = -2,

	/**
	 * One of the required arguments was NULL.
	 */
	NullArgument = -3,

	/**
	 * The argument is out of range of allowed values.
	 */
	OutOfRange = -4,

	/**
	 * No more items available within the stream/iterating
	 * or the operation is no longer viable.
	 */
	NoMore = -5,

	/**
	 * Some kind of entity is not currently available.
	 */
	NotAvailable = -6,

	/**
	 * Some kind of entity was not found.
	 */
	Missing = -7,

	/**
	 * The operation is not valid in the current state.
	 */
	InvalidOperation = -8,

	/**
	 * The current state is invalid (corrupted).
	 */
	InvalidState = -9,

	/*
	 * The type is incompatible with the operation.
	 */
	WrongType = -10,

	/**
	 * The operation seems valid but is currently not supported.
	 */
	UnsupportedOperation = -11,

	/**
	 * The needed implementation for the (virtual) operation is missing.
	 */
	NoImplementation = -12,

	/**
	 * There is some kind of conflict or contradiction
	 * that has prevented the operation to succeed.
	 */
	Conflict = -13,

	/**
	 * There is some kind of sanity check that has
	 * failed while executing the operation.
	 */
	SanityCheckFailed = -14,

	/**
	 * The operation is not permitted due to security reasons.
	 */
	NoPermission = -15,

	/**
	 * The limit for some entities has been reached.
	 */
	OutOfLimit = -16,

	/**
	 * Memory allocation has failed.
	 */
	NoMemory = -17,

	/**
	 * Fatal error, leading to termination of execution.
	 */
	FatalError = -18

}; //-enum class EApparatusStatus

/**
 * Check if the passed in type is a status.
 * 
 * @tparam T The type to check.
 * @return The state of examination. 
 */
template < typename T >
constexpr FORCEINLINE bool
IsStatusType()
{
	return std::is_same<T, EApparatusStatus>::value;
}

/**
 * Check if the status is a successful one.
 */
constexpr FORCEINLINE bool
OK(const EApparatusStatus Status)
{
	return Status > EApparatusStatus::Error;
}

/**
 * Check if a status is a noop.
 */
constexpr FORCEINLINE bool
IsNoop(const EApparatusStatus Status)
{
	return Status == EApparatusStatus::Noop;
}

/**
 * Universal conversion to a status.
 */
constexpr FORCEINLINE EApparatusStatus
ToStatus(const EApparatusStatus Status)
{
	return Status;
}

/**
 * Noop.
 * 
 * @return The resulting noop.
 */
constexpr FORCEINLINE EApparatusStatus
StatusCombine()
{
	return EApparatusStatus::Noop;
}

/**
 * Bypass a status.
 * 
 * @param A The first status to bypass.
 * @return The resulting status.
 */
constexpr FORCEINLINE EApparatusStatus
StatusCombine(const EApparatusStatus A)
{
	return A;
}

/**
 * Combine two statuses deriving a new one based on them.
 * 
 * Be warned the the order of evaluation for function arguments
 * is not explicitly defined or guaranteed in C++, so @p B can
 * evaluate prior to @p A.
 * 
 * @param A The first status to combine.
 * @param B The second status to combine.
 * @return The resulting status.
 */
constexpr FORCEINLINE EApparatusStatus
StatusCombine(const EApparatusStatus A, const EApparatusStatus B)
{
	return (B == EApparatusStatus::Noop) || !OK(A) ? A : B;
}

/**
 * Combine two statuses deriving a new one based on them.
 * 
 * Be warned the the order of evaluation for operator operands
 * is not explicitly defined or guaranteed in C++ so @p B can be evaluated
 * before @p A.
 */
constexpr FORCEINLINE EApparatusStatus
operator+(const EApparatusStatus A, const EApparatusStatus B)
{
	return ::StatusCombine(A, B);
}

/**
 * Combine several statuses deriving a new one based on them.
 * Array version.
 * 
 * @tparam N The number of elements in the array.
 * @param Statuses The array of statuses to combine.
 * @param DefaultStatus The default status to be returned,
 * when no statuses are in the array.
 */
template < size_t N >
FORCEINLINE EApparatusStatus
StatusCombine(const EApparatusStatus (&Statuses)[N], const EApparatusStatus DefaultStatus = EApparatusStatus::Noop)
{
	if (N == 0) // Compile-time branch.
	{
		return DefaultStatus;
	}
	auto Status = Statuses[0];
	for (int32 i = 1; i < N; ++i) // Compile-time unwinding.
	{
		Status = StatusCombine(Status, Statuses[i]);
	}
	return Status;
}

/**
 * Combine several statuses deriving a new one based on them.
 * Initializer list version.
 * 
 * @param Statuses The list of statuses to combine.
 * @param DefaultStatus The default status to be returned,
 * when no statuses are in the array.
 */
FORCEINLINE EApparatusStatus
StatusCombine(std::initializer_list<EApparatusStatus> Statuses,
			  const EApparatusStatus DefaultStatus = EApparatusStatus::Noop)
{
	if (Statuses.size() == 0)
	{
		return DefaultStatus;
	}
	auto Status = EApparatusStatus::Noop;
	for (auto CurStatus : Statuses)
	{
		Status = StatusCombine(Status, CurStatus);
	}
	return Status;
}

/**
 * Combine several statuses deriving a new one based on them.
 * 
 * @warning The order of the evaluation of the function arguments
 * is not explicitly defined or guaranteed in C++.
 * 
 * @note The function is recursive.
 */
template < typename... Ts >
constexpr FORCEINLINE EApparatusStatus
StatusCombine(const EApparatusStatus StatusA, const EApparatusStatus StatusB, const Ts... OtherStatuses)
{
	return StatusCombine(StatusCombine(StatusA, StatusB), OtherStatuses...);
}

#pragma region Accumulation

/**
 * Accumulate several statuses deriving a new one based on them.
 */
template < size_t N >
FORCEINLINE EApparatusStatus&
StatusAccumulate(EApparatusStatus& Status, const EApparatusStatus (&Statuses)[N])
{
	for (int32 i = 0; i < N; ++i) // Compile-time unwinding.
	{
		StatusAccumulate(Status, Statuses[i]);
	}
	return Status;
}

/**
 * Accumulate several statuses deriving a new one based on them.
 * Single argument version.
 */
FORCEINLINE EApparatusStatus&
StatusAccumulate(EApparatusStatus& Status)
{
	return Status;
}

/**
 * Accumulate several statuses deriving a new one based on them.
 * Two arguments version.
 * 
 * Be warned the the order of evaluation for function arguments
 * is not explicitly defined or guaranteed in C++.
 */
FORCEINLINE EApparatusStatus&
StatusAccumulate(EApparatusStatus& Status, const EApparatusStatus AddStatus)
{
	Status = StatusCombine(Status, AddStatus);
	return Status;
}

/**
 * Accumulate several statuses deriving a new one based on them.
 * Multiple arguments version.
 * 
 * Be warned the the order of evaluation for function arguments
 * is not explicitly defined or guaranteed in C++.
 */
template < typename... Ts >
FORCEINLINE EApparatusStatus&
StatusAccumulate(EApparatusStatus& Status, const EApparatusStatus AddStatus, Ts&&... AddStatuses)
{
	return StatusAccumulate(StatusAccumulate(Status, AddStatus), std::forward<Ts>(AddStatuses)...);
}

#pragma endregion Accumulation

/**
 * Accumulate a status to an existing status.
 */
FORCEINLINE EApparatusStatus&
operator+=(EApparatusStatus& A, const EApparatusStatus B)
{
	return ::StatusAccumulate(A, B);
}

/**
 * Accumulate a payload to an existing status.
 */
template < typename PayloadT >
FORCEINLINE EApparatusStatus
operator+=(const EApparatusStatus A, const PayloadT& B)
{
	return A;
}

/**
 * Accumulate a status to an existing payload.
 */
template < typename PayloadT >
FORCEINLINE EApparatusStatus
operator+=(const PayloadT& A, const EApparatusStatus B)
{
	return B;
}

/**
 * Convert an Apparatus status variable to a string representation.
 * 
 * @param Status The status to convert.
 * @return The resulting string representation.
 */
inline FString
ToString(const EApparatusStatus Status)
{
	switch (Status)
	{
		case EApparatusStatus::Noop:
			return FString(TEXT("Noop"));
		case EApparatusStatus::Success:
			return FString(TEXT("Success"));
		case EApparatusStatus::Reused:
			return FString(TEXT("Reused"));
		case EApparatusStatus::NoItems:
			return FString(TEXT("NoItems"));
		case EApparatusStatus::Deferred:
			return FString(TEXT("Deferred"));
		case EApparatusStatus::Error:
			return FString(TEXT("Error"));
		case EApparatusStatus::InvalidArgument:
			return FString(TEXT("InvalidArgument"));
		case EApparatusStatus::NullArgument:
			return FString(TEXT("NullArgument"));
		case EApparatusStatus::OutOfRange:
			return FString(TEXT("OutOfRange"));
		case EApparatusStatus::NoMore:
			return FString(TEXT("NoMore"));
		case EApparatusStatus::NotAvailable:
			return FString(TEXT("NotAvailable"));
		case EApparatusStatus::Missing:
			return FString(TEXT("Missing"));
		case EApparatusStatus::InvalidOperation:
			return FString(TEXT("InvalidOperation"));
		case EApparatusStatus::InvalidState:
			return FString(TEXT("InvalidState"));
		case EApparatusStatus::WrongType:
			return FString(TEXT("WrongType"));
		case EApparatusStatus::UnsupportedOperation:
			return FString(TEXT("UnsupportedOperation"));
		case EApparatusStatus::Conflict:
			return FString(TEXT("Conflict"));
		case EApparatusStatus::SanityCheckFailed:
			return FString(TEXT("SanityCheckFailed"));
		case EApparatusStatus::NoPermission:
			return FString(TEXT("NoPermission"));
		case EApparatusStatus::OutOfLimit:
			return FString(TEXT("OutOfLimit"));
		case EApparatusStatus::NoMemory:
			return FString(TEXT("NoMemory"));
		case EApparatusStatus::FatalError:
			return FString(TEXT("FatalError"));
	}
	return FString(TEXT("<Invalid-Status>"));
}

/**
 * Make sure the check is passing
 * or otherwise either return a status or raise
 * an assertion failure.
 * 
 * @param Paradigm The paradigm of the method.
 * @param Expr The expression to evaluate.
 * @param Status The status to return in case of an error.
 */
#define AssessCondition(Paradigm, Expr, Status) \
	if (IsPolite(Paradigm))                 \
	{                                       \
		if (!(Expr))                        \
		{                                   \
			return Status;                  \
		}                                   \
	}                                       \
	else                                    \
	{                                       \
		check(Expr);                        \
	}

/**
 * Make sure the check is passing
 * or otherwise either return a status or raise
 * an assertion failure.
 * Message version.
 * 
 * @param Paradigm The paradigm of the method.
 * @param Expression The expression to evaluate.
 * @param Status The status to return in case of an error.
 * @param Format The formatting message to output in case
 * of an error.
 * @param ... The arguments for the formatting.
 */
#define AssessConditionFormat(Paradigm, Expression, Status, Format, ...) \
	if (IsPolite(Paradigm))                                              \
	{                                                                    \
		if (!(Expression))                                               \
		{                                                                \
			return Status;                                               \
		}                                                                \
	}                                                                    \
	else                                                                 \
	{                                                                    \
		checkf(Expression, Format, ##__VA_ARGS__);                       \
	}

/**
 * A common base class for all outcome types.
 */
struct FCommonOutcome {};

/**
 * A common base class for all harsh outcome types.
 */
struct FCommonHarshOutcome : FCommonOutcome {};

/**
 * A common base class for all polite outcome types.
 */
struct FCommonPoliteOutcome : FCommonOutcome {};

/**
 * Check if a type is an outcome.
 * 
 * @tparam T The type to examine.
 * @return The state of examination.
 */
template < typename T >
constexpr FORCEINLINE bool
IsOutcomeType()
{
	return std::is_base_of<FCommonOutcome, T>::value;
}

/**
 * Check if a type is a harsh outcome.
 * 
 * @tparam T The type to examine.
 * @return The state of examination.
 */
template < typename T >
constexpr FORCEINLINE bool
IsHarshOutcomeType()
{
	return std::is_base_of<FCommonHarshOutcome, T>::value;
}

/**
 * Check if a type is a polite outcome.
 * 
 * @tparam T The type to examine.
 * @return The state of examination.
 */
template < typename T >
constexpr FORCEINLINE bool
IsPoliteOutcomeType()
{
	return std::is_base_of<FCommonPoliteOutcome, T>::value;
}

template < typename T, bool bArray = std::is_array<T>::value >
struct TStatusArrayTester
{
	static constexpr bool Value = IsStatusType<more::array_value_t<T>>();
};

template < typename T >
struct TStatusArrayTester<T, /*bArray=*/false >
{
	static constexpr bool Value = IsStatusType<std::remove_pointer_t<T>>();
};

template < typename T >
constexpr FORCEINLINE bool
IsStatusArrayType()
{
	return TStatusArrayTester<T>::Value;
}

template < typename T, bool bArray = std::is_array<T>::value >
struct TOutcomeArrayTester
{
	static constexpr bool Value = IsOutcomeType<more::array_value_t<T>>();
};

template < typename T >
struct TOutcomeArrayTester<T, /*bArray=*/false >
{
	static constexpr bool Value = IsOutcomeType<std::remove_pointer_t<T>>();
};

template < typename T >
constexpr FORCEINLINE bool
IsOutcomeArrayType()
{
	return TOutcomeArrayTester<T>::Value || IsStatusArrayType<T>();
}

/**
 * Check if a type is a payload.
 * 
 * @tparam T The type to examine.
 * @return The state of examination.
 */
template < typename T >
constexpr FORCEINLINE bool
IsPayloadType()
{
	return (!IsOutcomeType<T>()) && (!IsStatusType<T>()) && (!IsOutcomeArrayType<T>()) && (!IsStatusArrayType<T>());
}

/**
 * A guarantee for a type to be a payload.
 * 
 * @tparam T The type to examine.
 */
template < typename T >
using TPayloadTypeSecurity = more::enable_if_t<IsPayloadType<T>(), bool>;

/**
 * Harsh outcome with no status but an optional payload.
 * 
 * @tparam PayloadT The optional payload to provide.
 */
template < typename PayloadT >
struct THarshOutcome
  : public FCommonHarshOutcome
{
	/**
	 * The type of the payload.
	 */
	using PayloadType = PayloadT;

	/**
	 * The payload of the outcome.
	 */
	PayloadT Payload;

	/**
	 * A constant denoting a successful operation outcome.
	 * 
	 * @param InPayload The payload to initialize with.
	 */
	static constexpr THarshOutcome<PayloadT>
	Success(const PayloadT& InPayload)
	{
		return THarshOutcome<PayloadT>(InPayload);
	}

	/**
	 * A constant denoting a successful skip of any actions.
	 * 
	 * @param InPayload The payload to initialize with.
	 */
	static constexpr THarshOutcome<PayloadT>
	Noop(const PayloadT& InPayload)
	{
		return THarshOutcome<PayloadT>(InPayload);
	}

	/**
	 * Convert to a status constant.
	 * 
	 * @return EApparatusStatus::Success Every harsh outcome
	 * is considered to be a success by default.
	 */
	constexpr EApparatusStatus
	ToStatus() const
	{
		return EApparatusStatus::Success;
	}

	/**
	 * Check if the outcome is a noop.
	 */
	constexpr bool
	IsNoop() const
	{
		return false;
	}

	/**
	 * Map the status from a status to a status.
	 * 
	 * @param From The status to map from.
	 * @param To The status to map to.
	 * @return Returns itself.
	 */
	THarshOutcome&
	MapStatus(const EApparatusStatus From, const EApparatusStatus To)
	{
		return *this;
	}

	/**
	 * Get the payload of the outcome.
	 */
	constexpr FORCEINLINE PayloadT
	GetPayload() const
	{
		return Payload;
	}

	/**
	 * Cast the payload statically.
	 * 
	 * @tparam The type to cast to.
	 */
	template < typename T >
	FORCEINLINE THarshOutcome<T>
	StaticCast()
	{
		return THarshOutcome<T>(static_cast<T>(Payload));
	}

	/**
	 * Dereference.
	 */
	FORCEINLINE THarshOutcome<PayloadT&>
	Dereference()
	{
		return THarshOutcome<PayloadT&>(Payload);
	}

	/**
	 * Convert to a successful status.
	 * 
	 * This operation should always be an explicit
	 * one for a harsh outcome, since
	 * the user should absolutely be aware
	 * of what he's actually doing.
	 */
	constexpr explicit
	operator EApparatusStatus() const
	{
		return ToStatus();
	}

	/**
	 * Convert to a payload.
	 * 
	 * This operator is an implicit one
	 * to be able to use the valuable outcome
	 * in place of a value itself.
	 */
	constexpr FORCEINLINE
	operator PayloadT() const
	{
		return GetPayload();
	}

#pragma region A Pointer-Like Interface

	/**
	 * A non-empty outcomes serves
	 * as a pointer to its payload by default.
	 */
	FORCEINLINE auto
	operator->() const
	{
		return const_cast<PayloadT*>(&Payload);
	}

	/**
	 * Get a constant reference to a payload.
	 */
	constexpr FORCEINLINE const PayloadT&
	operator*() const
	{
		return Payload;
	}

	/**
	 * Get a reference to a payload.
	 */
	constexpr FORCEINLINE PayloadT&
	operator*()
	{
		return Payload;
	}

#pragma region A Pointer-Like Interface

	/**
	 * Convert to successful state
	 * for using in a conditional expression.
	 * 
	 * This always return @c true, since every
	 * harsh result is already a success.
	 * 
	 * @return The state of examination.
	 */
	constexpr bool
	IsOK() const
	{
		return true;
	}

#pragma region Direct Payload Comparison

	FORCEINLINE bool
	operator==(const PayloadT& Other) const
	{
		return Payload == Other;
	}

	FORCEINLINE bool
	operator!=(const PayloadT& Other) const
	{
		return Payload != Other;
	}

#pragma endregion Direct Payload Comparison

	FORCEINLINE THarshOutcome&
	operator+=(const THarshOutcome&)
	{
		return *this;
	}

	FORCEINLINE THarshOutcome&
	operator+=(const EApparatusStatus InStatus)
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while accumulating with a harsh outcome: %s"),
			   *(::ToString(InStatus)));
		return *this;
	}

	FORCEINLINE THarshOutcome
	operator+(const EApparatusStatus InStatus) const
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while combining with a harsh outcome: %s"),
			   *(::ToString(InStatus)));
		return THarshOutcome();
	}

	FORCEINLINE FString
	ToString() const
	{
		return FString("<Harsh-No-Status>");
	}

	/**
	 * Initialize a harsh outcome using payload
	 * only.
	 * 
	 * @tparam InPayloadT The payload type.
	 * @param InPayload The payload to move.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	THarshOutcome(InPayloadT&& InPayload)
	  : Payload(std::forward<InPayloadT>(InPayload))
	{}

	/**
	 * Initialize a harsh outcome using a status and a default payload.
	 *
	 * @tparam InPayloadT The payload type.
	 * @param InStatus The status to initialize with.
	 * Must be a successful one.
	 */
	FORCEINLINE
	THarshOutcome(const EApparatusStatus InStatus)
	  : Payload()
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while converting to a harsh outcome: %s"),
			   *(::ToString(InStatus)));
	}

	/**
	 * Initialize a harsh outcome using a status and moving a payload.
	 *
	 * @tparam InPayloadT The payload type.
	 * @param InStatus The status to initialize with.
	 * Must be a successful one.
	 * @param InPayload The payload to move.
	 */
	template < typename InPayloadT >
	FORCEINLINE
	THarshOutcome(const EApparatusStatus InStatus, InPayloadT&& InPayload)
	  : Payload(std::forward<InPayloadT>(InPayload))
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while converting to a harsh outcome: %s"),
			   *(::ToString(InStatus)));
	}

	/**
	 * Convert from a polite outcome.
	 * 
	 * Will assert the outcome is a successful one.
	 * 
	 * @tparam InPayloadT The type of the payload carried
	 * by the passed-in outcome.
	 * @param InOutcome The polite outcome to convert from.
	 * Must be a successful one.
	 */
	template < typename InPayloadT >
	THarshOutcome(const TPoliteOutcome<InPayloadT>& InOutcome);

	/**
	 * Move a polite outcome to a harsh one.
	 * 
	 * Will assert the outcome is a successful one.
	 * 
	 * @tparam InPayloadT The type of the payload carried
	 * by the passed-in outcome.
	 * @param InOutcome The polite outcome to move.
	 * Must be a successful one.
	 */
	template < typename InPayloadT >
	THarshOutcome(TPoliteOutcome<InPayloadT>&& InOutcome);

	/**
	 * Initialize a harsh outcome using a harsh outcome
	 * and a payload override.
	 * 
	 * @tparam InAnyPayloadT The payload type of the incoming
	 * outcome. This is used for deduction only ignored.
	 * @param InOutcome The outcome to initialize from.
	 * This is basically ignored here.
	 * @param InPayload The payload override to initialize with.
	 */
	template < typename InAnyPayloadT, typename InPayloadT >
	constexpr FORCEINLINE
	THarshOutcome(const THarshOutcome<InAnyPayloadT>& InOutcome, InPayloadT&& InPayload)
	  : Payload(std::forward<InPayloadT>(InPayload))
	{}

}; //-struct THarshOutcome<PayloadT>

/**
 * Harsh outcome with a pointer-type outcome.
 * 
 * @tparam PayloadT The optional payload object to point.
 */
template < typename PayloadT >
struct THarshOutcome<PayloadT*>
  : public FCommonHarshOutcome
{
	/**
	 * The payload pointer type.
	 */
	using PayloadType = PayloadT*;

	/**
	 * The payload pointer of the outcome.
	 */
	PayloadT* Payload;

	/**
	 * A constant denoting a successful operation outcome.
	 */
	static constexpr THarshOutcome<PayloadT*>
	Success(TYPE_OF_NULLPTR)
	{
		return THarshOutcome<PayloadT*>(nullptr);
	}

	/**
	 * A constant denoting a successful skip of any actions.
	 */
	static constexpr THarshOutcome<PayloadT*>
	Noop(TYPE_OF_NULLPTR)
	{
		return THarshOutcome<PayloadT*>(nullptr);
	}

	/**
	 * A constant denoting a successful operation outcome.
	 */
	static constexpr THarshOutcome<PayloadT*>
	Success(PayloadT* const InPayload)
	{
		return THarshOutcome<PayloadT*>(InPayload);
	}

	/**
	 * A constant denoting a successful skip of any actions.
	 */
	static constexpr THarshOutcome<PayloadT*>
	Noop(PayloadT* const InPayload)
	{
		return THarshOutcome<PayloadT*>(InPayload);
	}

	/**
	 * Convert to a status constant.
	 * 
	 * @return EApparatusStatus::Success Every harsh outcome
	 * is considered to be a success by default.
	 */
	constexpr EApparatusStatus
	ToStatus() const
	{
		return EApparatusStatus::Success;
	}

	/**
	 * Check if the outcome is a noop.
	 */
	constexpr bool
	IsNoop() const
	{
		return false;
	}

	/**
	 * Map the status from a status to a status.
	 * 
	 * @param From The status to map from.
	 * @param To The status to map to.
	 * @return Returns itself.
	 */
	THarshOutcome&
	MapStatus(const EApparatusStatus From, const EApparatusStatus To)
	{
		return *this;
	}

	/**
	 * Get the payload of the outcome.
	 */
	FORCEINLINE PayloadType
	GetPayload() const
	{
		return Payload;
	}

	/**
	 * Convert to a successful status.
	 * 
	 * This operation should always be an explicit
	 * one for a harsh outcome, since
	 * the user should absolutely be aware
	 * of what he's actually doing.
	 */
	constexpr explicit
	operator EApparatusStatus() const
	{
		return ToStatus();
	}

	/**
	 * Convert to a payload.
	 */
	FORCEINLINE
	operator PayloadType() const
	{
		return GetPayload();
	}

	/**
	 * Cast the pointer statically.
	 * 
	 * @tparam The type of the pointer to cast to.
	 */
	template < typename T >
	FORCEINLINE THarshOutcome<T>
	StaticCast()
	{
		return THarshOutcome<T>(static_cast<T>(Payload));
	}

	/**
	 * Dereference.
	 */
	template < typename T = PayloadT, more::enable_if_t<!std::is_void<std::decay_t<T>>::value, bool> = true >
	FORCEINLINE THarshOutcome<T&>
	Dereference()
	{
		return THarshOutcome<T&>(*Payload);
	}

	/**
	 * Convert to successful state
	 * for using in a conditional expression.
	 * 
	 * This always return @c true, since every
	 * harsh result is already a success.
	 * 
	 * @return The state of examination.
	 */
	constexpr bool
	IsOK() const
	{
		return true;
	}

#pragma region Pointer-Based Operations

	template < typename T = PayloadT,
			   typename std::enable_if<!std::is_void<T>::value, int>::type = 0 >
	FORCEINLINE T*
	operator->() const
	{
		return Payload;
	}

	template < typename T = PayloadT,
			   typename std::enable_if<!std::is_void<T>::value, int>::type = 0 >
	FORCEINLINE T&
	operator*() const
	{
		return *Payload;
	}

	FORCEINLINE bool
	operator==(TYPE_OF_NULLPTR) const
	{
		return Payload == nullptr;
	}

	FORCEINLINE bool
	operator!=(TYPE_OF_NULLPTR) const
	{
		return Payload != nullptr;
	}

	FORCEINLINE bool
	operator==(PayloadT* const Other) const
	{
		return Payload == Other;
	}

	FORCEINLINE bool
	operator!=(PayloadT* const Other) const
	{
		return Payload != Other;
	}

	FORCEINLINE bool
	operator!() const
	{
		return Payload == nullptr;
	}

	FORCEINLINE
	operator bool() const
	{
		return Payload != nullptr;
	}

#pragma endregion Pointer-Based Operations

	FORCEINLINE THarshOutcome&
	operator+=(const THarshOutcome&)
	{
		return *this;
	}

	FORCEINLINE THarshOutcome&
	operator+=(const EApparatusStatus InStatus)
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while accumulating with a harsh outcome: %s"),
			   *(::ToString(InStatus)));
		return *this;
	}

	FORCEINLINE THarshOutcome
	operator+(const EApparatusStatus InStatus) const
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while combining with a harsh outcome: %s"),
			   *(::ToString(InStatus)));
		return THarshOutcome();
	}

	FORCEINLINE FString
	ToString() const
	{
		return FString::Format("<Harsh-Pointer-No-Status:{0}>", {Payload});
	}

	/**
	 * Initialize a default harsh outcome
	 * with a payload pointer to copy. 
	 */
	constexpr FORCEINLINE
	THarshOutcome()
	  : Payload()
	{}

	/**
	 * Initialize a harsh outcome
	 * with a payload pointer to copy. 
	 * 
	 * @tparam InPayloadT The type of the passed-in payload.
	 * Must be assignable to the destination pointer type.
	 * @param InPayload The pointer to copy.
	 */
	template < typename InPayloadT >
	FORCEINLINE
	THarshOutcome(InPayloadT&& InPayload)
	  : Payload(std::forward<InPayloadT>(InPayload))
	{}

	/**
	 * Initialize a harsh outcome with a
	 * status and a pointer to copy.
	 * 
	 * @tparam InPayloadT The type of the passed-in payload.
	 * Must be assignable to the destination pointer type.
	 * @param InStatus The status to initialize with.
	 * Must be a successful one.
	 * @param InPayload The pointer to copy.
	 * Isn't checked for being a @c nullptr.
	 */
	template < typename InPayloadT >
	FORCEINLINE
	THarshOutcome(const EApparatusStatus InStatus,
				  const InPayloadT       InPayload)
	  : Payload(InPayload)
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while converting to a harsh pointer outcome: %s"),
			   *(::ToString(InStatus)));
	}

	/**
	 * Initialize a harsh outcome with a
	 * status and a nullptr.
	 *
	 * @param InStatus The status to initialize with.
	 * Must be a successful one.
	 */
	FORCEINLINE
	THarshOutcome(const EApparatusStatus InStatus)
	  : Payload(nullptr)
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while converting to a harsh pointer outcome: %s"),
			   *(::ToString(InStatus)));
	}

	/**
	 * Convert from a polite outcome.
	 * 
	 * Will assert the outcome is a successful one.
	 * 
	 * @tparam InPayloadT The type of the passed-in payload
	 * pointer type.
	 * @param InOutcome The polite outcome to convert from.
	 */
	template < typename InPayloadT >
	THarshOutcome(const TPoliteOutcome<InPayloadT*>& InOutcome);

	/**
	 * Initialize a harsh outcome using a harsh outcome
	 * and a payload override.
	 * 
	 * @tparam AnyPayloadT The type of the payload of the incoming
	 * harsh outcome. Used for deduction only.
	 * @tparam InPayloadT The type of the payload to initialize with.
	 * @param InOutcome The outcome to initialize from. 
	 * This is basically ignored here.
	 * @param InPayload The payload pointer to initialize with.
	 */
	template < typename AnyPayloadT, typename InPayloadT >
	constexpr FORCEINLINE
	THarshOutcome(const THarshOutcome<AnyPayloadT>& InOutcome,
				  const InPayloadT                  InPayload)
	  : Payload(InPayload)
	{}

}; //-struct THarshOutcome<PayloadT*>

/**
 * A harsh outcome with no payload.  
 */
template < >
struct THarshOutcome<void>
  : public FCommonHarshOutcome
{
	/**
	 * The type of the payload, i.e. lack of it.
	 */
	using PayloadType = void;

	/**
	 * A constant denoting a successful operation outcome.
	 */
	static constexpr THarshOutcome<void>
	Success()
	{
		return THarshOutcome<void>();
	}

	/**
	 * A constant denoting a successful skip of any actions.
	 */
	static constexpr THarshOutcome<void>
	Noop()
	{
		return THarshOutcome<void>();
	}

	/**
	 * Convert to a status constant.
	 * 
	 * @return EApparatusStatus::Success Every harsh outcome
	 * is considered to be a success by default.
	 */
	constexpr EApparatusStatus
	ToStatus() const
	{
		return EApparatusStatus::Success;
	}

	/**
	 * Check if the outcome is a noop.
	 */
	constexpr bool
	IsNoop() const
	{
		return false;
	}

	/**
	 * Map the status from a status to a status.
	 * 
	 * @param From The status to map from.
	 * @param To The status to map to.
	 * @return Returns itself.
	 */
	THarshOutcome&
	MapStatus(const EApparatusStatus From, const EApparatusStatus To)
	{
		return *this;
	}

	/**
	 * Get the in-existent payload.
	 */
	constexpr FORCEINLINE void
	GetPayload() const
	{}

	/**
	 * Convert to a successful status.
	 * 
	 * This operation should always be an explicit
	 * one for a harsh outcome, since
	 * the user should absolutely be aware
	 * of what he's actually doing.
	 */
	constexpr explicit
	operator EApparatusStatus() const
	{
		return ToStatus();
	}

	/**
	 * Convert to successful state
	 * for using in a conditional expression.
	 * 
	 * This always return @c true, since every
	 * harsh result is already a success.
	 * 
	 * @return The state of examination.
	 */
	constexpr bool
	IsOK() const
	{
		return true;
	}

	FORCEINLINE THarshOutcome&
	operator+=(const THarshOutcome&)
	{
		return *this;
	}

	FORCEINLINE THarshOutcome&
	operator+=(const EApparatusStatus InStatus)
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while accumulating with a harsh outcome: %s"),
			   *(::ToString(InStatus)));
		return *this;
	}

	FORCEINLINE THarshOutcome
	operator+(const EApparatusStatus InStatus) const
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while combining with a harsh outcome: %s"),
			   *(::ToString(InStatus)));
		return THarshOutcome();
	}

	FORCEINLINE FString
	ToString() const
	{
		return FString("<Harsh-No-Status>");
	}

	constexpr FORCEINLINE
	THarshOutcome()
	{}

	/**
	 * Any payload received is considered
	 * to be a harsh success.
	 * 
	 * This basically means that an empty
	 * harsh outcome can be initialized
	 * with any type of value.
	 * 
	 * @tparam InPayloadT The type of the other payload.
	 * @param InPayload The payload itself.
	 * This is just being ignored.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	THarshOutcome(const InPayloadT& InPayload)
	{}

	/**
	 * Convert a status to a harsh outcome.
	 * 
	 * Any successful status is considered
	 * to be a valid harsh outcome.
	 * 
	 * @param InStatus The status to convert.
	 * Must be a successful one.
	 */
	FORCEINLINE
	THarshOutcome(const EApparatusStatus InStatus)
	{
		checkf(OK(InStatus),
			   TEXT("Erroneous status while converting to a harsh outcome: %s"),
			   *(::ToString(InStatus)));
	}

	/**
	 * Convert from a polite outcome.
	 * 
	 * Will assert the outcome is a successful one.
	 * Any incoming payload will be just ignored.
	 * 
	 * @tparam InPayloadT The type of the other payload.
	 * @param InOutcome The polite outcome to convert from.
	 * Checked to be a successful one.
	 */
	template < typename InPayloadT >
	THarshOutcome(const TPoliteOutcome<InPayloadT>& InOutcome);

}; //-struct THarshOutcome<void>

/**
 * Polite outcome with a status and an optional payload.
 * 
 * @tparam PayloadT The optional payload type.
 */
template < typename PayloadT >
struct TPoliteOutcome
  : public FCommonPoliteOutcome
{
	/**
	 * The type of the payload in the outcome.
	 */
	using PayloadType = PayloadT;

	/**
	 * The embedded status of the outcome.
	 */
	EApparatusStatus Status;

	/**
	 * The payload of the outcome.
	 */
	PayloadT Payload;

	/**
	 * A constant denoting a successful operation outcome.
	 */
	static constexpr TPoliteOutcome<PayloadT>
	Success(const PayloadT& InPayload)
	{
		return TPoliteOutcome<PayloadT>(EApparatusStatus::Success, InPayload);
	}

	/**
	 * A constant denoting a successful skip of any actions.
	 */
	static constexpr TPoliteOutcome<PayloadT>
	Noop(const PayloadT& InPayload)
	{
		return TPoliteOutcome<PayloadT>(EApparatusStatus::Noop, InPayload);
	}

	/**
	 * Convert to a status constant.
	 * 
	 * @return The embedded captured status.
	 */
	constexpr FORCEINLINE EApparatusStatus
	ToStatus() const
	{
		return Status;
	}

	/**
	 * Check if the outcome is a noop.
	 */
	constexpr bool
	IsNoop() const
	{
		return Status == EApparatusStatus::Noop;
	}

	/**
	 * Map the status from a status to a status.
	 * 
	 * @param From The status to map from.
	 * @param To The status to map to.
	 * @return Returns itself.
	 */
	TPoliteOutcome&
	MapStatus(const EApparatusStatus From, const EApparatusStatus To)
	{
		if (Status == From)
		{
			Status = To;
		}
		return *this;
	}

	/**
	 * Get the payload.
	 */
	FORCEINLINE PayloadT
	GetPayload() const
	{
		return Payload;
	}

#pragma region Pointer-Like Interface

	/**
	 * A non-empty outcomes serves
	 * as a pointer to its payload by default.
	 */
	FORCEINLINE auto
	operator->() const
	{
		return const_cast<PayloadT*>(&Payload);
	}

	/**
	 * Get a constant reference to a payload.
	 */
	constexpr FORCEINLINE const PayloadT&
	operator*() const
	{
		return Payload;
	}

	/**
	 * Get a reference to a payload.
	 */
	constexpr FORCEINLINE PayloadT&
	operator*()
	{
		return Payload;
	}

#pragma region Pointer-Like Interface

	/**
	 * Cast the payload statically.
	 * 
	 * @tparam The type to cast to.
	 */
	template < typename T >
	FORCEINLINE TPoliteOutcome<T>
	StaticCast()
	{
		return TPoliteOutcome<T>(Status, static_cast<T>(Payload));
	}

	/**
	 * Dereference.
	 */
	FORCEINLINE TPoliteOutcome<PayloadT&>
	Dereference()
	{
		return TPoliteOutcome<PayloadT&>(Status, Payload);
	}

	/**
	 * Convert to a status.
	 *
	 * This is an explicit conversion
	 * for a polite outcome.
	 *
	 * @return The embedded captured status.
	 */
	constexpr FORCEINLINE
	operator EApparatusStatus() const
	{
		return ToStatus();
	}

	/**
	 * Convert to a payload.
	 */
	FORCEINLINE
	operator PayloadT() const
	{
		return GetPayload();
	}

	/**
	 * Convert to success state
	 * for using in a conditional expression.
	 * 
	 * Returns @c true for successful statuses,
	 * @c false otherwise.
	 * 
	 * @return The state of examination.
	 */
	constexpr FORCEINLINE bool
	IsOK() const
	{
		return ::OK(ToStatus());
	}

#pragma region Direct Payload Comparison

	constexpr FORCEINLINE bool
	operator==(const PayloadT& Other) const
	{
		return Payload == Other;
	}

	constexpr FORCEINLINE bool
	operator!=(const PayloadT& Other) const
	{
		return Payload != Other;
	}

#pragma endregion Direct Payload Comparison

	/**
	 * Accumulate a harsh outcome with an optional payload.
	 * 
	 * This is semantically equivalent to accumulating a
	 * success status since any harsh outcome is already a success.
	 * 
	 * @tparam InPayloadT The payload of the harsh outcome.
	 * @return Returns itself as a result.
	 */
	template < typename InPayloadT >
	FORCEINLINE TPoliteOutcome&
	operator+=(const THarshOutcome<InPayloadT>&)
	{
		StatusAccumulate(Status, EApparatusStatus::Success);
		return *this;
	}

	/**
	 * Accumulate a status within the outcome.
	 * 
	 * @param InStatus The status to accumulate.
	 * @return Returns itself.
	 */
	FORCEINLINE TPoliteOutcome&
	operator+=(const EApparatusStatus InStatus)
	{
		StatusAccumulate(Status, InStatus);
		return *this;
	}

	constexpr FORCEINLINE TPoliteOutcome
	operator+(const EApparatusStatus InStatus) const
	{
		return TPoliteOutcome(StatusCombine(Status, InStatus));
	}

	FORCEINLINE FString
	ToString() const
	{
		return ::ToString(ToStatus());
	}

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Initialize a default polite outcome using a noop status and a default payload.
	 */
	constexpr FORCEINLINE
	TPoliteOutcome()
	  : Status(EApparatusStatus::Noop)
	  , Payload()
	{}

	/**
	 * Initialize a polite outcome with a payload only.
	 * 
	 * Initializing an outcome using only its payload
	 * is considered to be a valid, successful outcome.
	 * 
	 * @tparam InPayloadT The type of the payload to initialize with.
	 * @param InPayload The payload to initialize with.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(InPayloadT&& InPayload)
	  : Status(EApparatusStatus::Success)
	  , Payload(std::forward<InPayloadT>(InPayload))
	{}

	/**
	 * Initialize a polite outcome using a status and a default payload.
	 * 
	 * @tparam InPayloadT The type of the payload to override with.
	 * @param InStatus The status to initialize with.
	 */
	constexpr FORCEINLINE
	TPoliteOutcome(const EApparatusStatus InStatus)
	  : Status(InStatus)
	  , Payload()
	{}

	/**
	 * Initialize a polite outcome using a status and a payload.
	 * 
	 * @tparam InPayloadT The type of the payload to override with.
	 * @param InStatus The status to initialize from.
	 * @param InPayload The payload to initialize with.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const EApparatusStatus InStatus,
				   InPayloadT&&           InPayload)
	  : Status(InStatus)
	  , Payload(std::forward<InPayloadT>(InPayload))
	{}

	/**
	 * Initialize a polite outcome using a harsh outcome.
	 * Moving version.
	 * 
	 * @tparam InPayloadT The type of the payload to override with.
	 * @param InOutcome The outcome to initialize from,
	 * while moving its payload. 
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(THarshOutcome<InPayloadT>&& InOutcome)
	  : Status(EApparatusStatus::Success)
	  , Payload(MoveTempIfPossible(InOutcome.Payload))
	{}

	/**
	 * Initialize a polite outcome using a harsh outcome.
	 * 
	 * @tparam InPayloadT The type of the payload to override with.
	 * @param InOutcome The outcome to initialize from.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const THarshOutcome<InPayloadT>& InOutcome)
	  : Status(EApparatusStatus::Success)
	  , Payload(InOutcome.Payload)
	{}

	/**
	 * Initialize a copy of the polite outcome.
	 * Moving version.
	 * 
	 * @tparam InPayloadT The type of the payload to move.
	 * @param InOutcome The outcome to move. 
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(TPoliteOutcome<InPayloadT>&& InOutcome)
	  : Status(InOutcome.Status)
	  , Payload(MoveTempIfPossible(InOutcome.Payload))
	{}

	/**
	 * Initialize a copy of the polite outcome.
	 * 
	 * @tparam InPayloadT The type of the payload to copy.
	 * @param InOutcome The outcome to copy.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const TPoliteOutcome<InPayloadT>& InOutcome)
	  : Status(InOutcome.Status)
	  , Payload(InOutcome.Payload)
	{}

	/**
	 * Initialize a polite outcome using a harsh outcome
	 * and a payload override.
	 * 
	 * @tparam AnyPayloadT The type of the payload
	 * @tparam InPayloadT The type of the payload to override with.
	 * @param InOutcome The outcome to initialize from. 
	 * This is basically ignored here.
	 * @param InPayload The payload to initialize with.
	 */
	template < typename AnyPayloadT, typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const THarshOutcome<AnyPayloadT>& InOutcome,
				   InPayloadT&&                      InPayload)
	  : Status(EApparatusStatus::Success)
	  , Payload(std::forward<InPayloadT>(InPayload))
	{}

	/**
	 * Initialize a polite outcome using a polite outcome
	 * and a payload override.
	 * 
	 * @tparam AnyPayloadT The type of the payload
	 * @tparam InPayloadT The type of the payload to override with.
	 * @param InOutcome The outcome to initialize from. 
	 * This is basically ignored here.
	 * @param InPayload The payload to initialize with.
	 */
	template < typename AnyPayloadT, typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const TPoliteOutcome<AnyPayloadT>& InOutcome,
				   InPayloadT&&                       InPayload)
	  : Status(InOutcome.Status)
	  , Payload(std::forward<InPayloadT>(InPayload))
	{}

	/// @}
#pragma endregion Initialization
}; //-struct TPoliteOutcome<PayloadT>

/**
 * Polite outcome with a status and a pointer type result.
 * 
 * @tparam PayloadT The pointed payload type.
 */
template < typename PayloadT >
struct TPoliteOutcome<PayloadT*>
  : public FCommonPoliteOutcome
{
	/**
	 * The type of the payload pointer.
	 */
	using PayloadType = PayloadT*;

	/**
	 * The embedded status of the outcome.
	 */
	EApparatusStatus Status;

	/**
	 * The embedded payload pointer.
	 */
	PayloadType Payload;

	/**
	 * A constant denoting a successful operation outcome.
	 */
	static constexpr TPoliteOutcome<PayloadT*> Success(TYPE_OF_NULLPTR)
	{
		return TPoliteOutcome<PayloadT*>(EApparatusStatus::Success, nullptr);
	}

	/**
	 * A constant denoting a successful skip of any actions.
	 */
	static constexpr TPoliteOutcome<PayloadT*> Noop(TYPE_OF_NULLPTR)
	{
		return TPoliteOutcome<PayloadT*>(EApparatusStatus::Noop, nullptr);
	}

	/**
	 * A constant denoting a successful operation outcome.
	 */
	static constexpr TPoliteOutcome<PayloadT*> Success(PayloadT* const InPayload)
	{
		return TPoliteOutcome<PayloadT*>(EApparatusStatus::Success, InPayload);
	}

	/**
	 * A constant denoting a successful skip of any actions.
	 */
	static constexpr TPoliteOutcome<PayloadT*> Noop(PayloadT* const InPayload)
	{
		return TPoliteOutcome<PayloadT*>(EApparatusStatus::Noop, InPayload);
	}

	/**
	 * Convert to a status constant.
	 * 
	 * @return The embedded captured status.
	 */
	constexpr FORCEINLINE EApparatusStatus
	ToStatus() const
	{
		return Status;
	}

	/**
	 * Check if the outcome is a noop.
	 */
	constexpr bool
	IsNoop() const
	{
		return Status == EApparatusStatus::Noop;
	}

	/**
	 * Map the status from a status to a status.
	 * 
	 * @param From The status to map from.
	 * @param To The status to map to.
	 * @return Returns itself.
	 */
	TPoliteOutcome&
	MapStatus(const EApparatusStatus From, const EApparatusStatus To)
	{
		if (Status == From)
		{
			Status = To;
		}
		return *this;
	}

	/**
	 * Get the payload.
	 */
	FORCEINLINE PayloadType
	GetPayload() const
	{
		return Payload;
	}

	/**
	 * Cast the payload pointer statically.
	 * 
	 * @tparam The type to cast to.
	 */
	template < typename T >
	FORCEINLINE TPoliteOutcome<T>
	StaticCast()
	{
		return TPoliteOutcome<T>(Status, static_cast<T>(Payload));
	}

	/**
	 * Dereference.
	 */
	template < typename T = PayloadT, more::enable_if_t<!std::is_void<std::decay_t<T>>::value, bool> = true >
	FORCEINLINE TPoliteOutcome<T&>
	Dereference()
	{
		return TPoliteOutcome<T&>(Status, *Payload);
	}

	/**
	 * Convert to a status.
	 *
	 * This is an explicit conversion
	 * for a polite outcome.
	 *
	 * @return The embedded captured status.
	 */
	constexpr FORCEINLINE
	operator EApparatusStatus() const
	{
		return ToStatus();
	}

	/**
	 * Convert to a payload.
	 */
	FORCEINLINE
	operator PayloadType() const
	{
		return GetPayload();
	}

	/**
	 * Convert to success state
	 * for using in a conditional expression.
	 * 
	 * Returns @c true for successful statuses,
	 * @c false otherwise.
	 * 
	 * @return The state of examination.
	 */
	constexpr FORCEINLINE bool
	IsOK() const
	{
		return ::OK(ToStatus());
	}

	/**
	 * Accumulate a harsh outcome with an optional payload.
	 * 
	 * This is semantically equivalent to accumulating a
	 * success status since any harsh outcome is already a success.
	 * 
	 * @tparam InPayloadT The payload of the harsh outcome.
	 * @return Returns itself as a result.
	 */
	template < typename InPayloadT >
	FORCEINLINE TPoliteOutcome&
	operator+=(const THarshOutcome<InPayloadT>&)
	{
		::StatusAccumulate(Status, EApparatusStatus::Success);
		return *this;
	}

	/**
	 * Accumulate a status within the outcome.
	 * 
	 * @param InStatus The status to accumulate.
	 * @return Returns itself.
	 */
	FORCEINLINE TPoliteOutcome&
	operator+=(const EApparatusStatus InStatus)
	{
		::StatusAccumulate(Status, InStatus);
		return *this;
	}

	constexpr FORCEINLINE TPoliteOutcome
	operator+(const EApparatusStatus InStatus) const
	{
		return TPoliteOutcome(::StatusCombine(Status, InStatus));
	}

#pragma region Pointer-Based Operations

	template < typename T = PayloadT,
			   typename std::enable_if<!std::is_void<T>::value, int>::type = 0 >
	FORCEINLINE T*
	operator->() const
	{
		return Payload;
	}

	template < typename T = PayloadT,
			   typename std::enable_if<!std::is_void<T>::value, int>::type = 0 >
	FORCEINLINE T&
	operator*() const
	{
		return *Payload;
	}

	FORCEINLINE bool
	operator==(TYPE_OF_NULLPTR) const
	{
		return Payload == nullptr;
	}

	FORCEINLINE bool
	operator!=(TYPE_OF_NULLPTR) const
	{
		return Payload != nullptr;
	}

	FORCEINLINE bool
	operator==(PayloadT* const Other) const
	{
		return Payload == Other;
	}

	FORCEINLINE bool
	operator!=(PayloadT* const Other) const
	{
		return Payload != Other;
	}

	FORCEINLINE bool
	operator!() const
	{
		return Payload == nullptr;
	}

	FORCEINLINE
	operator bool() const
	{
		return Payload != nullptr;
	}

#pragma endregion Pointer-Based Operations

	FORCEINLINE FString
	ToString() const
	{
		return ::ToString(ToStatus());
	}

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Initialize a default polite pointer outcome using a noop status and a null pointer.
	 */
	constexpr FORCEINLINE
	TPoliteOutcome()
	  : Status(EApparatusStatus::Noop)
	  , Payload(nullptr)
	{}

	/**
	 * Initialize a polite pointer outcome using a payload only.
	 * 
	 * Any payload is considered to be a success.
	 * 
	 * @tparam InPayloadT The type of the payload to initialize with.
	 * @param InPayload The payload to initialize with.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const InPayloadT InPayload)
	  : Status(EApparatusStatus::Success)
	  , Payload(InPayload)
	{}

	/**
	 * Initialize a polite pointer outcome using a status and a null pointer.
	 * 
	 * @param InStatus The status to initialize with.
	 */
	constexpr FORCEINLINE
	TPoliteOutcome(const EApparatusStatus InStatus)
	  : Status(InStatus)
	  , Payload(nullptr)
	{}

	/**
	 * Initialize a polite pointer outcome using a status and a payload.
	 * 
	 * @tparam InPayloadT The type of the payload to initialize with.
	 * @param InStatus The status to initialize with.
	 * @param InPayload The payload to initialize with.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const EApparatusStatus InStatus,
				   const InPayloadT       InPayload)
	  : Status(InStatus)
	  , Payload(InPayload)
	{}

	/**
	 * Initialize a polite outcome using a harsh outcome.
	 * 
	 * Any harsh outcome is considered to be a success.
	 * 
	 * @tparam InPayloadT The type of the payload.
	 * @param InOutcome The outcome to initialize from. 
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const THarshOutcome<InPayloadT>& InOutcome)
	  : Status(EApparatusStatus::Success)
	  , Payload(InOutcome.Payload)
	{}

	/**
	 * Initialize a copy of the polite outcome.
	 * 
	 * @tparam InPayloadT The type of the payload.
	 * @param InOutcome The outcome to copy.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const TPoliteOutcome<InPayloadT>& InOutcome)
	  : Status(InOutcome.Status)
	  , Payload(InOutcome.Payload)
	{}

	/**
	 * Initialize a polite outcome using a harsh outcome
	 * and a payload override.
	 * 
	 * @tparam AnyPayloadT The type of the harsh outcome payload.
	 * Used for deduction only.
	 * @tparam InPayloadT The type of the payload override.
	 * @param InOutcome The outcome to initialize from. 
	 * This is basically ignored here.
	 * @param InPayload The payload pointer to initialize with.
	 */
	template < typename AnyPayloadT, typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const THarshOutcome<AnyPayloadT>& InOutcome,
				   InPayloadT* const                 InPayload)
	  : Status(EApparatusStatus::Success)
	  , Payload(InPayload)
	{}

	/**
	 * Initialize a polite outcome copy with
	 * a payload override.
	 * 
	 * @tparam AnyPayloadT The type of the polite outcome payload.
	 * Used for deduction only.
	 * @tparam InPayloadT The pointed type of the payload override.
	 * @param InOutcome The outcome to initialize from. 
	 * This is basically ignored here.
	 * @param InPayload The payload pointer to initialize with.
	 */
	template < typename AnyPayloadT, typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const TPoliteOutcome<AnyPayloadT>& InOutcome,
				   InPayloadT* const                  InPayload)
	  : Status(InOutcome.Status)
	  , Payload(InPayload)
	{}

	/// @}
#pragma endregion Initialization
}; //-struct TPoliteOutcome<PayloadT*>

/**
 * Polite outcome with a status but no payload.
 */
template < >
struct TPoliteOutcome<void>
  : public FCommonPoliteOutcome
{
	/**
	 * The type of the payload, i.e. lack of it.
	 */
	using PayloadType = void;

	/**
	 * The embedded status of the outcome.
	 */
	EApparatusStatus Status;

	/**
	 * A constant denoting a successful operation outcome.
	 */
	static constexpr TPoliteOutcome<void> Success()
	{
		return TPoliteOutcome<void>(EApparatusStatus::Success);
	} 

	/**
	 * A constant denoting a successful skip of any actions.
	 */
	static constexpr TPoliteOutcome<void> Noop()
	{
		return TPoliteOutcome<void>(EApparatusStatus::Noop);
	}

	/**
	 * Convert to a status constant.
	 * 
	 * @return The embedded captured status.
	 */
	constexpr FORCEINLINE EApparatusStatus
	ToStatus() const
	{
		return Status;
	}

	/**
	 * Check if the outcome is a noop.
	 */
	constexpr bool
	IsNoop() const
	{
		return Status == EApparatusStatus::Noop;
	}

	/**
	 * Map the status from a status to a status.
	 * 
	 * @param From The status to map from.
	 * @param To The status to map to.
	 * @return Returns itself.
	 */
	TPoliteOutcome&
	MapStatus(const EApparatusStatus From, const EApparatusStatus To)
	{
		if (Status == From)
		{
			Status = To;
		}
		return *this;
	}

	/**
	 * Get the non-existent payload.
	 */
	constexpr FORCEINLINE void
	GetPayload() const
	{}

	/**
	 * Convert to a status.
	 *
	 * This is an explicit conversion
	 * for a polite outcome.
	 *
	 * @return The embedded captured status.
	 */
	constexpr FORCEINLINE
	operator EApparatusStatus() const
	{
		return ToStatus();
	}

	/**
	 * Convert to success state
	 * for using in a conditional expression.
	 * 
	 * Returns @c true for successful statuses,
	 * @c false otherwise.
	 * 
	 * @return The state of examination.
	 */
	constexpr FORCEINLINE bool
	IsOK() const
	{
		return ::OK(ToStatus());
	}

	/**
	 * Accumulate a harsh outcome.
	 * 
	 * This is semantically identical to accumulating
	 * with a success status, since every harsh outcome
	 * is basically a non-fail.
	 * 
	 * @tparam OtherPayloadT The optional payload of the harsh outcome.
	 * @return Returns itself as a result.
	 */
	template < typename OtherPayloadT >
	FORCEINLINE TPoliteOutcome&
	operator+=(const THarshOutcome<OtherPayloadT>&)
	{
		::StatusAccumulate(Status, EApparatusStatus::Success);
		return *this;
	}

	FORCEINLINE TPoliteOutcome&
	operator+=(const EApparatusStatus InStatus)
	{
		::StatusAccumulate(Status, InStatus);
		return *this;
	}

	/**
	 * Accumulate a polite outcome with any payload.
	 * 
	 * @tparam AnyPayloadT The other payload to accumulate.
	 * @param InOutcome The outcome to combine with.
	 * @return Returns itself as a result.
	 */
	template < typename AnyPayloadT >
	FORCEINLINE TPoliteOutcome&
	operator+=(const TPoliteOutcome<AnyPayloadT>& InOutcome)
	{
		::StatusAccumulate(Status, InOutcome.Status);
		return *this;
	}

	constexpr FORCEINLINE TPoliteOutcome
	operator+(const EApparatusStatus InStatus) const
	{
		return TPoliteOutcome(::StatusCombine(Status, InStatus));
	}

	FORCEINLINE FString
	ToString() const
	{
		return ::ToString(ToStatus());
	}

	/**
	 * Initialize a default polite pointer outcome using a noop status.
	 */
	constexpr FORCEINLINE
	TPoliteOutcome()
	  : Status(EApparatusStatus::Noop)
	{}


	/**
	 * Initialize a polite outcome using a status.
	 * 
	 * @param InStatus The status to initialize with.
	 */
	constexpr FORCEINLINE
	TPoliteOutcome(const EApparatusStatus InStatus)
	  : Status(InStatus)
	{}

	/**
	 * Initialize using a status and any type of payload.
	 * 
	 * @tparam AnyPayloadT The payload to initialize from.
	 * This is used for deduction only and is basically ignored.
	 * @param InStatus The status to initialize with.
	 */
	template < typename AnyPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const EApparatusStatus InStatus, 
				   AnyPayloadT&&)
	  : Status(InStatus)
	{}

	/**
	 * Initialize a polite outcome with an ignored payload.
	 *
	 * Just a payload is considered to be a success.
	 * 
	 * @tparam AnyPayloadT The payload to initialize from.
	 * This is used for deduction only.
	 */
	template < typename AnyPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(AnyPayloadT&&)
	  : Status(EApparatusStatus::Success)
	{}

	/**
	 * Initialize a polite outcome with a harsh one.
	 *
	 * A harsh outcome with any payload
	 * is considered to be a success. 
	 * 
	 * @tparam InPayloadT The type of the other payload.
	 * This is used for deduction only.
	 * @param InOutcome The outcome to initialize from.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const THarshOutcome<InPayloadT>& InOutcome)
	  : Status(EApparatusStatus::Success)
	{}

	/**
	 * Initialize a copy of a polite outcome.
	 * 
	 * @tparam InPayloadT The type of the other payload.
	 * This is used for deduction only.
	 * @param InOutcome The polite outcome to copy.
	 */
	template < typename InPayloadT >
	constexpr FORCEINLINE
	TPoliteOutcome(const TPoliteOutcome<InPayloadT>& InOutcome)
	  : TPoliteOutcome(InOutcome.Status)
	{}

}; //-struct TPoliteOutcome<void>

template < typename PayloadT >
template < typename InPayloadT >
FORCEINLINE
THarshOutcome<PayloadT>::THarshOutcome(const TPoliteOutcome<InPayloadT>& InOutcome)
  : Payload(InOutcome.Payload)
{
	checkf(OK(InOutcome.ToStatus()),
		   TEXT("Erroneous status while converting to a harsh outcome: %s"),
		   *(InOutcome.ToString()));
}

template < typename PayloadT >
template < typename InPayloadT >
FORCEINLINE
THarshOutcome<PayloadT>::THarshOutcome(TPoliteOutcome<InPayloadT>&& InOutcome)
  : Payload(MoveTempIfPossible(InOutcome.Payload))
{
	checkf(OK(InOutcome.ToStatus()),
		   TEXT("Erroneous status while converting to a harsh outcome: %s"),
		   *(InOutcome.ToString()));
}

template < typename PayloadT >
template < typename InPayloadT >
FORCEINLINE
THarshOutcome<PayloadT*>::THarshOutcome(const TPoliteOutcome<InPayloadT*>& InOutcome)
  : Payload(InOutcome.Payload)
{
	checkf(OK(InOutcome.ToStatus()),
		   TEXT("Erroneous status while converting to a harsh outcome: %s"),
		   *(InOutcome.ToString()));
}

template < typename InPayloadT >
FORCEINLINE
THarshOutcome<void>::THarshOutcome(const TPoliteOutcome<InPayloadT>& InOutcome)
{
	checkf(OK(InOutcome.ToStatus()),
		   TEXT("Erroneous status while converting to a harsh outcome: %s"),
		   *(InOutcome.ToString()));
}

#pragma region General Outcome

/**
 * A general Apparatus function/method outcome.
 * 
 * The outcome is a special status delivery mechanism.
 * 
 * @note If the payload is specified (non-void), it will be used
 * as an outcome itself in case of a harsh paradigm.
 * 
 * @tparam Paradigm The paradigm type in question.
 * @tparam PayloadT The type of an optional payload to deliver.
 */
template < EParadigm Paradigm, typename PayloadT = void >
using TOutcome =
	typename std::conditional<
		std::is_void<PayloadT>::value,
		typename std::conditional<
			IsPolite(Paradigm),
			EApparatusStatus,
			THarshOutcome<void>>::type,
		typename std::conditional<
			IsPolite(Paradigm),
			TPoliteOutcome<PayloadT>,
			PayloadT>::type>::type;

/**
 * A portable Apparatus outcome used in virtual methods and 
 * other compatiblity layers.
 * 
 * @tparam PayloadT The type of an optional payload to deliver.
 */
template < typename PayloadT = void >
using TPortableOutcome = TOutcome<EParadigm::DefaultPortable, PayloadT>;

#pragma endregion General Outcome

/**
 * Confirm the harsh outcome to be a successful one.
 * 
 * Harsh outcomes are always considered to be successful.
 */
template < typename PayloadT >
constexpr FORCEINLINE bool
OK(const THarshOutcome<PayloadT>&)
{
	return true;
}

/**
 * Check if an outcome is be a successful one.
 */
template < typename PayloadT >
constexpr FORCEINLINE bool
OK(const TPoliteOutcome<PayloadT>& Outcome)
{
	return Outcome.IsOK();
}

/**
 * Check if all outcomes are OK.
 * Initializer list version. 
 */
template < typename PayloadT >
constexpr FORCEINLINE bool
OK(std::initializer_list<THarshOutcome<PayloadT>> Outcomes)
{
	return true;
}

/**
 * Check if all outcomes are OK.
 * Initializer list version. 
 */
template < typename PayloadT >
FORCEINLINE bool
OK(std::initializer_list<TPoliteOutcome<PayloadT>> Outcomes)
{
	for (const auto& Outcome : Outcomes)
	{
		if (!OK(Outcome))
		{
			return false;
		}
	}
	return true;
}

/**
 * Check if all outcomes are OK.
 * Array version.
 */
template < typename PayloadT, size_t N >
constexpr FORCEINLINE bool
OK(const THarshOutcome<PayloadT> (&Outcomes)[N])
{
	return true;
}

/**
 * Check if all outcomes are OK.
 * Array version.
 */
template < typename PayloadT, size_t N >
FORCEINLINE bool
OK(const TPoliteOutcome<PayloadT> (&Outcomes)[N])
{
	for (int32 i = 0; i < N; ++i)
	{
		if (!OK(Outcomes[i]))
		{
			return false;
		}
	}
	return true;
}

/**
 * Any other outcome is considered to be a payload
 * which is also always a valid success.
 * 
 * @tparam PayloadT The type of the payload to examine.
 */
template < typename PayloadT,
		   TPayloadTypeSecurity<std::decay_t<PayloadT>> = true >
constexpr FORCEINLINE bool
OK(PayloadT&&)
{
	return true;
}

/**
 * Confirm the harsh outcome to be a successful noop.
 * 
 * Harsh outcomes are never considered to be noops.
 */
template < typename PayloadT >
constexpr FORCEINLINE bool
IsNoop(const THarshOutcome<PayloadT>&)
{
	return false;
}

/**
 * Check if an outcome is be a successful one.
 */
template < typename PayloadT >
constexpr FORCEINLINE bool
IsNoop(const TPoliteOutcome<PayloadT>& Outcome)
{
	return Outcome.IsNoop();
}

/**
 * Any other outcome is considered to be a payload
 * which is never a noop.
 */
template < typename PayloadT,
		   TPayloadTypeSecurity<std::decay_t<PayloadT>> = true >
constexpr FORCEINLINE bool
IsNoop(PayloadT&&)
{
	return false;
}

#pragma region Universal Payload Getting

/**
 * Get an empty harsh outcome's payload in a universal manner.
 * 
 * @note Does nothing and returns nothing.
 */
FORCEINLINE void
GetPayload(const THarshOutcome<void>&)
{}

/**
 * Get an empty polite outcome's payload in a universal manner.
 * 
 * @note Does nothing and returns nothing.
 */
FORCEINLINE void
GetPayload(const TPoliteOutcome<void>&)
{}

/**
 * Get a harsh outcome's payload in a universal manner.
 */
template < typename PayloadT >
constexpr FORCEINLINE PayloadT
GetPayload(const THarshOutcome<PayloadT>& Outcome)
{
	return Outcome.GetPayload();
}

/**
 * Get a polite outcome's payload in a universal manner.
 */
template < typename PayloadT >
constexpr FORCEINLINE PayloadT
GetPayload(const TPoliteOutcome<PayloadT>& Outcome)
{
	return Outcome.GetPayload();
}

/**
 * Any other outcome is considered to be a payload
 * which is just returned.
 */
template < typename T,
		   more::enable_if_t<IsPayloadType<more::flatten_t<T>>(), int> = 0 >
constexpr FORCEINLINE bool
GetPayload(T&& Payload)
{
	return std::forward<T>(Payload);
}

#pragma endregion Universal Payload Getting

#pragma region Universal Outcome Making

// Polite version with a payload.
template < EParadigm Paradigm, typename PayloadT = void, bool bHarsh = IsHarsh(Paradigm) >
struct TOutcomeMaker
{
	template < EApparatusStatus Status, typename InPayloadT >
	static constexpr FORCEINLINE auto
	Do(InPayloadT&& Payload)
	{
		return TPoliteOutcome<PayloadT>(Status, std::forward<InPayloadT>(Payload));
	}

	template < typename InPayloadT >
	static constexpr FORCEINLINE auto
	Do(const EApparatusStatus Status, InPayloadT&& Payload)
	{
		return TPoliteOutcome<PayloadT>(Status, std::forward<InPayloadT>(Payload));
	}

	template < typename InPayloadT, typename AnyPayloadT >
	static constexpr FORCEINLINE auto
	Do(const THarshOutcome<AnyPayloadT>& Outcome, InPayloadT&& Payload)
	{
		return TPoliteOutcome<PayloadT>(Outcome.ToStatus(), std::forward<InPayloadT>(Payload));
	}

	template < typename InPayloadT, typename AnyPayloadT >
	static constexpr FORCEINLINE auto
	Do(const TPoliteOutcome<AnyPayloadT>& Outcome, InPayloadT&& Payload)
	{
		return TPoliteOutcome<PayloadT>(Outcome.ToStatus(), std::forward<InPayloadT>(Payload));
	}
}; //-struct TOutcomeMaker<Paradigm, PayloadT, /*bHarsh=*/false>

// Harsh version with a payload.
template < EParadigm Paradigm, typename PayloadT >
struct TOutcomeMaker<Paradigm, PayloadT, /*bHarsh=*/true>
{
	template < EApparatusStatus Status, bool bOK = OK(Status) >
	struct TStatusResolver
	{
		template < typename InPayloadT >
		static FORCEINLINE PayloadT
		Do(InPayloadT&& Payload)
		{
			check(OK(Status));
			return std::forward<InPayloadT>(Payload);
		}
	};

	// A successful resolved should guarantee compile-time static-ness.
	template < EApparatusStatus Status >
	struct TStatusResolver<Status, /*bOK=*/true>
	{
		template < typename InPayloadT >
		static constexpr FORCEINLINE PayloadT
		Do(InPayloadT&& Payload)
		{
			return std::forward<InPayloadT>(Payload);
		}
	};

	template < EApparatusStatus Status, typename InPayloadT >
	static constexpr FORCEINLINE PayloadT
	Do(InPayloadT&& Payload)
	{
		return TStatusResolver<Status>::template Do(std::forward<InPayloadT>(Payload));
	}

	template < typename InPayloadT >
	static FORCEINLINE PayloadT
	Do(const EApparatusStatus Status, InPayloadT&& Payload)
	{
		check(OK(Status));
		return std::forward<InPayloadT>(Payload);
	}

	template < typename InPayloadT, typename AnyPayloadT >
	static constexpr FORCEINLINE PayloadT
	Do(const THarshOutcome<AnyPayloadT>& Outcome, InPayloadT&& Payload)
	{
		return std::forward<InPayloadT>(Payload);
	}

	template < typename InPayloadT, typename AnyPayloadT >
	static FORCEINLINE PayloadT
	Do(const TPoliteOutcome<AnyPayloadT>& Outcome, InPayloadT&& Payload)
	{
		check(OK(Outcome));
		return std::forward<InPayloadT>(Payload);
	}
}; //-struct TOutcomeMaker<Paradigm, PayloadT, /*bHarsh*/true>

// Harsh payload-less version.
template < EParadigm Paradigm >
struct TOutcomeMaker<Paradigm, /*PayloadT=*/void, /*bHarsh=*/true>
{
	template < EApparatusStatus Status, bool bOK = OK(Status) >
	struct TStatusResolver
	{
		static FORCEINLINE auto
		Do()
		{
			check(OK(Status));
			return THarshOutcome<>();
		}
	};

	// A successful resolved should guarantee compile-time static-ness.
	template < EApparatusStatus Status >
	struct TStatusResolver<Status, /*bOK=*/true>
	{
		static constexpr FORCEINLINE auto
		Do()
		{
			return THarshOutcome<>();
		}
	};

	template < EApparatusStatus Status >
	static constexpr FORCEINLINE TOutcome<Paradigm>
	Do()
	{
		return TStatusResolver<Status>::template Do();
	}

	static constexpr FORCEINLINE TOutcome<Paradigm>
	Do(const EApparatusStatus Status)
	{
		return TOutcome<Paradigm>(Status);
	}

	template < typename InPayloadT >
	static constexpr FORCEINLINE TOutcome<Paradigm>
	Do(const THarshOutcome<InPayloadT>& Outcome)
	{
		return Outcome;
	}

	template < typename InPayloadT >
	static constexpr FORCEINLINE TOutcome<Paradigm>
	Do(const TPoliteOutcome<InPayloadT>& Outcome)
	{
		return Outcome;
	}
}; //-struct TOutcomeMaker<Paradigm, /*PayloadT=*/void, /*bHarsh=*/true>

// Polite payload-less version.
template < EParadigm Paradigm >
struct TOutcomeMaker<Paradigm, /*PayloadT=*/void, /*bHarsh=*/false>
{
	template < EApparatusStatus Status >
	static constexpr FORCEINLINE EApparatusStatus
	Do()
	{
		return Status;
	}

	static constexpr FORCEINLINE EApparatusStatus
	Do(const EApparatusStatus Status)
	{
		return Status;
	}

	template < typename InPayloadT >
	static constexpr FORCEINLINE EApparatusStatus
	Do(const THarshOutcome<InPayloadT>&)
	{
		return EApparatusStatus::Success;
	}

	template < typename InPayloadT >
	static constexpr FORCEINLINE EApparatusStatus
	Do(const TPoliteOutcome<InPayloadT>& Outcome)
	{
		return Outcome.Status;
	}
}; //-struct TOutcomeMaker<Paradigm, /*PayloadT=*/void, /*bHarsh=*/false>

/**
 * Make an empty outcome in a universal manner.
 * 
 * @tparam Paradigm The paradigm of the outcome to produce.
 * @tparam Status The compile-time status of the operation.
 * @param Payload The payload to use.
 * @return The resulting outcome.
 */
template < EParadigm Paradigm, EApparatusStatus Status >
static constexpr TOutcome<Paradigm>
MakeOutcome()
{
	return TOutcomeMaker<Paradigm>::template Do<Status>();
}

/**
 * Make an outcome in a universal manner while copying its payload.
 * 
 * @tparam Paradigm The paradigm of the outcome to produce.
 * @tparam PayloadT The type of the outcome payload.
 * @tparam Status The compile-time status of the operation.
 * @tparam InPayloadT The type of the passed-in payload.
 * @param Payload The payload to use.
 * @return The resulting outcome.
 */
template < EParadigm Paradigm, typename PayloadT, EApparatusStatus Status, typename InPayloadT,
		   TPayloadTypeSecurity<std::decay_t<InPayloadT>> = true >
static constexpr TOutcome<Paradigm, PayloadT>
MakeOutcome(InPayloadT&& Payload)
{
	return TOutcomeMaker<Paradigm, PayloadT>::template Do<Status>(std::forward<InPayloadT>(Payload));
}

/**
 * Make an outcome in a universal manner while copying its payload.
 * 
 * @tparam Paradigm The paradigm of the outcome to produce.
 * @tparam Status The compile-time status of the operation.
 * @tparam PayloadT The type of the outcome payload.
 * @tparam InPayloadT The type of the passed-in payload.
 * @param Payload The payload to use.
 * @return The resulting outcome.
 */
template < EParadigm Paradigm, EApparatusStatus Status, typename PayloadT, typename InPayloadT,
		   TPayloadTypeSecurity<std::decay_t<InPayloadT>> = true >
static constexpr TOutcome<Paradigm, PayloadT>
MakeOutcome(InPayloadT&& Payload)
{
	return MakeOutcome<Paradigm, PayloadT, Status>(std::forward<InPayloadT>(Payload));
}

/**
 * Make an outcome in a universal manner while copying its payload.
 * 
 * @tparam Paradigm The paradigm of the outcome to produce.
 * @tparam PayloadT The type of the outcome payload.
 * @tparam InPayloadT The type of the passed-in payload.
 * @param Status The status of the outcome.
 * @param Payload The payload to copy.
 * @return The resulting outcome.
 */
template < EParadigm Paradigm, typename PayloadT, typename InPayloadT >
static constexpr TOutcome<Paradigm, PayloadT>
MakeOutcome(const EApparatusStatus Status, InPayloadT&& Payload)
{
	return TOutcomeMaker<Paradigm, PayloadT>::Do(Status, std::forward<InPayloadT>(Payload));
}

/**
 * Make an outcome in a universal manner while moving its payload.
 * 
 * @tparam Paradigm The paradigm of the outcome to produce.
 * @tparam PayloadT The type of the overloaded payload.
 * @tparam InPayloadT The type of the passed-in payload.
 * @tparam AnyPayloadT The type of the payload to bypass. Ignored here.
 * @param Payload The payload to set.
 * @return The resulting outcome.
 */
template < EParadigm Paradigm, typename PayloadT,
		   typename InPayloadT, typename AnyPayloadT,
		   TPayloadTypeSecurity<std::decay_t<AnyPayloadT>> = true,
		   TPayloadTypeSecurity<std::decay_t<InPayloadT>> = true >
static constexpr TOutcome<Paradigm, PayloadT>
MakeOutcome(AnyPayloadT&&, InPayloadT&& Payload)
{
	return TOutcomeMaker<Paradigm, PayloadT>::template Do<EApparatusStatus::Success>(std::forward<InPayloadT>(Payload));
}

/**
 * Make an outcome in a universal manner while moving its payload.
 * 
 * @tparam Paradigm The paradigm of the outcome to produce.
 * @tparam PayloadT The type of the overloaded payload.
 * @tparam InPayloadT The type of the passed-in payload.
 * @tparam AnyPayloadT The type of the payload in the outcome. Ignored.
 * @param Outcome The outcome to bypass.
 * @param Payload The payload to move.
 * @return The resulting outcome.
 */
template < EParadigm Paradigm, typename PayloadT, typename InPayloadT, typename AnyPayloadT >
static constexpr TOutcome<Paradigm, PayloadT>
MakeOutcome(const THarshOutcome<AnyPayloadT>& Outcome, InPayloadT&& Payload)
{
	return TOutcomeMaker<Paradigm, PayloadT>::template Do(Outcome, std::forward<InPayloadT>(Payload));
}

/**
 * Make an outcome in a universal manner while moving its payload.
 * 
 * @tparam Paradigm The paradigm of the outcome to produce.
 * @tparam PayloadT The type of the overloaded payload.
 * @tparam InPayloadT The type of the passed-in payload.
 * @tparam AnyPayloadT The type of the payload in the outcome. Ignored.
 * @param Outcome The outcome to bypass.
 * @param Payload The payload to move.
 * @return The resulting outcome.
 */
template < EParadigm Paradigm, typename PayloadT, typename InPayloadT, typename AnyPayloadT >
static constexpr TOutcome<Paradigm, PayloadT>
MakeOutcome(const TPoliteOutcome<AnyPayloadT>& Outcome, InPayloadT&& Payload)
{
	return TOutcomeMaker<Paradigm, PayloadT>::Do(Outcome, std::forward<InPayloadT>(Payload));
}

/**
 * Make an outcome in a universal manner.
 * 
 * @tparam Paradigm The paradigm of the outcome to produce.
 * @param Status The status of the outcome.
 * @return The resulting outcome.
 */
template < EParadigm Paradigm >
static constexpr TOutcome<Paradigm>
MakeOutcome(const EApparatusStatus Status)
{
	return TOutcomeMaker<Paradigm>::Do(Status);
}

#pragma endregion Universal Outcome Making

#pragma region Outcome Guarantee

template < EParadigm Paradigm, typename L >
constexpr FORCEINLINE TOutcome<Paradigm, void>
ExecuteWithOutcome(const L& Lambda, void (L::*)(void) const)
{
	Lambda();
	return MakeOutcome<Paradigm, EApparatusStatus::Success>();
}

template < EParadigm Paradigm, typename L, typename PayloadT >
constexpr FORCEINLINE TOutcome<Paradigm, PayloadT>
ExecuteWithOutcome(const L& Lambda, THarshOutcome<PayloadT> (L::*)(void) const)
{
	return (TOutcome<Paradigm, PayloadT>)Lambda();
}

template < EParadigm Paradigm, typename L, typename PayloadT >
constexpr FORCEINLINE TOutcome<Paradigm, PayloadT>
ExecuteWithOutcome(const L& Lambda, TPoliteOutcome<PayloadT> (L::*)(void) const)
{
	return (TOutcome<Paradigm, PayloadT>)Lambda();
}

template < EParadigm Paradigm, typename L, typename PayloadT >
constexpr FORCEINLINE TOutcome<Paradigm, PayloadT>
ExecuteWithOutcome(const L& Lambda, PayloadT (L::*)(void) const)
{
	return MakeOutcome<Paradigm, PayloadT, EApparatusStatus::Success>(Lambda());
}

template < EParadigm Paradigm, typename L >
constexpr FORCEINLINE TOutcome<Paradigm>
ExecuteWithOutcome(const L& Lambda, EApparatusStatus (L::*)(void) const)
{
	return MakeOutcome<Paradigm>(Lambda());
}

template < EParadigm Paradigm, typename L >
constexpr FORCEINLINE auto
ExecuteWithOutcome(const L& Lambda)
{
	return ExecuteWithOutcome<Paradigm>(Lambda, &L::operator());
}

#pragma endregion Outcome Guarantee

#pragma region Universal Conversion to String

/**
 * Convert a harsh outcome to a string representation.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The textual representation of the outcome.
 */
template < typename PayloadT >
FORCEINLINE FString
ToString(const THarshOutcome<PayloadT>& Outcome)
{
	return Outcome.ToString();
}

/**
 * Convert a polite outcome to a string representation.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The textual representation of the outcome.
 */
template < typename PayloadT >
FORCEINLINE FString
ToString(const TPoliteOutcome<PayloadT>& Outcome)
{
	return Outcome.ToString();
}

/**
 * Convert a payload to a string representation.
 * 
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Payload The outcome to convert.
 * @return The textual representation of the outcome.
 */
template < typename PayloadT >
FORCEINLINE FString
ToString(const PayloadT& Payload)
{
	return FString(TEXT("<generic-payload-as-success>"));
}

#pragma endregion Universal Conversion to String

#pragma region Universal Static Cast

/**
 * Convert a harsh outcome to a status.
 * 
 * @tparam To The type to cast to.
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The successful status.
 */
template < typename To, typename PayloadT >
constexpr FORCEINLINE THarshOutcome<To>
OutcomeStaticCast(const THarshOutcome<PayloadT>& Outcome)
{
	return Outcome.template StaticCast<To>();
}

/**
 * Convert a polite outcome to a status.
 * 
 * @tparam To The type to cast to.
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The outcome status.
 */
template < typename To, typename PayloadT >
constexpr FORCEINLINE TPoliteOutcome<To>
OutcomeStaticCast(const TPoliteOutcome<PayloadT>& Outcome)
{
	return Outcome.template StaticCast<To>();
}

/**
 * Statically cast a generic payload pointer.
 * 
 * @tparam To The type to cast to.
 * @tparam PayloadT The type of the payload.
 * @param Payload The payload to cast.
 * @return The payload being cast.
 */
template < typename To, typename PayloadT >
constexpr FORCEINLINE auto
OutcomeStaticCast(PayloadT* const Payload)
{
	return static_cast<To>(Payload);
}

/**
 * Convert a harsh outcome to a status.
 * 
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The successful status.
 */
template < typename PayloadT >
FORCEINLINE PayloadT&
OutcomeDereference(const THarshOutcome<PayloadT*>& Outcome)
{
	return Outcome.template Dereference();
}

/**
 * Convert a polite outcome to a status.
 * 
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The outcome status.
 */
template < typename PayloadT >
FORCEINLINE PayloadT&
OutcomeDereference(const TPoliteOutcome<PayloadT*>& Outcome)
{
	return Outcome.template Dereference();
}

/**
 * Convert a harsh outcome to a status.
 * 
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The successful status.
 */
template < typename PayloadT >
FORCEINLINE PayloadT&
OutcomeDereference(const THarshOutcome<PayloadT>& Outcome)
{
	return Outcome.Dereference();
}

/**
 * Convert a polite outcome to a status.
 * 
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The outcome status.
 */
template < typename PayloadT >
FORCEINLINE PayloadT&
OutcomeDereference(const TPoliteOutcome<PayloadT>& Outcome)
{
	return Outcome.Dereference();
}

/**
 * Statically cast a generic payload pointer.
 * 
 * @tparam PayloadT The type of the payload.
 * @param Payload The payload to cast.
 * @return The payload being cast.
 */
template < typename PayloadT >
FORCEINLINE PayloadT&
OutcomeDereference(PayloadT& Payload)
{
	return Payload;
}

/**
 * Statically cast a generic payload pointer.
 * 
 * @tparam PayloadT The type of the payload.
 * @param Payload The payload to cast.
 * @return The payload being cast.
 */
template < typename PayloadT >
FORCEINLINE PayloadT&
OutcomeDereference(PayloadT* const Payload)
{
	return *Payload;
}

#pragma endregion Universal Static Cast

#pragma region Universal Conversion to Status

/**
 * Convert a harsh outcome to a status.
 * 
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The successful status.
 */
template < typename PayloadT >
constexpr FORCEINLINE EApparatusStatus
ToStatus(const THarshOutcome<PayloadT>& Outcome)
{
	return Outcome.ToStatus();
}

/**
 * Convert a polite outcome to a status.
 * 
 * @tparam PayloadT The type of the payload of the outcome.
 * @param Outcome The outcome to convert.
 * @return The outcome status.
 */
template < typename PayloadT >
constexpr FORCEINLINE EApparatusStatus
ToStatus(const TPoliteOutcome<PayloadT>& Outcome)
{
	return Outcome.ToStatus();
}

/**
 * Convert a generic payload to a status.
 * 
 * @tparam PayloadT The type of the payload.
 * @param Outcome The outcome to convert.
 * @return The successful status.
 */
template < typename PayloadT,
		   TPayloadTypeSecurity<more::flatten_t<PayloadT>> = true>
constexpr FORCEINLINE EApparatusStatus
ToStatus(PayloadT&& Outcome)
{
	return EApparatusStatus::Success;
}

#pragma endregion Universal Conversion to Status

#pragma region Outcome Combining
/// @name Outcome Combining
/// @{

template < typename... Ts >
struct TOutcomeCombiner
{};

template < typename T >
struct TOutcomeCombiner<T>
{
	using Type = T;
};

/**
 * Combine outcomes.
 * Zero arguments version.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @return The resulting outcome. Just calls the default constructor.
 */
template < EParadigm Paradigm = EParadigm::SafePolite >
constexpr FORCEINLINE TOutcome<Paradigm, void>
OutcomeCombine()
{
	return TOutcome<Paradigm, void>(EApparatusStatus::Noop);
}

#pragma region Status + Status

template < >
struct TOutcomeCombiner<EApparatusStatus, EApparatusStatus>
{
	using Type = EApparatusStatus;
};

/**
 * Combine a single status bypassing it.
 * 
 * @param Status The status to bypass.
 * @return The resulting status. Just passes by the argument.
 */
constexpr FORCEINLINE EApparatusStatus
OutcomeCombine(const EApparatusStatus Status)
{
	return Status;
}

/**
 * Combine two statuses as outcomes.
 * 
 * @param StatusA The first status to combine.
 * @param StatusB The second status to combine.
 * @return The resulting status combination.
 */
constexpr FORCEINLINE EApparatusStatus
OutcomeCombine(const EApparatusStatus StatusA, const EApparatusStatus StatusB)
{
	return StatusCombine(StatusA, StatusB);
}

#pragma endregion Status + Status

#pragma region Status + Payload

template < typename B >
struct TOutcomeCombiner<EApparatusStatus, B>
{
	using Type = TPoliteOutcome<B>;
};

/**
 * Combine a status and a payload into a filled polite outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam PayloadT The type of the payload to combine with.
 * @param Status The status to combine.
 * @param Payload The payload to combine with.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename PayloadT,
		   TPayloadTypeSecurity<std::decay_t<PayloadT>> = true >
constexpr FORCEINLINE auto
OutcomeCombine(const EApparatusStatus Status, PayloadT&& Payload)
{
	return MakeOutcome<Paradigm, std::decay_t<PayloadT>>(Status, std::forward<PayloadT>(Payload));
}

#pragma endregion Status + Payload

#pragma region Payload + Status

template < typename A >
struct TOutcomeCombiner<A, EApparatusStatus>
{
	using Type = TPoliteOutcome<A>;
};

/**
 * Combine a payload and a status into a polite non-empty outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam T The type of the payload.
 * @param Payload The payload to combine with.
 * @param Status The status to combine.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T,
		   TPayloadTypeSecurity<more::flatten_t<T>> = true >
constexpr auto
OutcomeCombine(T&& Payload, const EApparatusStatus Status)
{
	return MakeOutcome<Paradigm, more::flatten_t<T>>(Status, std::forward<T>(Payload));
}

#pragma endregion Payload + Status

#pragma region Status + Harsh

template < typename B >
struct TOutcomeCombiner<EApparatusStatus, THarshOutcome<B>>
{
	using Type = typename std::conditional<std::is_void<B>::value, EApparatusStatus, TPoliteOutcome<B>>;
};

/**
 * Combine into a polite outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @param Status The status to combine.
 * @param Outcome The payload to combine with.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T = void,
		   more::enable_if_t<!std::is_void<T>::value, int> = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const EApparatusStatus Status, THarshOutcome<T>&& Outcome)
{
	return MakeOutcome<Paradigm, T>(Status, MoveTempIfPossible(Outcome.Payload));
}

#ifndef DOXYGEN_ONLY

/**
 * Combine into a polite outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam T An empty harsh outcome type.
 * @param Status The status to combine.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T = void,
		   more::enable_if_t<std::is_same<T, more::flatten_t<THarshOutcome<void>>>::value, int> = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const EApparatusStatus Status, T&&)
{
	return Status;
}

#endif //!DOXYGEN_ONLY

#pragma endregion Status + Harsh

#pragma region Polite + Status

template < typename A >
struct TOutcomeCombiner<TPoliteOutcome<A>, EApparatusStatus>
{
	using Type = typename std::conditional<std::is_void<A>::value, EApparatusStatus, TPoliteOutcome<A>>;
};

/**
 * Combine into a polite outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @param Status The status to combine.
 * @param Outcome The payload to combine with.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T = void,
		   more::enable_if_t<!std::is_void<T>::value, int> = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(TPoliteOutcome<T>&& Outcome, const EApparatusStatus Status)
{
	return MakeOutcome<Paradigm, T>(StatusCombine(Status, Outcome.Status), MoveTempIfPossible(Outcome.Payload));
}

/**
 * Combine a non-empty polite outcome and a status into a non-empty polite outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @param Outcome The payload to combine with.
 * @param Status The status to combine.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T = void,
		   more::enable_if_t<!std::is_void<T>::value, bool> = true >
constexpr FORCEINLINE auto
OutcomeCombine(const TPoliteOutcome<T>& Outcome, const EApparatusStatus Status)
{
	return MakeOutcome<Paradigm, T>(StatusCombine(Status, Outcome.Status), Outcome.Payload);
}

#ifndef DOXYGEN_ONLY

/**
 * Combine an empty polite outcome and a status into a status.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam T The type of the empty polite outcome.
 * @param Outcome The payload to combine with.
 * @param Status The status to combine.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T = void,
		   more::enable_if_t<std::is_same<T, more::flatten_t<TPoliteOutcome<void> > >::value, bool> = true >
constexpr FORCEINLINE auto
OutcomeCombine(T&& Outcome, const EApparatusStatus Status)
{
	return StatusCombine(Status, Outcome.Status);
}

#endif // !DOXYGEN_ONLY

#pragma endregion Polite + Status

#pragma region Status + Polite

template < typename B >
struct TOutcomeCombiner<EApparatusStatus, TPoliteOutcome<B>>
{
	using Type = typename std::conditional<std::is_void<B>::value, EApparatusStatus, TPoliteOutcome<B>>;
};

/**
 * Combine into a polite outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @param Status The status to combine.
 * @param Outcome The payload to combine with.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T = void,
		   more::enable_if_t<!std::is_void<T>::value, int> = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const EApparatusStatus Status, TPoliteOutcome<T>&& Outcome)
{
	return MakeOutcome<Paradigm, T>(StatusCombine(Status, Outcome.Status), MoveTempIfPossible(Outcome.Payload));
}

/**
 * Combine into a polite outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @param Status The status to combine.
 * @param Outcome The payload to combine with.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T = void,
		   more::enable_if_t<!std::is_void<T>::value, int> = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const EApparatusStatus Status, const TPoliteOutcome<T>& Outcome)
{
	return MakeOutcome<Paradigm, T>(StatusCombine(Status, Outcome.Status), Outcome.Payload);
}

#ifndef DOXYGEN_ONLY

/**
 * Combine a status and an empty polite outcome into a status.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam T The empty polite outcome type.
 * @param Status The status to combine.
 * @param Outcome The outcome to combine with.
 * @return The resulting polite outcome.
 * Just combines the status and the payload into a polite outcome..
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename T = void,
		   more::enable_if_t<std::is_same<T, more::flatten_t<TPoliteOutcome<void>>>::value, bool> = true >
constexpr FORCEINLINE auto
OutcomeCombine(const EApparatusStatus Status, T&& Outcome)
{
	return StatusCombine(Status, Outcome.Status);
}

#endif //!DOXYGEN_ONLY

#pragma endregion Status + Polite

#pragma region Payload + Payload

template < typename A, typename B >
struct TOutcomeCombiner<A, B>
{
	using Type = typename std::conditional<std::is_void<B>::value, A, B>::type;
};

/**
 * Combine a single payload bypassing it.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam PayloadT The type of the payload to bypass.
 * @return The resulting outcome. Just passes by the argument.
 */
template < EParadigm Paradigm = EParadigm::Polite,
		   typename PayloadT = void,
		   TPayloadTypeSecurity<std::decay_t<PayloadT>> = true >
constexpr FORCEINLINE auto
OutcomeCombine(PayloadT&& Payload)
{
	return MakeOutcome<Paradigm, PayloadT, EApparatusStatus::Success>(std::forward<PayloadT>(Payload));
}

/**
 * Combine two payloads producing an outcome.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam PayloadT The type of the first payload to combine.
 * @tparam AnyPayloadT The type of the second payload to combine. Ignored.
 * @param Payload The second payload to combine.
 * @return The resulting outcome. Just passes by the second payload.
 */
template < EParadigm Paradigm = EParadigm::SafeHarsh,
		   typename PayloadT = void, typename AnyPayloadT = void,
		   TPayloadTypeSecurity<std::decay_t<AnyPayloadT>> = true,
		   TPayloadTypeSecurity<PayloadT> = true >
constexpr FORCEINLINE auto
OutcomeCombine(AnyPayloadT&&, const PayloadT& Payload)
{
	return MakeOutcome<Paradigm, PayloadT, EApparatusStatus::Success>(Payload);
}

/**
 * Combine two payloads producing an outcome.
 * Forwarding version.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam PayloadT The type of the first payload to combine.
 * @tparam AnyPayloadT The type of the second payload to combine. Ignored.
 * @param Payload The second payload to combine.
 * @return The resulting outcome. Just passes by the second payload.
 */
template < EParadigm Paradigm = EParadigm::SafeHarsh,
		   typename PayloadT = void, typename AnyPayloadT = void,
		   TPayloadTypeSecurity<std::decay_t<AnyPayloadT>> = true,
		   TPayloadTypeSecurity<std::decay_t<PayloadT>> = true >
constexpr FORCEINLINE auto
OutcomeCombine(AnyPayloadT&&, PayloadT&& Payload)
{
	return MakeOutcome<Paradigm, PayloadT, EApparatusStatus::Success>(std::forward<PayloadT>(Payload));
}

#pragma endregion Payload + Payload

#pragma region Payload + Harsh

template < typename A, typename B >
struct TOutcomeCombiner<A, THarshOutcome<B>>
{
	using Type = typename std::conditional<std::is_void<B>::value, A, THarshOutcome<B>>::type;
};

/**
 * Combine a payload with a harsh empty outcome producing a payload.
 * 
 * @tparam Paradigm The paradigm to work under.
 * @tparam PayloadT The first outcome payload type.
 * @tparam T Harsh outcome type.
 * @param Payload The first payload to combine.
 * @return The resulting payload.
 * Basically the same as the first argument.
 */
template < EParadigm Paradigm = EParadigm::SafeHarsh,
		   typename PayloadT, typename T,
		   more::enable_if_t<
		   		IsPayloadType<std::decay_t<PayloadT>>() &&
		   		std::is_same<std::decay_t<T>, THarshOutcome<void>>(), bool> = true >
constexpr FORCEINLINE auto
OutcomeCombine(PayloadT&& Payload, T&&)
{
	return std::forward<PayloadT>(Payload);
}

/**
 * Combine a payload with a harsh non-empty outcome.
 * Moving version.
 * 
 * @tparam PayloadT The type of the payload.
 * @tparam AnyPayloadT The first outcome payload type. Ignored.
 * @param Outcome The harsh outcome to combine with.
 * @return The resulting harsh outcome.
 * Basically returns the second argument.
 */
template < typename PayloadT,
		   typename AnyPayloadT,
		   more::enable_if_t<
				IsPayloadType<std::decay_t<AnyPayloadT>>() &&
				!std::is_void<PayloadT>::value, bool> = true >
constexpr FORCEINLINE auto
OutcomeCombine(AnyPayloadT&&, THarshOutcome<PayloadT>&& Outcome)
{
	return MoveTempIfPossible(Outcome);
}

/**
 * Combine a payload with a harsh non-empty one.
 * 
 * @tparam PayloadT The type of the payload to combine with.
 * @tparam AnyPayloadT The first outcome payload type. Ignored.
 * @param Outcome The second outcome to combine.
 * @return The resulting harsh outcome.
 * Basically returns the second argument.
 */
template < typename PayloadT,
		   typename AnyPayloadT,
		   more::enable_if_t<
				IsPayloadType<std::decay_t<AnyPayloadT>>() &&
				!std::is_void<PayloadT>::value, bool> = true >
constexpr FORCEINLINE auto
OutcomeCombine(AnyPayloadT&&, const THarshOutcome<PayloadT>& Outcome)
{
	return Outcome;
}

#pragma endregion Payload + Harsh

#pragma region Payload + Polite

template < typename A, typename B >
struct TOutcomeCombiner<A, TPoliteOutcome<B>>
{
	using Type = typename std::conditional<std::is_void<B>::value, TPoliteOutcome<A>, THarshOutcome<B>>::type;
};

/**
 * Combine a payload with a polite one.
 * 
 * @tparam APayloadT The first outcome payload type.
 * @param A The first payload to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically the same as the first argument,
 * since every harsh result is considered to be a success.
 */
template < typename APayloadT,
		   typename std::enable_if<IsPayloadType<more::flatten_t<APayloadT>>(), int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(APayloadT&& A, const TPoliteOutcome<void>& B)
{
	return TPoliteOutcome<APayloadT>(B.Status, std::forward<APayloadT>(A));
}

/**
 * Combine a payload with a polite non-empty one.
 * Moving version.
 * 
 * @tparam APayloadT The first outcome payload type.
 * @param A The first payload to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically returns the second argument.
 */
template < typename APayloadT,
		   typename BPayloadT,
		   typename std::enable_if<
				IsPayloadType<more::flatten_t<APayloadT>>() && 
				!std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(APayloadT&& A, TPoliteOutcome<BPayloadT>&& B)
{
	return B;
}

/**
 * Combine a payload with a polite non-empty one.
 * 
 * @tparam APayloadT The first outcome payload type.
 * @param A The first payload to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically returns the second argument.
 */
template < typename APayloadT,
		   typename BPayloadT,
		   typename std::enable_if<
				IsPayloadType<more::flatten_t<APayloadT>>() &&
				!std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(APayloadT&& A, const TPoliteOutcome<BPayloadT>& B)
{
	return B;
}

#pragma endregion Payload + Polite

#pragma region Harsh + Payload

template < typename A, typename B >
struct TOutcomeCombiner<THarshOutcome<A>, B>
{
	using Type = typename std::conditional<std::is_void<B>::value, THarshOutcome<A>, B>::type;
};

#ifndef DOXYGEN_ONLY

/**
 * Combine a harsh outcome with a payload.
 * 
 * @tparam T The harsh outcome type. Ignored but checked.
 * @tparam BPayloadT The second payload type.
 * @param A The first payload to combine.
 * @param B The payload to combine with.
 * @return The resulting harsh outcome.
 * Basically the same as the second argument.
 */
template < typename T,
		   typename BPayloadT,
		   typename std::enable_if<
				IsHarshOutcomeType<more::flatten_t<T>>() &&
				IsPayloadType<more::flatten_t<BPayloadT>>(), bool>::type = true >
constexpr FORCEINLINE auto
OutcomeCombine(T&&, BPayloadT&& B)
{
	return std::forward<BPayloadT>(B);
}

#endif //!DOXYGEN_ONLY

#pragma endregion Harsh + Payload

#pragma region Polite + Payload

template < typename A, typename B >
struct TOutcomeCombiner<TPoliteOutcome<A>, B>
{
	using Type = typename std::conditional<std::is_void<B>::value, TPoliteOutcome<A>, TPoliteOutcome<B>>::type;
};

#ifndef DOXYGEN_ONLY

/**
 * Combine a polite outcome with a payload.
 * 
 * @tparam T The first polite outcome type. Ignored but checked.
 * @tparam BPayloadT The second payload type.
 * @param A The first payload to combine.
 * @param B The payload to combine with.
 * @return The resulting polite outcome.
 * Basically the same as the second argument.
 */
template < typename T,
		   typename BPayloadT,
		   typename std::enable_if<
		   					IsPoliteOutcomeType<more::flatten_t<T>>() &&
							IsPayloadType<more::flatten_t<BPayloadT>>(), int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(T&& A, BPayloadT&& B)
{
	return TPoliteOutcome<BPayloadT>(A.Status, std::forward<BPayloadT>(B));
}

#endif //!DOXYGEN_ONLY

#pragma endregion Polite + Payload

#pragma region Harsh + Harsh

template < typename A, typename B >
struct TOutcomeCombiner<THarshOutcome<A>, THarshOutcome<B>>
{
	using Type = typename std::conditional<std::is_void<B>::value, THarshOutcome<A>, THarshOutcome<B>>::type;
};

/**
 * Bypass the harsh outcome.
 * Moving version.
 * 
 * @tparam PayloadT The payload type of the outcome.
 * @param Outcome The outcome to bypass.
 * @return The resulting harsh outcome.
 */
template < typename PayloadT >
constexpr FORCEINLINE auto
OutcomeCombine(THarshOutcome<PayloadT>&& Outcome)
{
	return Outcome;
}

/**
 * Bypass the harsh outcome.
 * 
 * @tparam PayloadT The payload type of the outcome.
 * @param Outcome The outcome to bypass.
 * @return The resulting harsh outcome.
 */
template < typename PayloadT >
constexpr FORCEINLINE auto
OutcomeCombine(const THarshOutcome<PayloadT>& Outcome)
{
	return Outcome;
}

/**
 * Combine two harsh outcomes into a new harsh outcome.
 * Moving the second argument version.
 * 
 * @tparam T The payload type of the first outcome.
 * @tparam BPayloadT The payload type of the second outcome.
 * @param A The first outcome to combine.
 * @param B The second outcome to combine.
 * @return The resulting harsh outcome.
 * Basically moves the second argument.
 */
template < typename T, typename BPayloadT,
		   typename std::enable_if<
				IsHarshOutcomeType<more::flatten_t<T>>() &&
				!std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(T&& A, THarshOutcome<BPayloadT>&& B)
{
	return B;
}

/**
 * Combine two harsh outcomes into a new harsh outcome.
 * 
 * @tparam T The payload type of the first outcome.
 * @tparam BPayloadT The payload type of the second outcome.
 * @param A The first outcome to combine.
 * @param B The second outcome to combine.
 * @return The resulting harsh outcome.
 * Basically returns the second argument.
 */
template < typename T, typename BPayloadT,
		   typename std::enable_if<
				IsHarshOutcomeType<more::flatten_t<T>>() &&
				!std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(T&& A, const THarshOutcome<BPayloadT>& B)
{
	return B;
}

/**
 * Combine two harsh outcomes with and without a payload into a new harsh outcome.
 * Copying the first argument version.
 * 
 * @tparam APayloadT The payload type of the first outcome.
 * @tparam T The type of the harsh outcome for universal referencing.
 * @param A The first outcome to combine with.
 * @param B The second outcome to combine.
 * This one is basically ignored here.
 * @return The resulting harsh outcome.
 * Basically returns the first argument.
 */
template < typename APayloadT, typename T,
		   typename std::enable_if<std::is_same<more::flatten_t<T>, THarshOutcome<void>>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const THarshOutcome<APayloadT>& A, T&& B)
{
	return A;
}

/**
 * Combine two harsh outcomes with and without a payload into a new harsh outcome.
 * Moving the first argument version.
 * 
 * @tparam APayloadT The payload type of the first outcome.
 * @tparam T The type of the harsh outcome for universal referencing.
 * @param A The first outcome to combine with.
 * @param B The second outcome to combine.
 * This one is basically ignored here.
 * @return The resulting harsh outcome.
 * Basically returns the first argument.
 */
template < typename APayloadT, typename T,
		   typename std::enable_if<std::is_same<more::flatten_t<T>, THarshOutcome<void>>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(THarshOutcome<APayloadT>&& A, T&& B)
{
	return A;
}

#pragma endregion Harsh + Harsh

#pragma region Harsh + Polite

template < typename A, typename B >
struct TOutcomeCombiner<THarshOutcome<A>, TPoliteOutcome<B>>
{
	using Type = typename std::conditional<std::is_void<B>::value, TPoliteOutcome<A>, TPoliteOutcome<B>>::type;
};

/**
 * Combine a harsh outcome with a polite one,
 * producing a polite outcome.
 * Moving the second argument version.
 * 
 * @tparam APayloadT The payload type of the first outcome.
 * This is ignored here and may be a @c void.
 * @tparam BPayloadT The payload type of the second outcome.
 * @param A The first outcome to combine while moving it.
 * Its payload is ignored completely while the status is used.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically returns the second argument.
 */
template < typename APayloadT, typename BPayloadT,
		   typename std::enable_if<std::is_void<APayloadT>::value || !std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const THarshOutcome<APayloadT>& A, TPoliteOutcome<BPayloadT>&& B)
{
	return B;
}

/**
 * Combine a harsh outcome with a polite one,
 * producing a polite outcome.
 * 
 * @tparam APayloadT The payload type of the first outcome.
 * Its payload is ignored. May be a @c void.
 * @tparam BPayloadT The payload type of the second outcome.
 * @param A The first outcome to combine with.
 * This is ignored here completely.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically returns the second argument.
 */
template < typename APayloadT, typename BPayloadT,
		   typename std::enable_if<std::is_void<APayloadT>::value || !std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const THarshOutcome<APayloadT>& A, const TPoliteOutcome<BPayloadT>& B)
{
	return B;
}

/**
 * Combine a non-empty harsh outcome with a polite empty one.
 * Moving the first payload version.
 * 
 * @tparam APayloadT The payload type of the first outcome.
 * May not be a @c void.
 * @param A The first outcome to combine with.
 * @param B The second outcome to combine.
 * This is basically what gets returned.
 * @return The resulting polite outcome
 * getting its status from the second argument.
 */
template < typename APayloadT,
		   typename std::enable_if<!std::is_void<APayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(THarshOutcome<APayloadT>&& A, TPoliteOutcome<void>&& B)
{
	return TPoliteOutcome<APayloadT>(B.Status, MoveTempIfPossible(A.Payload));
}

/**
 * Combine a non-empty harsh outcome with a polite empty one.
 * 
 * @tparam APayloadT The payload type of the first outcome.
 * May not be a @c void.
 * @param A The first outcome to combine with.
 * @param B The second outcome to combine.
 * This is basically what gets returned.
 * @return The resulting polite outcome
 * getting its status from the second argument.
 */
template < typename APayloadT,
		   typename std::enable_if<!std::is_void<APayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const THarshOutcome<APayloadT>& A, const TPoliteOutcome<void>& B)
{
	return TPoliteOutcome<APayloadT>(B.Status, A.Payload);
}

#pragma endregion Harsh + Polite

#pragma region Polite + Polite

template < typename A, typename B >
struct TOutcomeCombiner<TPoliteOutcome<A>, TPoliteOutcome<B>>
{
	using Type = typename std::conditional<std::is_void<B>::value, TPoliteOutcome<A>, TPoliteOutcome<B>>::type;
};

/**
 * Bypass the polite outcome.
 * Moving version.
 * 
 * @tparam PayloadT The payload type of the outcome.
 * @param Outcome The outcome to bypass.
 * @return The resulting polite outcome.
 */
template < typename PayloadT >
constexpr FORCEINLINE auto
OutcomeCombine(TPoliteOutcome<PayloadT>&& Outcome)
{
	return std::forward<PayloadT>(Outcome);
}

/**
 * Bypass the polite outcome.
 * 
 * @tparam PayloadT The payload type of the outcome.
 * @param Outcome The outcome to bypass.
 * @return The resulting polite outcome.
 */
template < typename PayloadT >
constexpr FORCEINLINE auto
OutcomeCombine(const TPoliteOutcome<PayloadT>& Outcome)
{
	return Outcome;
}

/**
 * Combine two polite outcomes into a new polite outcome
 * while moving the second outcome.
 * 
 * @tparam APayloadT The payload type of the first outcome.
 * @tparam BPayloadT The payload type of the second outcome.
 * @param A The first outcome to combine.
 * @param B The second outcome to combine while moving it.
 * @return The resulting polite outcome.
 */
template < typename APayloadT, typename BPayloadT,
		   typename std::enable_if<!std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const TPoliteOutcome<APayloadT>& A, TPoliteOutcome<BPayloadT>&& B)
{
	return TPoliteOutcome<BPayloadT>(::StatusCombine(A.Status, B.Status), MoveTempIfPossible(B.Payload));
}

/**
 * Combine two polite outcomes with payloads into a new polite outcome with a payload.
 * from the second argument.
 * 
 * @tparam APayloadT The payload type of the first outcome.
 * @tparam BPayloadT The payload type of the second outcome.
 * @param A The first outcome to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 */
template < typename APayloadT, typename BPayloadT,
		   typename std::enable_if<!std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const TPoliteOutcome<APayloadT>& A, const TPoliteOutcome<BPayloadT>& B)
{
	return TPoliteOutcome<BPayloadT>(::StatusCombine(A.Status, B.Status), B.Payload);
}

/**
 * Combine two polite outcomes into a new polite outcome
 * with a payload moved from the first argument.
 * 
 * @tparam APayloadT The payload type of the second outcome.
 * @param A The first outcome to combine while moving it.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 */
template < typename APayloadT,
		   typename std::enable_if<!std::is_void<APayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(TPoliteOutcome<APayloadT>&& A, const TPoliteOutcome<void>& B)
{
	return TPoliteOutcome<APayloadT>(::StatusCombine(A.Status, B.Status), MoveTempIfPossible(A.Payload));
}

/**
 * Combine two polite outcomes into a new polite outcome
 * with a payload copied from the first argument.
 * 
 * @tparam APayloadT The payload type of the second outcome.
 * @param A The first outcome to combine while copying its payload.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 */
template < typename APayloadT,
		   typename std::enable_if<!std::is_void<APayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const TPoliteOutcome<APayloadT>& A, const TPoliteOutcome<void>& B)
{
	return TPoliteOutcome<APayloadT>(::StatusCombine(A.Status, B.Status), A.Payload);
}

/**
 * Combine two empty polite outcomes into a new polite outcome.
 * Fully empty version.
 * 
 * @param A The first outcome to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * The statuses get combined.
 */
template < typename T1, typename T2,
		   typename std::enable_if<
		   					std::is_same<more::flatten_t<T1>, TPoliteOutcome<void>>::value &&
		   					std::is_same<more::flatten_t<T2>, TPoliteOutcome<void>>::value, int>::type = 0 >
constexpr FORCEINLINE TPoliteOutcome<void>
OutcomeCombine(T1&& A, T2&& B)
{
	return TPoliteOutcome<void>(::StatusCombine(A.Status, B.Status));
}

#pragma endregion Polite + Polite

#pragma region Polite + Harsh

template < typename A, typename B >
struct TOutcomeCombiner<TPoliteOutcome<A>, THarshOutcome<B>>
{
	using Type = typename std::conditional<std::is_void<B>::value, TPoliteOutcome<A>, TPoliteOutcome<B>>::type;
};

/**
 * Combine a polite outcome with a harsh one.
 * Moving version.
 * 
 * @tparam APayloadT The first outcome payload type.
 * @tparam BPayloadT The second outcome payload type.
 * May not be a @c void.
 * @param A The first outcome to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically the same as the first argument,
 * since every harsh result is considered to be a success.
 */
template < typename APayloadT, typename BPayloadT,
		   typename std::enable_if<!std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const TPoliteOutcome<APayloadT>& A, THarshOutcome<BPayloadT>&& B)
{
	return TPoliteOutcome<APayloadT>(A.Status, MoveTempIfPossible(B.Payload));
}

/**
 * Combine a polite outcome with a harsh one.
 * 
 * @tparam APayloadT The first outcome payload type.
 * @tparam BPayloadT The second outcome payload type.
 * May not be a @c void.
 * @param A The first outcome to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically the same as the first argument,
 * since every harsh result is considered to be a success.
 */
template < typename APayloadT, typename BPayloadT,
		   typename std::enable_if<!std::is_void<BPayloadT>::value, int>::type = 0 >
constexpr FORCEINLINE auto
OutcomeCombine(const TPoliteOutcome<APayloadT>& A, const THarshOutcome<BPayloadT>& B)
{
	return TPoliteOutcome<APayloadT>(A.Status, B.Payload);
}

/**
 * Combine a polite empty outcome with a harsh one.
 * Moving version.
 * 
 * @tparam BPayloadT The payload type of the second outcome.
 * @param A The first outcome to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically the same as the first argument,
 * since every harsh result is considered to be a success.
 */
template < typename APayloadT >
constexpr FORCEINLINE TPoliteOutcome<APayloadT>
OutcomeCombine(TPoliteOutcome<APayloadT>&& A, const THarshOutcome<void>& B)
{
	return A;
}

/**
 * Combine a polite empty outcome with a harsh one.
 * 
 * @tparam APayloadT The payload type of the second outcome.
 * @param A The first outcome to combine.
 * @param B The second outcome to combine.
 * @return The resulting polite outcome.
 * Basically the same as the first argument,
 * since every harsh result is considered to be a success.
 */
template < typename APayloadT >
constexpr FORCEINLINE auto
OutcomeCombine(const TPoliteOutcome<APayloadT>& A, const THarshOutcome<void>& B)
{
	return A;
}

#pragma endregion Polite + Harsh

#pragma region Multiple Arguments

template < typename A, typename B, typename... Ts >
struct TOutcomeCombiner<A, B, Ts...>
{
	using Type = typename TOutcomeCombiner<typename TOutcomeCombiner<A, B>::Type, Ts...>::Type;
};

/**
 * Combine multiple outcomes.
 * 
 * @note Arguments are evaluated in an arbitrary order in C++.
 * 
 * This method is recursive and also supports combining on payloads only.
 * 
 * @tparam OutcomeT1 The type of the first outcome to combine.
 * @tparam OutcomeT2 The type of the second outcome to combine.
 * @tparam OutcomeT3 The type of the third outcome to combine.
 * @tparam OtherOutcomeTs Other outcomes to combine.
 * @param Outcome1 The first outcome to combine.
 * @param Outcome2 The second outcome to combine.
 * @param Outcome3 The third outcome to combine.
 * @param OtherOutcomes Other outcomes to combine.
 * @return The resulting outcome.
 */
template < typename OutcomeT1, typename OutcomeT2, typename OutcomeT3, typename... OtherOutcomeTs >
constexpr FORCEINLINE typename TOutcomeCombiner<
	more::flatten_t<OutcomeT1>,
	more::flatten_t<OutcomeT2>,
	more::flatten_t<OutcomeT3>,
	more::flatten_t<OtherOutcomeTs>...>::Type
OutcomeCombine(OutcomeT1&& Outcome1, OutcomeT2&& Outcome2, OutcomeT3&& Outcome3, OtherOutcomeTs&&... OtherOutcomes)
{
	return OutcomeCombine(
			OutcomeCombine(std::forward<OutcomeT1>(Outcome1), std::forward<OutcomeT2>(Outcome2)),
			std::forward<OutcomeT3>(Outcome3),
			std::forward<OtherOutcomeTs>(OtherOutcomes)...);
}

#pragma region Array Combining

#pragma region Harsh Array Combining

/**
 * Combine several outcomes deriving a new one based on them.
 * Default outcome empty version.
 * 
 * @tparam N The number of elements in the array.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultOutcome The default outcome to be returned,
 * when no outcomes are in the array.
 */
template < size_t N >
constexpr FORCEINLINE THarshOutcome<void>
OutcomeCombine(const THarshOutcome<void> (&Outcomes)[N],
			   const THarshOutcome<void> DefaultOutcome = THarshOutcome<void>())
{
	return THarshOutcome<void>();
}

/**
 * Combine several outcomes deriving a new one based on them.
 * Default outcome version.
 * 
 * @tparam PayloadT The type of the payload in the outcomes.
 * @tparam N The number of elements in the array.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultOutcome The default outcome to be returned,
 * when no outcomes are in the array.
 */
template < typename PayloadT, size_t N >
FORCEINLINE THarshOutcome<PayloadT>
OutcomeCombine(const THarshOutcome<PayloadT> (&Outcomes)[N],
			   const THarshOutcome<PayloadT>& DefaultOutcome)
{
	return (N == 0) ? DefaultOutcome : Outcomes[N - 1];
}

/**
 * Combine several outcomes deriving a new one based on them.
 * Default outcome version.
 * 
 * @tparam N The number of elements in the array.
 * @tparam PayloadT The type of the payload in the outcomes.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultPayload The default payload to be returned,
 * when no outcomes are in the array.
 */
template < typename PayloadT, size_t N >
constexpr FORCEINLINE THarshOutcome<PayloadT>
OutcomeCombine(const THarshOutcome<PayloadT> (&Outcomes)[N],
			   const PayloadT&               DefaultPayload)
{
	return (N == 0) ? THarshOutcome<PayloadT>(DefaultPayload) : Outcomes[N - 1];
}

#pragma endregion Harsh Array Combining

#pragma region Polite Array Combining

/**
 * Combine several polite outcomes deriving a new one based on them.
 * Default outcome empty version.
 * 
 * @tparam N The number of elements in the array.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultOutcome The default outcome to be returned,
 * when no outcomes are in the array.
 * @return The combined outcome.
 */
template < size_t N >
constexpr FORCEINLINE EApparatusStatus
OutcomeCombine(const EApparatusStatus (&Outcomes)[N],
			   const EApparatusStatus DefaultOutcome = EApparatusStatus::Noop)
{
	if (N == 0) // Compile-time branch.
	{
		return DefaultOutcome;
	}
	auto Outcome = Outcomes[0];
	for (size_t i = 1; i < N; ++i)
	{
		Outcome = OutcomeCombine(Outcome, Outcomes[i]);
	}
	return Outcome;
}

/**
 * Combine several polite outcomes deriving a new one based on them.
 * Default outcome empty version.
 * 
 * @tparam N The number of elements in the array.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultOutcome The default outcome to be returned,
 * when no outcomes are in the array.
 * @return The combined outcome.
 */
template < size_t N >
constexpr FORCEINLINE TPoliteOutcome<void>
OutcomeCombine(const TPoliteOutcome<void> (&Outcomes)[N],
			   const TPoliteOutcome<void> DefaultOutcome = TPoliteOutcome<void>(EApparatusStatus::Noop))
{
	if (N == 0) // Compile-time branch.
	{
		return DefaultOutcome;
	}
	auto Outcome = Outcomes[0];
	for (size_t i = 1; i < N; ++i)
	{
		Outcome = OutcomeCombine(Outcome, Outcomes[i]);
	}
	return Outcome;
}

/**
 * Combine several polite outcomes deriving a new one based on them.
 * Default outcome version.
 * 
 * @tparam N The number of elements in the array.
 * @tparam PayloadT The type of the payload in the outcomes.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultOutcome The default outcome to be returned,
 * when no outcomes are in the array.
 */
template < typename PayloadT, size_t N >
FORCEINLINE TPoliteOutcome<PayloadT>
OutcomeCombine(const TPoliteOutcome<PayloadT> (&Outcomes)[N],
			   const TPoliteOutcome<PayloadT>& DefaultOutcome)
{
	if (N == 0) // Compile-time branch.
	{
		return DefaultOutcome;
	}
	auto Outcome = Outcomes[0];
	for (size_t i = 1; i < N; ++i) // Compile-time unwinding.
	{
		Outcome = OutcomeCombine(Outcome, Outcomes[i]);
	}
	return Outcome;
}

/**
 * Combine several polite outcomes deriving a new one based on them.
 * Default outcome version.
 * 
 * @tparam N The number of elements in the array.
 * @tparam PayloadT The type of the payload in the outcomes.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultPayload The default payload to be returned,
 * when no outcomes are in the array.
 */
template < typename PayloadT, size_t N >
FORCEINLINE TPoliteOutcome<PayloadT>
OutcomeCombine(const TPoliteOutcome<PayloadT> (&Outcomes)[N],
			   const PayloadT&               DefaultPayload)
{
	if (N == 0) // Compile-time branch.
	{
		return TPoliteOutcome<PayloadT>(DefaultPayload);
	}
	TPoliteOutcome<PayloadT> Outcome = Outcomes[0];
	for (size_t i = 1; i < N; ++i) // Compile-time unwinding.
	{
		Outcome = OutcomeCombine(Outcome, Outcomes[i]);
	}
	return Outcome;
}

#pragma endregion Polite Array Combining

#pragma endregion Array Combining

#pragma region Initializer List Combining

/**
 * Combine several outcomes deriving a new one based on them.
 * Initializer list version.
 * 
 * @tparam PayloadT The type of the payload in the outcomes.
 * @param Outcomes The array of outcomes to combine.
 * when no outcomes are in the array.
 * @return The resulting harsh outcome.
 */
FORCEINLINE THarshOutcome<void>
OutcomeCombine(std::initializer_list<THarshOutcome<void>> Outcomes)
{
	return THarshOutcome<void>();
}

/**
 * Combine several outcomes deriving a new one based on them.
 * Initializer list with a default payload version.
 * 
 * @tparam PayloadT The type of the payload in the outcomes.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultPayload The default payload value,
 * when no outcomes are in the list.
 * @return A combined outcome.
 */
template < typename PayloadT,
		   typename std::enable_if<IsPayloadType<typename more::flatten<PayloadT>::type>(), int>::type = 0 >
FORCEINLINE THarshOutcome<PayloadT>
OutcomeCombine(std::initializer_list<THarshOutcome<PayloadT>> Outcomes,
			   const PayloadT&                                DefaultPayload)
{
	auto Outcome = THarshOutcome<PayloadT>(EApparatusStatus::Noop, DefaultPayload);
	for (const auto& CurOutcome : Outcomes)
	{
		Outcome = OutcomeCombine(Outcome, CurOutcome);
	}
	return Outcome;
}

/**
 * Combine several outcomes deriving a new one based on them.
 * Initializer list with a default outcome version.
 * 
 * @tparam PayloadT The type of the payload in the outcomes.
 * @param Outcomes The array of outcomes to combine.
 * @param DefaultOutcome The default outcome,
 * when no outcomes are in the list.
 * @return 
 */
template < typename PayloadT >
FORCEINLINE THarshOutcome<PayloadT>
OutcomeCombine(std::initializer_list<THarshOutcome<PayloadT>> Outcomes,
			   const THarshOutcome<PayloadT>&                 DefaultOutcome)
{
	if (Outcomes.size() == 0)
	{
		return DefaultOutcome;
	}
	auto Outcome = DefaultOutcome;
	for (const auto& CurOutcome : Outcomes)
	{
		Outcome = OutcomeCombine(Outcome, CurOutcome);
	}
	return Outcome;
}

/**
 * Combine several polite outcomes deriving a new one based on them.
 * Initializer list version.
 * 
 * @tparam PayloadT The type of the payload in the outcomes.
 * @param Outcomes The array of outcomes to combine.
 * when no outcomes are in the array.
 * @param DefaultStatus The default status to use, when there are no
 * outcomes in the list.
 * @return The resulting harsh outcome.
 */
FORCEINLINE TPoliteOutcome<void>
OutcomeCombine(std::initializer_list<TPoliteOutcome<void>> Outcomes,
			   const EApparatusStatus                      DefaultStatus = EApparatusStatus::Noop)
{
	if (Outcomes.size() == 0)
	{
		return DefaultStatus;
	}
	auto Outcome = TPoliteOutcome<void>(DefaultStatus);
	for (const auto& CurOutcome : Outcomes)
	{
		Outcome = OutcomeCombine(Outcome, CurOutcome);
	}
	return Outcome;
}

/**
 * Combine several polite outcomes deriving a new one based on them.
 * Initializer list with a default outcome version.
 * 
 * @tparam PayloadT The type of the payload in the outcomes.
 * @param Outcomes The array of polite outcomes to combine.
 * @param DefaultOutcome The default outcome,
 * when no outcomes are in the list.
 * @return 
 */
template < typename PayloadT >
inline TPoliteOutcome<PayloadT>
OutcomeCombine(std::initializer_list<TPoliteOutcome<PayloadT>> Outcomes,
			   const TPoliteOutcome<PayloadT>&                 DefaultOutcome)
{
	if (Outcomes.size() == 0)
	{
		return DefaultOutcome;
	}
	auto Outcome = DefaultOutcome;
	for (const auto& CurOutcome : Outcomes)
	{
		Outcome = OutcomeCombine(Outcome, CurOutcome);
	}
	return Outcome;
}

#pragma endregion Initializer List Combining

#pragma endregion Multiple Arguments

/// @}
#pragma endregion Outcome Combining

/**
 * A general Apparatus function/method outcome
 * available if a condition is a succeeding one.
 * 
 * @tparam Paradigm The paradigm type in question.
 * @tparam Condition The condition to examine.
 * @tparam PayloadT The type of an optional payload to deliver.
 */
template < EParadigm Paradigm, bool Condition, typename PayloadT = void >
using TOutcomeIf = typename std::enable_if<Condition, TOutcome<Paradigm, PayloadT>>::type;

/**
 * A general Apparatus function/method outcome
 * available in an unsafe mode only.
 * 
 * @tparam Paradigm The paradigm type in question.
 * @tparam PayloadT The type of an optional payload to deliver.
 */
template < EParadigm Paradigm, typename PayloadT = void >
using TOutcomeIfUnsafe = TEnableIfUnsafe<Paradigm, TOutcome<Paradigm, PayloadT>>;

/**
 * A general Apparatus function/method outcome
 * available in an unsafe mode or under a specific condition.
 * 
 * @tparam Paradigm The paradigm type in question.
 * @tparam Condition Should the outcome be enabled still.
 * @tparam PayloadT The type of an optional payload to deliver.
 */
template < EParadigm Paradigm, bool Condition, typename PayloadT = void >
using TOutcomeIfUnsafeOr = TEnableIfUnsafeOr<Paradigm, Condition, TOutcome<Paradigm, PayloadT>>;

/**
 * Get the payload while ensuring
 * the status of the corresponding outcome is OK().
 * 
 * @param Outcome The outcome to ensure.
 */
#define EnsuredPayload(Outcome)                    \
	([&]()                                         \
	{                                              \
		const auto _Outcome##__LINE__ = (Outcome); \
		ensureMsgf(OK(_Outcome##__LINE__),         \
				   TEXT("Ensuring failed: %s"),    \
				   *ToString(_Outcome##__LINE__)); \
		return _Outcome##__LINE__.GetPayload();    \
	}())

/**
 * Get the payload while ensuring
 * the status is OK(). Formatting version.
 * 
 * @param Outcome The outcome to ensure.
 * @param Format The formatting message. Uses standard @c % formatting.
 * @param ... The arguments for the message being formatted.
 */
#define EnsuredPayloadf(Outcome, Format, ...)       \
	([&]()                                          \
	{                                               \
		const auto _Outcome##__LINE__ = (Outcome);  \
		ensureMsgf(OK(_Outcome##__LINE__),          \
				   Format,                          \
				   ##__VA_ARGS__);                  \
		return _Outcome##__LINE__.GetPayload();     \
	}())

/**
 * Ensure the outcome is OK().
 *
 * @param Outcome The outcome to ensure.
 */
#define EnsureOK(Outcome)                                  \
	([&]()                                                 \
	{                                                      \
		const auto _Outcome##__LINE__ = (Outcome);         \
		return ensureMsgf(OK(_Outcome##__LINE__),          \
						  TEXT("Ensuring has failed: %s"), \
						  *ToString(_Outcome##__LINE__));  \
	}())

/**
 * Verify the outcome is OK().
 *
 * @param Outcome The outcome to ensure.
 */
#define VerifyOK(Outcome)                                    \
	([&]()                                                   \
	{                                                        \
		const auto _Outcome##__LINE__ = (Outcome);           \
		verifyf(OK(_Outcome##__LINE__),                      \
				TEXT("Verifying has failed with error: %s"), \
				*(::ToString(_Outcome##__LINE__)));          \
	}())

/**
 * Avoid a certain event.
 * 
 * Performs a @c verify() and returns @c false, if the paradigm is harsh.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @return The state of assessment.
 */
#define Avoid(Paradigm, Expression) \
	[&]()                           \
	{                               \
		if (IsPolite(Paradigm))     \
		{                           \
			return Expression;      \
		}                           \
		else                        \
		{                           \
			verify(!(Expression));  \
			return false;           \
		}                           \
	}()

/**
 * Avoid a certain event.
 * 
 * Performs a @c verify() and returns @c false, if the paradigm is harsh.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @param Format The formatting message. Uses standard @c % formatting.
 * @param ... The arguments for the message being formatted.
 * @return The state of assessment.
 */
#define AvoidFormat(Paradigm, Expression, Format, ...)     \
	[&]()                                                  \
	{                                                      \
		if (IsPolite(Paradigm))                            \
		{                                                  \
			return Expression;                             \
		}                                                  \
		else                                               \
		{                                                  \
			verifyf(!(Expression), Format, ##__VA_ARGS__); \
			return false;                                  \
		}                                                  \
	}()

/**
 * Avoid a certain state.
 * 
 * Performs a @c check() and returns @c false, if the paradigm is harsh.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @return The state of assessment.
 */
#define AvoidCondition(Paradigm, Expression) \
	[&]()                                \
	{                                    \
		if (IsPolite(Paradigm))          \
		{                                \
			return Expression;           \
		}                                \
		else                             \
		{                                \
			check(!(Expression));        \
			return false;                \
		}                                \
	}()

/**
 * Avoid a certain state.
 * 
 * Performs a @c check() and returns @c false, if the paradigm is harsh.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @param Format The formatting message. Uses standard @c % formatting.
 * @param ... The arguments for the message being formatted.
 * @return The state of assessment.
 */
#define AvoidConditionFormat(Paradigm, Expression, Format, ...) \
	[&]()                                                   \
	{                                                       \
		if (IsPolite(Paradigm))                             \
		{                                                   \
			return Expression;                              \
		}                                                   \
		else                                                \
		{                                                   \
			checkf(!(Expression), Format, ##__VA_ARGS__);   \
			return false;                                   \
		}                                                   \
	}()

/**
 * The last caught Apparatus error.
 * 
 * Set in the Avoid* functions.
 */
static thread_local EApparatusStatus GApparatusLastError = EApparatusStatus::Noop;

/**
 * The main core-level Apparatus delegates.
 */
class APPARATUSRUNTIME_API FApparatusStatus
{
  public:

	/**
	 * Get the last caught error status.
	 */
	static FORCEINLINE EApparatusStatus
	GetLastError()
	{
		return GApparatusLastError;
	}
};

/**
 * Avoid an erroneous statement.
 * 
 * Performs a @c verify() and returns @c false, if the paradigm is harsh.
 * 
 * @warning This macro is only for Apparatus-compatible APIs.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @return The state of assessment.
 */
#define AvoidError(Paradigm, Expression)                                        \
	[&]()                                                                       \
	{                                                                           \
		if (IsPolite(Paradigm))                                                 \
		{                                                                       \
			const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]()  \
			{                                                                   \
				return Expression;                                              \
			});                                                                 \
			if (!OK(_Outcome##__LINE__))                                        \
			{                                                                   \
				GApparatusLastError = ToStatus(_Outcome##__LINE__);             \
				return true;                                                    \
			}                                                                   \
			return false;                                                       \
		}                                                                       \
		else                                                                    \
		{                                                                       \
			verify(OK(Expression));                                             \
			return false;                                                       \
		}                                                                       \
	}()

/**
 *  Avoid an erroneous statement with a formatting message.
 * 
 * Performs a @c verify() and returns @c false, if the paradigm is harsh.
 * 
 * @warning This macro is only for Apparatus-compatible APIs.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @param Format The formatting message. Uses standard @c % formatting.
 * @param ... The arguments for the message being formatted.
 * @return The state of assessment.
 */
#define AvoidErrorFormat(Paradigm, Expression, Format, ...)                     \
	[&]()                                                                       \
	{                                                                           \
		if (IsPolite(Paradigm))                                                 \
		{                                                                       \
			const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]()  \
			{                                                                   \
				return Expression;                                              \
			});                                                                 \
			if (!OK(_Outcome##__LINE__))                                        \
			{                                                                   \
				GApparatusLastError = ToStatus(_Outcome##__LINE__);             \
				return true;                                                    \
			}                                                                   \
			return false;                                                       \
		}                                                                       \
		else                                                                    \
		{                                                                       \
			verifyf(OK(Expression), Format, ##__VA_ARGS__);                     \
			return false;                                                       \
		}                                                                       \
	}()

/**
 * Avoid an erroneous condition.
 * 
 * Performs a @c check() and returns @c false, if the paradigm is harsh.
 * 
 * @warning This macro is only for Apparatus-compatible APIs.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @return The state of assessment.
 */
#define AvoidErrorCondition(Paradigm, Expression)                               \
	[&]()                                                                       \
	{                                                                           \
		if (IsPolite(Paradigm))                                                 \
		{                                                                       \
			const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]()  \
			{                                                                   \
				return Expression;                                              \
			});                                                                 \
			if (!OK(_Outcome##__LINE__))                                        \
			{                                                                   \
				GApparatusLastError = ToStatus(_Outcome##__LINE__);             \
				return true;                                                    \
			}                                                                   \
			return false;                                                       \
		}                                                                       \
		else                                                                    \
		{                                                                       \
			check(OK(Expression));                                              \
			return false;                                                       \
		}                                                                       \
	}()

/**
 * Avoid an erroneous condition with a formatting message.
 * 
 * Performs a @c check() and returns @c false, if the paradigm is harsh.
 * 
 * @warning This macro is only for Apparatus-compatible APIs.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @param Format The formatting message. Uses standard @c % formatting.
 * @param ... The arguments for the message being formatted.
 * @return The state of assessment.
 */
#define AvoidErrorConditionFormat(Paradigm, Expression, Format, ...)            \
	[&]()                                                                       \
	{                                                                           \
		if (IsPolite(Paradigm))                                                 \
		{                                                                       \
			const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]()  \
			{                                                                   \
				return Expression;                                              \
			});                                                                 \
			if (!OK(_Outcome##__LINE__))                                        \
			{                                                                   \
				GApparatusLastError = ToStatus(_Outcome##__LINE__);             \
				return true;                                                    \
			}                                                                   \
			return false;                                                       \
		}                                                                       \
		else                                                                    \
		{                                                                       \
			checkf(OK(Expression), Format, ##__VA_ARGS__);                      \
			return false;                                                       \
		}                                                                       \
	}()

/**
 * Make sure the statement is successful
 * or otherwise either return its resulting status
 * or just execute it.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 */
#define AssessOK(Paradigm, Expression)                                     \
	if (IsPolite(Paradigm))                                                \
	{                                                                      \
		const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]() \
		{                                                                  \
			return Expression;                                             \
		});                                                                \
		if (!OK(_Outcome##__LINE__))                                       \
		{                                                                  \
			return _Outcome##__LINE__;                                     \
		}                                                                  \
	}                                                                      \
	else                                                                   \
	{                                                                      \
		verify(OK(Expression));                                            \
	}

/**
 * Make sure the statement is successful
 * or otherwise either return its resulting status
 * or just execute it.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 * @param Finalize The statement to finalize with.
 */
#define AssessFinalizeOK(Paradigm, Expression, Finalize)                   \
	if (IsPolite(Paradigm))                                                \
	{                                                                      \
		const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]() \
		{                                                                  \
			return Expression;                                             \
		});                                                                \
		if (!OK(_Outcome##__LINE__))                                       \
		{                                                                  \
			Finalize;                                                      \
			return _Outcome##__LINE__;                                     \
		}                                                                  \
	}                                                                      \
	else                                                                   \
	{                                                                      \
		verify(OK(Expression));                                            \
	}

/**
 * Make sure the statement is successful
 * or otherwise either return its resulting status
 * or just execute it. Textural formatting version.
 * 
 * @param Paradigm A paradigm to assess under.
 * @param Expression The expression to assess.
 * @param Format The formatting message. Uses standard @c % formatting.
 * @param ... The arguments for the message being formatted.
 */
#define AssessOKFormat(Paradigm, Expression, Format, ...)                       \
	if (IsPolite(Paradigm))                                                \
	{                                                                      \
		const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]() \
		{                                                                  \
			return Expression;                                             \
		});                                                                \
		if (!OK(_Outcome##__LINE__))                                       \
		{                                                                  \
			return _Outcome##__LINE__;                                     \
		}                                                                  \
	}                                                                      \
	else                                                                   \
	{                                                                      \
		verifyf(OK(Expression), Format, ##__VA_ARGS__);                    \
	}

/**
 * Make sure the statement is successful
 * or otherwise either return its resulting status
 * or just execute it. Textural formatting version.
 * 
 * @param Paradigm A paradigm to assess under.
 * @param Expression The expression to assess.
 * @param Finalize The statement to finalize with.
 * @param Format The formatting message. Uses standard @c % formatting.
 * @param ... The arguments for the message being formatted.
 */
#define AssessFinalizeOKf(Paradigm, Expression, Finalize, Format, ...)     \
	if (IsPolite(Paradigm))                                                \
	{                                                                      \
		const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]() \
		{                                                                  \
			return Expression;                                             \
		});                                                                \
		if (!OK(_Outcome##__LINE__))                                       \
		{                                                                  \
			Finalize;                                                      \
			return _Outcome##__LINE__;                                     \
		}                                                                  \
	}                                                                      \
	else                                                                   \
	{                                                                      \
		verifyf(OK(Expression), Format, ##__VA_ARGS__);                    \
	}

/**
 * Make sure the statement is successful
 * or otherwise either return its resulting status
 * or just execute it.
 * 
 * @param Paradigm The paradigm to assess under.
 * @param Expression The expression to assess.
 * Guaranteed to be executed only once.
 */
#define AssessConditionOK(Paradigm, Expression)                            \
	if (IsPolite(Paradigm))                                                \
	{                                                                      \
		const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]() \
		{                                                                  \
			return Expression;                                             \
		});                                                                \
		if (!OK(_Outcome##__LINE__))                                       \
		{                                                                  \
			return _Outcome##__LINE__;                                     \
		}                                                                  \
	}                                                                      \
	else                                                                   \
	{                                                                      \
		check(OK(Expression));                                             \
	}

/**
 * Make sure the statement is successful
 * or otherwise either return its resulting status
 * or just execute it. Textural formatting version.
 * 
 * @param Paradigm A paradigm to assess under.
 * @param Expression The expression to assess.
 * @param Format The formatting message. Uses standard @c % formatting.
 * @param ... The arguments for the message being formatted.
 */
#define AssessConditionOKf(Paradigm, Expression, Format, ...)              \
	if (IsPolite(Paradigm))                                                \
	{                                                                      \
		const auto _Outcome##__LINE__ = ExecuteWithOutcome<Paradigm>([&]() \
		{                                                                  \
			return Expression;                                             \
		});                                                                \
		if (!OK(_Outcome##__LINE__))                                       \
		{                                                                  \
			return _Outcome##__LINE__;                                     \
		}                                                                  \
	}                                                                      \
	else                                                                   \
	{                                                                      \
		checkf(OK(Expression), Format, ##__VA_ARGS__);                     \
	}

/**
 * Stringify a symbol.
 */
#define APPARATUS_STRINGIFY(symbol) #symbol

/*
 * Internal macro to log a positive status.
 * Depends on the #APPARATUS_VERSBOSE definition.
 */
#ifdef APPARATUS_VERSBOSE

#define iAPPARATUS_REPORT_POSITIVE(Format, ...)                                \
			UE_LOG(LogApparatus,                                               \
				   Log,                                                        \
				   Format,                                                     \
				   ##__VA_ARGS__)

#else

#define iAPPARATUS_REPORT_POSITIVE(Format, ...)

#endif

/**
 * The main core-level Apparatus delegates.
 */
class APPARATUSRUNTIME_API FApparatusDelegates
{
  public:

	/**
	 * Standard error reporting operation using @c UE_LOG.
	 * 
	 * @param Status The status to report.
	 * @param Message The message to report.
	 */
	static FORCEINLINE void
	StandardReportError(EApparatusStatus Status, const FString& Message)
	{
		UE_LOG(LogApparatus, Error, TEXT("%s: %s"), *ToString(Status), *Message);
	}

	DECLARE_DELEGATE_TwoParams(FReportStatusDelegate, EApparatusStatus, const FString&);

	/**
	 * Called to report an error status.
	 */
	static FReportStatusDelegate ReportError;

  private:

	struct FInitializer
	{
		FORCEINLINE FInitializer()
		{
			if (!ReportError.IsBound())
			{
				ReportError.BindStatic(StandardReportError);
			}
		}
	};

	static FInitializer Initializer;

}; //-class FApparatusDelegates


/**
 * Report a status with a formatted message.
 * 
 * @param Status The status to report but without the <code>EApparatusStatus::</code> prefix.
 * @param Format The format string to report with.
 * @param ... The arguments to use within the format string.
 */
#define APPARATUS_REPORT(Status, Format, ...)                                  \
{                                                                              \
	if (!OK(EApparatusStatus::Status))                                         \
	{                                                                          \
		if (EApparatusStatus::Status < EApparatusStatus::FatalError)           \
		{                                                                      \
			UE_LOG(LogApparatus,                                               \
				   Fatal,                                                      \
				   APPARATUS_STRINGIFY(Status) ": " Format,                    \
				   ##__VA_ARGS__);                                             \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			FApparatusDelegates::ReportError.ExecuteIfBound(                   \
				EApparatusStatus::Status,                                      \
				FString::Printf(Format, ##__VA_ARGS__));                       \
		}                                                                      \
	}                                                                          \
	else                                                                       \
	{                                                                          \
		iAPPARATUS_REPORT_POSITIVE(APPARATUS_STRINGIFY(Status) ": " Format,    \
								   ##__VA_ARGS__);                             \
	}                                                                          \
}

/**
 * Report a status along with a message.
 * 
 * @param Status The status to report.
 * @param InMessage The message to report along with the status.
 * @return The same status as passed.
 */
FORCEINLINE EApparatusStatus
Report(const EApparatusStatus Status,
	   const TCHAR*           InMessage)
{
	if (!OK(Status))
	{
		if (Status < EApparatusStatus::FatalError)
		{
			UE_LOG(LogApparatus,
				   Fatal,
				   TEXT("Fatal: %s"),
				   InMessage);
		}
		else
		{
			FApparatusDelegates::ReportError.ExecuteIfBound(
				Status,
				FString(InMessage));
		}
	}
	else
	{
		iAPPARATUS_REPORT_POSITIVE("%s: %s",
								   *ToString(Status), InMessage);
	}
	return Status;
}

/**
 * Report a status with a formatted message.
 * 
 * @param Status The status to report.
 * @param InFormatString The format string to report with. Example: <code>"{0}/{1}: {2}"</code>
 * @param InOrderedArguments The arguments to use within the format string.
 * @return The same status as passed.
 */
FORCEINLINE EApparatusStatus
Report(const EApparatusStatus               Status,
	   const TCHAR*                         InFormatString,
	   const FStringFormatOrderedArguments& InOrderedArguments)
{
	if (!OK(Status))
	{
		if (Status < EApparatusStatus::FatalError)
		{
			UE_LOG(LogApparatus,
				   Fatal,
				   TEXT("Fatal: %s"),
				   *FString::Format(InFormatString, InOrderedArguments));
		}
		else
		{
			FApparatusDelegates::ReportError.ExecuteIfBound(
				Status,
				FString::Format(InFormatString, InOrderedArguments));
		}
	}
	else
	{
		iAPPARATUS_REPORT_POSITIVE("%s: %s",
								   *ToString(Status), *FString::Format(InFormatString, InOrderedArguments));
	}
	return Status;
}

/**
 * Report a status with a formatted message.
 * 
 * @param Status The status to report.
 * @param InFormatString The format string to report with. Example: <code>"{foo}/{bar}: {baz}"</code>
 * @param InNamedArguments The arguments to use within the format string.
 * @return The same status as passed.
 */
FORCEINLINE EApparatusStatus
Report(const EApparatusStatus             Status,
	   const TCHAR*                       InFormatString,
	   const FStringFormatNamedArguments& InNamedArguments)
{
	if (!OK(Status))
	{
		if (Status < EApparatusStatus::FatalError)
		{
			UE_LOG(LogApparatus,
				   Fatal,
				   TEXT("Fatal: %s"),
				   *FString::Format(InFormatString, InNamedArguments));
		}
		else
		{
			FApparatusDelegates::ReportError.ExecuteIfBound(
				Status,
				FString::Format(InFormatString, InNamedArguments));
		}
	}
	else
	{
		iAPPARATUS_REPORT_POSITIVE("%s: %s",
								   *ToString(Status), *FString::Format(InFormatString, InNamedArguments));
	}
	return Status;
}

/**
 * Report a status along with a message. Templated version.
 * 
 * @tparam Status The status to report.
 * @param InMessage The message to report along with the status.
 * @return The same status as passed.
 */
template < EApparatusStatus Status >
FORCEINLINE EApparatusStatus
Report(const TCHAR* InMessage)
{
	if (!OK(Status))
	{
		if (Status < EApparatusStatus::FatalError)
		{
			UE_LOG(LogApparatus,
				   Fatal,
				   TEXT("Fatal: %s"),
				   InMessage);
		}
		else
		{
			FApparatusDelegates::ReportError.ExecuteIfBound(
				Status,
				FString(InMessage));
		}
	}
	else
	{
		iAPPARATUS_REPORT_POSITIVE("%s: %s",
								   *ToString(Status), InMessage);
	}
	return Status;
}

/**
 * Report a status with a formatted message. Templated version.
 * 
 * @tparam Status The status to report.
 * @param InFormatString The format string to report with. Example: <code>"{0}/{1}: {2}"</code>
 * @param InOrderedArguments The arguments to use within the format string.
 * @return The same status as passed.
 */
template < EApparatusStatus Status >
FORCEINLINE EApparatusStatus
Report(const TCHAR*                         InFormatString,
	   const FStringFormatOrderedArguments& InOrderedArguments)
{
	if (!OK(Status))
	{
		if (Status < EApparatusStatus::FatalError)
		{
			UE_LOG(LogApparatus,
				   Fatal,
				   TEXT("Fatal: %s"),
				   *FString::Format(InFormatString, InOrderedArguments));
		}
		else
		{
			FApparatusDelegates::ReportError.ExecuteIfBound(
				Status,
				FString::Format(InFormatString, InOrderedArguments));
		}
	}
	else
	{
		iAPPARATUS_REPORT_POSITIVE("%s: %s",
								   *ToString(Status), *FString::Format(InFormatString, InOrderedArguments));
	}
	return Status;
}

/**
 * Report a status with a formatted message. Templated version.
 * 
 * @tparam Status The status to report.
 * @param InFormatString The format string to report with. Example: <code>"{foo}/{bar}: {baz}"</code>
 * @param InNamedArguments The arguments to use within the format string.
 * @return The same status as passed.
 */
template < EApparatusStatus Status >
FORCEINLINE EApparatusStatus
Report(const TCHAR*                       InFormatString,
	   const FStringFormatNamedArguments& InNamedArguments)
{
	if (!OK(Status))
	{
		if (Status < EApparatusStatus::FatalError)
		{
			UE_LOG(LogApparatus,
				   Fatal,
				   TEXT("Fatal: %s"),
				   *FString::Format(InFormatString, InNamedArguments));
		}
		else
		{
			FApparatusDelegates::ReportError.ExecuteIfBound(
				Status,
				FString::Format(InFormatString, InNamedArguments));
		}
	}
	else
	{
		iAPPARATUS_REPORT_POSITIVE("%s: %s",
								   *ToString(Status), *FString::Format(InFormatString, InNamedArguments));
	}
	return Status;
}

/**
 * Repeat the report got from a previously called function.
 */
#define APPARATUS_REPEAT_REPORT(Status, Format, ...)                           \
{                                                                              \
	if (!OK(Status))                                                           \
	{                                                                          \
		if (Status < EApparatusStatus::FatalError)                             \
		{                                                                      \
			UE_LOG(LogApparatus,                                               \
				   Fatal,                                                      \
				   TEXT("↳: ") Format,                                         \
				   ##__VA_ARGS__);                                             \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			UE_LOG(LogApparatus,                                               \
				   Error,                                                      \
				   TEXT("↳: ") Format,                                         \
				   ##__VA_ARGS__);                                             \
		}                                                                      \
	}                                                                          \
	else                                                                       \
	{                                                                          \
		iAPPARATUS_REPORT_POSITIVE(TEXT("↳: ") Format,                         \
								   ##__VA_ARGS__);                             \
	}                                                                          \
}

/**
 * Report a success status with a formatted message.
 */
#define APPARATUS_REPORT_SUCCESS(Format, ...)                                  \
{                                                                              \
	APPARATUS_REPORT(Success, Format, ##__VA_ARGS__);                          \
}
