// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"

#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Kismet/GameplayStatics.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
	SetRootComponent(GeometryCollection);

	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	TreasureLocation = CreateDefaultSubobject<USceneComponent>("TreasureLocation");
	TreasureLocation->SetupAttachment(GetRootComponent());
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnChaosBreakEvent);
}

void ABreakableActor::OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent)
{
	Execute_GetHit(this, BreakEvent.Location, nullptr);
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken)
	{
		return;
	}
	bBroken = true;
	
	if (BreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BreakSound, GetActorLocation());
	}
	
	if (UWorld* World = GetWorld(); World && TreasureClasses.Num() > 0)
	{
		const TSubclassOf<ATreasure> RandomTreasure = TreasureClasses[FMath::RandRange(0, TreasureClasses.Num() - 1)];
		if (ensure(RandomTreasure))
		{
			World -> SpawnActor<ATreasure>(RandomTreasure, TreasureLocation->GetComponentTransform().GetLocation(), GetActorRotation());
		}
	}

	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SetLifeSpan(5.f);
}
