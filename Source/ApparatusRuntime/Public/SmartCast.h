/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SmartCast.h
 * Created: 2022-04-12 21:13:14
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

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define SMART_CAST_H_SKIPPED_MACHINE_H
#endif

#include "Subjective.h"
#include "UnsafeSubjectHandle.h"

#ifdef SMART_CAST_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


/**
 * The smarter casting implementation.
 * 
 * @tparam Paradigm The paradigm to convert under.
 * @tparam From The type to cast from.
 * @tparam To The type to cast to.
 * @tparam DeferredCheck An argument type check to defer the evaluation.
 */
template < EParadigm Paradigm = EParadigm::Default,
		   typename From = void, typename To = void,
		   bool DeferredCheck = false >
struct TSmartCastImpl
{
	typedef From                              FromType;
	typedef typename more::flatten<To>::type* ToType;

	static FORCEINLINE ToType
	DoCast(From InObject)
	{
		// Use a standard Unreal cast by default:
		return Cast<typename more::flatten<To>::type>(InObject);
	}
};

#pragma region Same Types

template < EParadigm AnyParadigm, typename Same, bool DeferredCheck >
struct TSmartCastImpl<AnyParadigm, /*From=*/Same, /*To=*/Same, DeferredCheck>
{
	typedef Same FromType;
	typedef Same ToType;

	static FORCEINLINE Same
	DoCast(Same InObject)
	{
		// Return itself:
		return InObject;
	}
};

#pragma endregion Same Types

#pragma region Mutable to Immutable

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/ISubjective*, /*To=*/const ISubjective, DeferredCheck>
{
	typedef ISubjective*       FromType;
	typedef const ISubjective* ToType;

	static FORCEINLINE const ISubjective*
	DoCast(ISubjective* const InSubjective)
	{
		return const_cast<const ISubjective*>(InSubjective);
	}
};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/ISubjective*, /*To=*/const ISubjective*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/ISubjective*, /*To=*/const ISubjective, DeferredCheck>
{};

#pragma endregion Mutable to Immutable

#pragma region Immutable to Mutable

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Safe, /*From=*/const ISubjective*, /*To=*/ISubjective, DeferredCheck>
{
	static_assert(DeferredCheck, "Casting from a const subjective to a mutable one is not safe. Use an explicit unsafe cast, please.");
};

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Unsafe, /*From=*/const ISubjective*, /*To=*/ISubjective, DeferredCheck>
{
	typedef const ISubjective* FromType;
	typedef ISubjective*       ToType;

	static FORCEINLINE ToType
	DoCast(FromType const InSubjective)
	{
		return const_cast<ToType>(InSubjective);
	}
};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/const ISubjective*, /*To=*/ISubjective*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/const ISubjective*, /*To=*/ISubjective, DeferredCheck>
{};

#pragma endregion Immutable to Mutable

#pragma region Mutable to Solid

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/ISubjective*, /*To=*/ISolidSubjective, DeferredCheck>
{
	typedef ISubjective*      FromType;
	typedef ISolidSubjective* ToType;

	static FORCEINLINE ISolidSubjective*
	DoCast(ISubjective* const InSubjective)
	{
		return static_cast<ISolidSubjective*>(InSubjective);
	}
};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/ISubjective*, /*To=*/ISolidSubjective*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/ISubjective*, /*To=*/ISolidSubjective, DeferredCheck>
{};

#pragma endregion Mutable to Solid

#pragma region Immutable to Solid

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Safe, /*From=*/const ISubjective*, /*To=*/ISolidSubjective, DeferredCheck>
{
	static_assert(DeferredCheck, "Casting from a const subjective to a solid one is not safe. Use an explicit unsafe cast, please.");
};

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Unsafe, /*From=*/const ISubjective*, /*To=*/ISolidSubjective, DeferredCheck>
{
	typedef const ISubjective* FromType;
	typedef ISolidSubjective*  ToType;

	static FORCEINLINE ToType
	DoCast(ISubjective* const InSubjective)
	{
		return static_cast<ToType>(const_cast<ISubjective*>(InSubjective));
	}
};

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Internal, /*From=*/const ISubjective*, /*To=*/ISolidSubjective, DeferredCheck>
  : public TSmartCastImpl<EParadigm::Unsafe, /*From=*/const ISubjective*, /*To=*/ISolidSubjective, DeferredCheck>
{};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/const ISubjective*, /*To=*/ISolidSubjective*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/const ISubjective*, /*To=*/ISolidSubjective, DeferredCheck>
{};

