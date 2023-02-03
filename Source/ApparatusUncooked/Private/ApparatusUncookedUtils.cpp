/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusUncookedUtils.cpp
 * Created: 2021-10-20 17:25:29
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

#include "ApparatusUncookedUtils.h"

#include "K2Node_TemporaryVariable.h"


void
MoveSplitPinLinkToIntermediate(class UEdGraphNode* const     SourceNode,
							   class FKismetCompilerContext& CompilerContext,
							   const UEdGraphSchema_K2*      Schema,
							   UEdGraphPin* const            SrcPin,
							   UEdGraphPin* const            DstPin)
{
	check(Schema);

	if (SrcPin->SubPins.Num() == 0)
	{
		// The source pin is not split at all.
		// Do the usual move:
		ensure(!CompilerContext
				.MovePinLinksToIntermediate(
					*SrcPin,
					*DstPin)
				.IsFatal());
		if ((DstPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard) &&
			(SrcPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct))
		{
			if (DstPin->LinkedTo.Num() == 0)
			{
				// The destination intermediate pin still has no links, so
				// its type won't get defined (as it should in order to compile),
				// so we have to make a temporary struct here
				// and move exactly it to the destination as a dummy fill.
				auto* StructType = Cast<UScriptStruct>(SrcPin->PinType.PinSubCategoryObject);
				if (StructType)
				{
					StructType->RecursivelyPreload();
				}
				auto* const TempStruct = CompilerContext.SpawnInternalVariable(
					SourceNode,
					SrcPin->PinType.PinCategory,
					SrcPin->PinType.PinSubCategory,
					StructType);
				ensure(!CompilerContext
						.MovePinLinksToIntermediate(
							*TempStruct->GetVariablePin(),
							*DstPin)
						.IsFatal());
			}
			else
			{
				// PostReconstructNode() will refresh parent pin wildcards so we can't use it here.
				DstPin->PinType.PinCategory          = UEdGraphSchema_K2::PC_Struct;
				DstPin->PinType.PinSubCategoryObject = SrcPin->PinType.PinSubCategoryObject;
			}
		}
	}
	else
	{
		// The source pin is split.
		// Split the destination aswell and move the links there...
		if (!ensureMsgf(SrcPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct,
						TEXT("The source pin must be a struct: %s"),
						*SrcPin->PinName.ToString()))
		{
			return;
		}
		if (!ensureMsgf(SrcPin->PinType.PinSubCategoryObject != nullptr,
						TEXT("Missing a struct type of the source pin: %s"),
						*SrcPin->PinName.ToString()))
		{
			return;
		}
		DstPin->PinType.PinCategory          = UEdGraphSchema_K2::PC_Struct;
		DstPin->PinType.PinSubCategoryObject = SrcPin->PinType.PinSubCategoryObject;
		// Move the parent pin (just in case):
		ensure(!CompilerContext
				.MovePinLinksToIntermediate(
					*SrcPin,
					*DstPin)
				.IsFatal());
		Schema->SplitPin(DstPin, false);
		if (!ensureMsgf(DstPin->SubPins.Num() == SrcPin->SubPins.Num(),
						TEXT("The splitting of source and destination pins differ: %s, %s"),
						*SrcPin->PinName.ToString(), *DstPin->PinName.ToString()))
		{
			return;
		}
		for (int32 i = 0; i < SrcPin->SubPins.Num(); ++i)
		{
			auto* DstSubPin = DstPin->SubPins[i];
			auto* SrcSubPin = SrcPin->SubPins[i];
			MoveSplitPinLinkToIntermediate(SourceNode, CompilerContext, Schema,
										   SrcSubPin, DstSubPin);
		}
	}
}
