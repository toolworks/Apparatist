/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Mechanical.inl
 * Created: 2022-04-16 22:41:12
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

FORCEINLINE void
IMechanical::DoRegister()
{
	const auto Object = CastChecked<UObject>(this);
	if (UNLIKELY(Object->HasAnyFlags(RF_ClassDefaultObject)))
	{
		return;
	}
	auto* const World = Object->GetWorld();
	check(World);
	UMachine::ObtainMechanism(World)->RegisterMechanical(Object);
}

inline void
IMechanical::DoUnregister()
{
	if (LIKELY(UMachine::HasInstance()))
	{
		const auto Object = CastChecked<UObject>(this);
		auto* const World = Object->GetWorld();
		if (LIKELY(World))
		{
			UMachine::ObtainMechanism(World)->UnregisterMechanical(Object);
		}
	}
}

FORCEINLINE AMechanism*
IMechanical::GetMechanism() const
{
	UWorld* const World = CastChecked<UObject>(this)->GetWorld();
	if (LIKELY(World))
	{
		return UMachine::RetainInstance()->ObtainMechanism(World);
	}
	return nullptr;
}

FORCEINLINE FSubjectHandle
IMechanical::SpawnSubject(const EFlagmark Flagmark/*=FM_None*/)
{
	return GetMechanism()->SpawnSubject(Flagmark);
}

template < typename ChainT >
FORCEINLINE TSharedRef<ChainT>
IMechanical::Enchain(const FFilter& InFilter) const
{
	return GetMechanism()->template Enchain<ChainT>(InFilter);
}

FORCEINLINE TSharedRef<TChain<TChunkIt<FSolidSubjectHandle>, TBeltIt<FSolidSubjectHandle>>>
IMechanical::EnchainSolid(const FFilter& InFilter) const
{
	return Enchain<TChain<TChunkIt<FSolidSubjectHandle>, TBeltIt<FSolidSubjectHandle>>>(InFilter);
}

#pragma region Operating

template < typename ChainT, typename FilterT, typename MechanicT >
FORCEINLINE typename std::enable_if<!std::is_function<MechanicT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
IMechanical::Operate(FilterT&& Filter, const MechanicT& Mechanic)
{
	return GetMechanism()->template Operate<ChainT, FilterT, MechanicT>(std::forward<FilterT>(Filter), Mechanic);
}

template < typename ChainT, typename FilterT, typename MechanicT >
FORCEINLINE typename std::enable_if<!std::is_function<MechanicT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
IMechanical::Operate(FilterT&& Filter, MechanicT& Mechanic)
{
	return GetMechanism()->template Operate<ChainT, FilterT, MechanicT>(std::forward<FilterT>(Filter), Mechanic);
}

template < typename ChainT, typename FilterT, typename FunctionT >
FORCEINLINE typename std::enable_if<std::is_function<FunctionT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
IMechanical::Operate(FilterT&& Filter, FunctionT* const Mechanic)
{
	return GetMechanism()->template Operate<ChainT, FilterT, FunctionT>(std::forward<FilterT>(Filter), Mechanic);
}

template < typename ChainT, typename MechanicT >
FORCEINLINE typename std::enable_if<!std::is_function<MechanicT>::value, EApparatusStatus>::type
IMechanical::Operate(const MechanicT& Mechanic)
{
	return GetMechanism()->template Operate<ChainT, MechanicT>(Mechanic);
}

template < typename ChainT, typename MechanicT >
FORCEINLINE typename std::enable_if<!std::is_function<MechanicT>::value, EApparatusStatus>::type
IMechanical::Operate(MechanicT& Mechanic)
{
	return GetMechanism()->template Operate<ChainT, MechanicT>(Mechanic);
}

template < typename ChainT, typename FunctionT >
FORCEINLINE typename std::enable_if<std::is_function<FunctionT>::value, EApparatusStatus>::type
IMechanical::Operate(FunctionT* const Mechanic)
{
	return GetMechanism()->template Operate<ChainT, FunctionT>(Mechanic);
}

template < typename ChainT, EParadigm Paradigm, typename FilterT, typename MechanicT >
FORCEINLINE typename std::enable_if<!std::is_function<MechanicT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
IMechanical::OperateConcurrently(FilterT&&        Filter,
								 const MechanicT& Mechanic,
								 const int32      ThreadsCountMax,
								 const int32      SlotsPerThreadMin,
								 const bool       bSync)
{
	return GetMechanism()->template OperateConcurrently<ChainT, Paradigm, FilterT, MechanicT>(std::forward<FilterT>(Filter), Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
}

template < typename ChainT, EParadigm Paradigm, typename FilterT, typename MechanicT >
FORCEINLINE typename std::enable_if<!std::is_function<MechanicT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
IMechanical::OperateConcurrently(FilterT&&   Filter,
								 MechanicT&  Mechanic,
								 const int32 ThreadsCountMax,
								 const int32 SlotsPerThreadMin,
								 const bool  bSync)
{
	return GetMechanism()->template OperateConcurrently<ChainT, Paradigm, FilterT, MechanicT>(std::forward<FilterT>(Filter), Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
}

template < typename ChainT, EParadigm Paradigm, typename FilterT, typename FunctionT >
FORCEINLINE typename std::enable_if<std::is_function<FunctionT>::value && std::is_base_of<FFilterIndicator, FilterT>::value, EApparatusStatus>::type
IMechanical::OperateConcurrently(FilterT&&        Filter,
								 FunctionT* const Mechanic,
								 const int32      ThreadsCountMax,
								 const int32      SlotsPerThreadMin,
								 const bool       bSync)
{
	return GetMechanism()->template OperateConcurrently<ChainT, Paradigm, FilterT, FunctionT>(std::forward<FilterT>(Filter), Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
}

template < typename ChainT, EParadigm Paradigm, typename MechanicT >
FORCEINLINE typename std::enable_if<!std::is_function<MechanicT>::value, EApparatusStatus>::type
IMechanical::OperateConcurrently(const MechanicT& Mechanic,
								 const int32      ThreadsCountMax,
								 const int32      SlotsPerThreadMin,
								 const bool       bSync)
{
	return GetMechanism()->template OperateConcurrently<ChainT, Paradigm, MechanicT>(Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
}

template < typename ChainT, EParadigm Paradigm, typename MechanicT >
FORCEINLINE typename std::enable_if<!std::is_function<MechanicT>::value, EApparatusStatus>::type
IMechanical::OperateConcurrently(MechanicT&  Mechanic,
								 const int32 ThreadsCountMax,
								 const int32 SlotsPerThreadMin,
								 const bool  bSync)
{
	return GetMechanism()->template OperateConcurrently<ChainT, Paradigm, MechanicT>(Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
}

template < typename ChainT, EParadigm Paradigm, typename FunctionT >
FORCEINLINE typename std::enable_if<std::is_function<FunctionT>::value, EApparatusStatus>::type
IMechanical::OperateConcurrently(FunctionT* const Mechanic,
								 const int32      ThreadsCountMax,
								 const int32      SlotsPerThreadMin,
								 const bool       bSync)
{
	return GetMechanism()->template OperateConcurrently<ChainT, Paradigm, FunctionT>(Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
}

#pragma endregion Operating