#pragma endregion Immutable to Solid

#pragma region Solid to Immutable

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/ISolidSubjective*, /*To=*/const ISubjective, DeferredCheck>
{
	typedef ISolidSubjective*  FromType;
	typedef const ISubjective* ToType;

	static FORCEINLINE const ISubjective*
	DoCast(ISolidSubjective* const InSubjective)
	{
		return static_cast<const ISubjective*>(InSubjective);
	}
};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/ISolidSubjective*, /*To=*/const ISubjective*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/ISolidSubjective*, /*To=*/const ISubjective, DeferredCheck>
{};

#pragma endregion Solid to Immutable

#pragma region Solid to Mutable

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Safe, /*From=*/ISolidSubjective*, /*To=*/ISubjective, DeferredCheck>
{
	static_assert(DeferredCheck, "Casting from a solid subjective to a mutable one is not safe. Use an explicit unsafe cast, please.");
};

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Unsafe, /*From=*/ISolidSubjective*, /*To=*/ISubjective, DeferredCheck>
{
	typedef ISolidSubjective* FromType;
	typedef ISubjective*      ToType;

	static FORCEINLINE ToType
	DoCast(ISolidSubjective* const InSubjective)
	{
		return static_cast<ToType>(InSubjective);
	}
};

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Internal, /*From=*/ISolidSubjective*, /*To=*/ISubjective, DeferredCheck>
  : public TSmartCastImpl<EParadigm::Unsafe, /*From=*/ISolidSubjective*, /*To=*/ISubjective, DeferredCheck>
{};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/ISolidSubjective*, /*To=*/ISubjective*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/ISolidSubjective*, /*To=*/ISubjective, DeferredCheck>
{};

#pragma endregion Solid to Mutable

#pragma region Generic Mutable to Immutable

template < EParadigm Paradigm, class T, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/const T, DeferredCheck>
{
	typedef T*       FromType;
	typedef const T* ToType;

	static FORCEINLINE ToType
	DoCast(T* const InObject)
	{
		return InObject;
	}
};

template < EParadigm Paradigm, class T, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/const T*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/const T, DeferredCheck>
{};

#pragma endregion Generic Mutable to Immutable

#pragma region Generic Immutable to Mutable

template < class T, bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Safe, /*From=*/const T*, /*To=*/T, DeferredCheck>
{
	static_assert(DeferredCheck, "Casting from a const object to a mutable one is not safe. Use an explicit unsafe cast, please.");
};

template < class T, bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Unsafe, /*From=*/const T*, /*To=*/T, DeferredCheck>
{
	typedef const T* FromType;
	typedef T*       ToType;

	static FORCEINLINE ToType
	DoCast(const T* const InSubjective)
	{
		return const_cast<ToType>(InSubjective);
	}
};

template < class T, bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Internal, /*From=*/const T*, /*To=*/T, DeferredCheck>
  : public TSmartCastImpl<EParadigm::Unsafe, /*From=*/const T*, /*To=*/T, DeferredCheck>
{};

template < EParadigm Paradigm, class T, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/const T*, /*To=*/T*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/const T*, /*To=*/T, DeferredCheck>
{};

#pragma endregion Generic Immutable to Mutable

#pragma region Identical Types

template < EParadigm Paradigm, class T, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/T, DeferredCheck>
{
	typedef T* FromType;
	typedef T* ToType;

	static FORCEINLINE ToType
	DoCast(T* const InSubjective)
	{
		return InSubjective;
	}
};

template < EParadigm Paradigm, class T, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/T&, /*To=*/T&, DeferredCheck>
{
	typedef T& FromType;
	typedef T& ToType;

	static FORCEINLINE ToType
	DoCast(FromType InSubjective)
	{
		return InSubjective;
	}
};

template < EParadigm Paradigm, class T, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/T*, DeferredCheck>
  : TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/T, DeferredCheck>
{};

#pragma endregion Identical Types

#pragma region Casts to References

template < EParadigm Paradigm, class T, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/T&, DeferredCheck>
  : protected TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/T, DeferredCheck>
{
	typedef T* FromType;
	typedef T& ToType;
	typedef TSmartCastImpl<Paradigm, /*From=*/T*, /*To=*/T, DeferredCheck> Base;

	static FORCEINLINE ToType
	DoCast(T* const InSubjective)
	{
		return *Base::DoCast(InSubjective);
	}
};

#pragma endregion Casts to References

