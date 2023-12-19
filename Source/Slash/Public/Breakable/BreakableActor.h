// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Physics/Experimental/ChaosEventType.h"
#include "BreakableActor.generated.h"

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	
	UGeometryCollectionComponent* GeometryCollection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;
	
	UFUNCTION()
	void OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent);

public:
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter);

private:
	UPROPERTY(EditAnywhere, Category=AudioSounds)
	USoundBase* BreakSound;
	
	UPROPERTY(EditAnywhere, Category=BreakableProperties)
	USceneComponent* TreasureLocation;

	UPROPERTY(EditAnywhere, Category=BreakableProperties)
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	bool bBroken = false;
};
