/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusUncookedUtils.h
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

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "EdGraphSchema_K2_Actions.h"
#include "KismetCompiler.h"


#define MAKE_LINK(from, to) ensure(Schema->TryCreateConnection(from, to))
#define MAKE_LINKF(from, to, format, ...) ensureMsgf(Schema->TryCreateConnection(from, to), format, ##__VA_ARGS__)

#define MOVE_LINK(from, to) ensure(!CompilerContext.MovePinLinksToIntermediate(*from, *to).IsFatal())
#define MOVE_LINKF(from, to, format, ...) ensureMsgf(!CompilerContext.MovePinLinksToIntermediate(*from, *to).IsFatal(), format, ##__VA_ARGS__)
#define MOVE_SPLIT_LINK(from, to) MoveSplitPinLinkToIntermediate(this, CompilerContext, Schema, (from), (to))

#define FLAGMARK_SHORTCUT_STR "\u25C6"
#define TRAIT_SHORTCUT_STR    "\u25A0"
#define DETAIL_SHORTCUT_STR   "\u25CF"

#define NEGATIVE_FLAGMARK_SHORTCUT_STR "\u25C7"
#define NEGATIVE_TRAIT_SHORTCUT_STR    "\u25A1"
#define NEGATIVE_DETAIL_SHORTCUT_STR   "\u25CB"

/**
 * Find a structure function parameter of a certain type.
 */
inline FStructProperty*
FindParameterOfType(UFunction* const     Function,
					UScriptStruct* const Type)
{
	for (TFieldIterator<FProperty> It(Function); It; ++It)
 	{
		FProperty* Property = *It;
		if ((Property->PropertyFlags & CPF_Parm) == CPF_None) continue;
		FStructProperty* StructProp = CastField<FStructProperty>(Property);
		if (!StructProp) continue;
		if (StructProp->Struct == Type) return StructProp;
 	}
	return nullptr;
}

/**
 * Find a function parameter of a certain class.
 * 
 * Supports base classes and interfaces.
 */
inline FProperty*
FindParameterOfClass(UFunction* Function, UClass* Class)
{
	if (Class->HasAnyClassFlags(CLASS_Interface))
	{
		for (TFieldIterator<FProperty> It(Function); It; ++It)
		{
			FProperty* Property = *It;
			if ((Property->PropertyFlags & CPF_Parm) == CPF_None) continue;
			FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property);
			if (ObjectProp) 
			{
				if (ObjectProp->PropertyClass->ImplementsInterface(Class)) return ObjectProp;
			}
			FInterfaceProperty* InterfaceProp = CastField<FInterfaceProperty>(Property);
			if (InterfaceProp)
			{
				if (InterfaceProp->InterfaceClass == Class) return InterfaceProp;
			}
		}
	}
	else
	{
		// Find exact class first...
		for (TFieldIterator<FProperty> It(Function); It; ++It)
		{
			FProperty* Property = *It;
			if ((Property->PropertyFlags & CPF_Parm) == CPF_None) continue;
			FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property);
			if (!ObjectProp) continue;
			if (ObjectProp->PropertyClass == Class) return ObjectProp;
		}
		// Find descendant...
		for (TFieldIterator<FProperty> It(Function); It; ++It)
		{
			FProperty* Property = *It;
			if ((Property->PropertyFlags & CPF_Parm) == CPF_None) continue;
			FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property);
			if (!ObjectProp) continue;
			if (ObjectProp->PropertyClass->IsChildOf(Class)) return ObjectProp;
		}
	}
	return nullptr;
}

APPARATUSUNCOOKED_API void
MoveSplitPinLinkToIntermediate(class UEdGraphNode* const     SourceNode,
							   class FKismetCompilerContext& CompilerContext,
							   const UEdGraphSchema_K2*      Schema,
							   UEdGraphPin* const            SrcPin,
							   UEdGraphPin* const            DstPin);
