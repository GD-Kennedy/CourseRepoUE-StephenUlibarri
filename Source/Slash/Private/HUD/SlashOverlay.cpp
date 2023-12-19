// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgreesBar)
	{
		HealthProgreesBar->SetPercent(Percent);	
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgreesBar)
	{
		StaminaProgreesBar->SetPercent(Percent);	
	}
}

void USlashOverlay::SetGold(int32 Count)
{
	if (GoldText)
	{
		FString String = FString::Printf(TEXT("%d"), Count);
		auto Text = FText::FromString(String);
		GoldText->SetText(Text);
	}
}

void USlashOverlay::SetSouls(int32 Count)
{
	if (SoulsText)
	{
		FString String = FString::Printf(TEXT("%d"), Count);
		auto Text = FText::FromString(String);
		SoulsText->SetText(Text);
	}
}
