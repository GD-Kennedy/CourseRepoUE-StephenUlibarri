// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "NiagaraComponent.h"
#include "Characters/SlashCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"


AWeapon::AWeapon()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	HitBox = CreateDefaultSubobject<UBoxComponent>("HitBox");
	HitBox->SetupAttachment(GetRootComponent());
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	HitboxTraceStart = CreateDefaultSubobject<USceneComponent>("HitBox Trace Start");
	HitboxTraceStart->SetupAttachment(GetRootComponent());
	
	HitboxTraceEnd = CreateDefaultSubobject<USceneComponent>("HitBox Trace End");
	HitboxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::EmptyIgnoreList()
{
	IgnoreActors.Empty();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnHitboxBeginOverlap);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnHitboxEndOverlap);
}

void AWeapon::AttackMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	Mesh->AttachToComponent(InParent,
							FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
							InSocketName);
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());	
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DeactiveEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->DestroyComponent();
	}
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_PickedUp;

	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttackMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();
	PlayEquipSound();
	DeactiveEmbers();
}

void AWeapon::ExecuteGetHit(FHitResult BoxHit)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor()))
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{		
	FHitResult BoxHit;
	BoxTrace(BoxHit);
	
	if (BoxHit.GetActor())
	{
		if (GetOwner() == BoxHit.GetActor() || ActorIsSameType(BoxHit.GetActor()))
		{
			return;
		}
		
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(),
			Damage,	GetInstigator()->GetController(),
			this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
	}
}

void AWeapon::OnHitboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = HitboxTraceStart->GetComponentLocation();
	const FVector End = HitboxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	
	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceExtent,
										 HitboxTraceStart->GetComponentRotation(), TraceTypeQuery1,
										 false, ActorsToIgnore,
										 bShowDebug ? EDrawDebugTrace::ForDuration :  EDrawDebugTrace::None,
										 BoxHit, true);
	IgnoreActors.AddUnique(BoxHit.GetActor());
}
