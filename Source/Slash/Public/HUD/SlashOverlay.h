// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Count);
	void SetSouls(int32 Count);
	
private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgreesBar;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgreesBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsText;
};