#pragma region Mutable Subject Handle To Immutable

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSubjectHandle, /*To=*/FConstSubjectHandle, DeferredCheck>
{
	typedef FSubjectHandle      FromType;
	typedef FConstSubjectHandle ToType;

	static FORCEINLINE ToType
	DoCast(const FSubjectHandle InHandle)
	{
		return InHandle;
	}
};

#pragma endregion Mutable Subject Handle To Immutable

#pragma region Mutable Subject Handle To Solid

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSubjectHandle, /*To=*/FSolidSubjectHandle, DeferredCheck>
{
	typedef FSubjectHandle      FromType;
	typedef FSolidSubjectHandle ToType;

	static FORCEINLINE ToType
	DoCast(const FSubjectHandle InHandle)
	{
		return InHandle;
	}
};

#pragma endregion Mutable Subject Handle To Solid

#pragma region Mutable Subject Handle To Unsafe

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Unsafe, /*From=*/FSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
{
	typedef FSubjectHandle       FromType;
	typedef FUnsafeSubjectHandle ToType;

	static FORCEINLINE ToType
	DoCast(const FSubjectHandle InHandle)
	{
		return (FUnsafeSubjectHandle)InHandle;
	}
};

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Internal, /*From=*/FSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
  : public TSmartCastImpl<EParadigm::Unsafe, /*From=*/FSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
{};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
{
	static_assert(DeferredCheck, "Only unsafe paradigm can cast from a mutable subject handle to an unsafe one.");
};

#pragma endregion Mutable Subject Handle To Unsafe

#pragma region Solid Subject Handle To Unsafe

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Unsafe, /*From=*/FSolidSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
{
	typedef FSolidSubjectHandle  FromType;
	typedef FUnsafeSubjectHandle ToType;

	static FORCEINLINE ToType
	DoCast(const FSolidSubjectHandle InHandle)
	{
		return (FUnsafeSubjectHandle)InHandle;
	}
};

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Internal, /*From=*/FSolidSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
  : public TSmartCastImpl<EParadigm::Unsafe, /*From=*/FSolidSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
{};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl< Paradigm, /*From=*/FSolidSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
{
	static_assert(DeferredCheck, "Only unsafe paradigm can cast from a solid subject handle to an unsafe one.");
};

#pragma endregion Solid Subject Handle To Unsafe

#pragma region Immutable Subject Handle To Unsafe

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Unsafe, /*From=*/FConstSubjectHandle,/*To=*/FUnsafeSubjectHandle, DeferredCheck>
{
	typedef FConstSubjectHandle  FromType;
	typedef FUnsafeSubjectHandle ToType;

	static FORCEINLINE ToType
	DoCast(const FConstSubjectHandle InHandle)
	{
		return (FUnsafeSubjectHandle)InHandle;
	}
};

template < bool DeferredCheck >
struct TSmartCastImpl<EParadigm::Internal, /*From=*/FConstSubjectHandle,/*To=*/FUnsafeSubjectHandle, DeferredCheck>
  : public TSmartCastImpl<EParadigm::Unsafe, /*From=*/FConstSubjectHandle,/*To=*/FUnsafeSubjectHandle, DeferredCheck>
{};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FConstSubjectHandle, /*To=*/FUnsafeSubjectHandle, DeferredCheck>
{
	static_assert(DeferredCheck, "Only unsafe paradigm can cast from a constant subject handle to an unsafe one.");
};

#pragma endregion Immutable Subject Handle To Unsafe

#pragma region Subject Handle To Subjective

#pragma region Mutable to Mutable

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSubjectHandle, /*To=*/ISubjective, DeferredCheck>
{
	typedef FSubjectHandle FromType;
	typedef ISubjective*   ToType;

	static FORCEINLINE ToType
	DoCast(const FSubjectHandle& InHandle)
	{
		return InHandle.GetSubjective();
	}
};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSubjectHandle, /*To=*/ISubjective*, DeferredCheck>
  : public TSmartCastImpl<Paradigm, /*From=*/FSubjectHandle, /*To=*/ISubjective, DeferredCheck>
{};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSubjectHandle, /*To=*/TScriptInterface<ISubjective>, DeferredCheck>
{
	typedef FSubjectHandle                FromType;
	typedef TScriptInterface<ISubjective> ToType;

	static FORCEINLINE ToType
	DoCast(const FSubjectHandle& InHandle)
	{
		return TScriptInterface<ISubjective>(Cast<UObject>(InHandle.GetSubjective()));
	}
};

#pragma endregion Mutable to Mutable

