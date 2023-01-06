/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: RunnableMechanic.h
 * Created: 2021-08-04 16:15:47
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
#include "UObject/Class.h"
#include "HAL/Runnable.h"


// Forward declarations:
template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
struct TChain;

/**
 * A base struct for all runnable mechanics.
 * 
 * You don't use this struct directly, but the templated
 * ::TRunnableMechanic instead.
 */
struct APPARATUSRUNTIME_API FRunnableMechanicBase
{
  private:

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	/**
	 * The runnable thread set during initialization.
	 */
	FRunnableThread* RunnableThread = nullptr;

  public:

	/**
	 * Get a thread this mechanic operates within.
	 * 
	 * Returns @c nullptr, if the mechanic is evaluating
	 * within the game thread.
	 */
	FORCEINLINE FRunnableThread*
	GetRunnableThread() const
	{
		return RunnableThread;
	}

}; // struct FRunnableMechanicBase

/**
 * Check if the supplied class actually a detail.
 * 
 * @tparam RunnableMechanicT A class to examine.
 */
template<class RunnableMechanicT>
constexpr bool IsRunnableMechanicClass()
{
	return std::is_base_of<FRunnableMechanicBase, RunnableMechanicT>::value;
}

/**
 * The components types storage.
 */
template < typename ...Ts >
struct TComponentTypesPack {};

/**
 * The base template for all object-based mechanics.
 * 
 * You can optionally use this template as a base for you custom mechanical
 * objects.
 * 
 * @tparam ChainT The type of chain to operate on.
 * @tparam Ts The types of components to operate on.
 */
template < typename ChainT, typename ...Ts >
struct TRunnableMechanic : public FRunnableMechanicBase
{
	/**
	 * The types of components used within the operation.
	 */
	using ComponentTypes = TComponentTypesPack<Ts...>;

	/**
	 * The type of cursor to operate with.
	 */
	using FCursor = typename ChainT::FCursor;

	/**
	 * Operate the current subject.
	 * 
	 * Implement in the descendants. Using an inline (<code>FORCEINLINE</code>)
	 * method is recommended, since the inlining-friendly context will
	 * be provided for the execution.
	 */
	virtual void operator()(const typename ChainT::FCursor&, Ts...) = 0;
};
