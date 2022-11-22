// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/GoNorthUEGameDataWidget.h"
#include "GoNorthUEHUD.generated.h"

class UGoNorthUEGameDataWidget;

UCLASS()
class AGoNorthUEHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGoNorthUEHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	void GameScoreUpdate(int64 InValue);

	UFUNCTION()
	void HighScoreUpdate();

	UFUNCTION()
	void WaveNumberUpdate(int32 InValue);

	int64 TestCounter = -27;

	UPROPERTY(VisibleAnywhere)
	UGoNorthUEGameDataWidget* CurrentWidget = nullptr;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGoNorthUEGameDataWidget> GameDataWidgetClass;

	

};