#pragma region Immutable to Immutable

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FConstSubjectHandle, /*To=*/const ISubjective, DeferredCheck>
{
	typedef FConstSubjectHandle FromType;
	typedef const ISubjective*  ToType;

	static FORCEINLINE ToType
	DoCast(const FConstSubjectHandle& InHandle)
	{
		return InHandle.GetSubjective();
	}
};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FConstSubjectHandle, /*To=*/const ISubjective*, DeferredCheck>
  : public TSmartCastImpl<Paradigm, /*From=*/FConstSubjectHandle, /*To=*/const ISubjective, DeferredCheck>
{};

#pragma endregion Immutable to Immutable

#pragma region Solid Handle To Solid Subjective

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSolidSubjectHandle, /*To=*/const ISolidSubjective, DeferredCheck>
{
	typedef FSolidSubjectHandle     FromType;
	typedef const ISolidSubjective* ToType;

	static FORCEINLINE ToType
	DoCast(const FSolidSubjectHandle& InHandle)
	{
		return InHandle.GetSubjective();
	}
};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSolidSubjectHandle, /*To=*/const ISolidSubjective*, DeferredCheck>
  : public TSmartCastImpl<Paradigm, /*From=*/FSolidSubjectHandle, /*To=*/const ISolidSubjective, DeferredCheck>
{};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSolidSubjectHandle, /*To=*/ISolidSubjective, DeferredCheck>
{
	typedef FSolidSubjectHandle FromType;
	typedef ISolidSubjective*   ToType;

	static FORCEINLINE ToType
	DoCast(const FSolidSubjectHandle& InHandle)
	{
		return InHandle.GetSubjective();
	}
};

template < EParadigm Paradigm, bool DeferredCheck >
struct TSmartCastImpl<Paradigm, /*From=*/FSolidSubjectHandle, /*To=*/ISolidSubjective*, DeferredCheck>
  : public TSmartCastImpl<Paradigm, /*From=*/FSolidSubjectHandle, /*To=*/ISolidSubjective, DeferredCheck>
{};

#pragma endregion Solid Handle To Solid Subjective

#pragma endregion Subject Handle To Subjective

/**
 * Cast the entity within paradigm.
 * UObject/Interface version.
 * 
 * Supports solidity casting.
 */
template < EParadigm Paradigm, class To = UObject*, class From = UObject*,
		   typename std::enable_if<!IsSubjectHandleType<typename more::flatten<From>::type>(), int>::type = 0 >
FORCEINLINE auto
SmartCast(From Src)
{
	return TSmartCastImpl<Paradigm, From, To>::DoCast(Src);
}

/**
 * Cast the entity within paradigm.
 * Subject handle reference to UObject version.
 * 
 * Supports solidity casting.
 */
template < EParadigm Paradigm, class To = UObject*, class From = FSubjectHandle,
		   typename std::enable_if<IsSubjectHandleType<From>(), int>::type = 0 >
FORCEINLINE auto
SmartCast(const From& Src)
{
	return TSmartCastImpl<Paradigm, From, To>::DoCast(Src);
}

/**
 * Cast the entity within paradigm.
 * Argument reordering version.
 * 
 * Supports solidity casting.
 */
template < class To, EParadigm Paradigm = EParadigm::Default, class From = UObject* >
FORCEINLINE auto
SmartCast(From Src)
{
	return TSmartCastImpl<Paradigm, From, To>::DoCast(Src);
}

/**
 * Cast the solidity state of an entity
 * while checking the result within paradigm.
 */
template < class To, class From, EParadigm Paradigm = EParadigm::Default >
FORCEINLINE typename TSmartCastImpl<Paradigm, From, To>::ToType
SmartCastChecked(From Src)
{
	const auto Result = SmartCast<Paradigm, To>(Src);
	check(Result);
	return Result;
}

/**
 * Cast the solidity state of an entity
 * while checking the result within paradigm.
 * Argument reordering version.
 */
template < EParadigm Paradigm, class To, class From >
FORCEINLINE typename TSmartCastImpl<Paradigm, From, To>::ToType
SmartCastChecked(From Src)
{
	const auto Result = SmartCast<Paradigm, To>(Src);
	check(Result);
	return Result;
}

/**
 * Get a subjective this handle is associated with (if any).
 */
FORCEINLINE FSolidSubjectHandle::SubjectivePtrType
FSolidSubjectHandle::GetSubjective() const
{
	return SmartCast<ISolidSubjective>(FCommonSubjectHandle::GetSubjective());
}
