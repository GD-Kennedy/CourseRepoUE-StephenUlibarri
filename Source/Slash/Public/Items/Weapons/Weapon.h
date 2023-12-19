// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponTypes WeaponType;
	
	UFUNCTION()
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void ExecuteGetHit(FHitResult BoxHit);
	bool ActorIsSameType(AActor* OtherActor);

	UFUNCTION()
	void AttackMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactiveEmbers();
	
	void EmptyIgnoreList();

	USoundBase* GetArmSound() const { return ArmSound; }
	USoundBase* GetEquipSound() const { return EquipSound; }
	FORCEINLINE UBoxComponent* GetHitBox() const { return HitBox; }

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnHitboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	USoundBase* EquipSound;
	
	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	USoundBase* ArmSound;

	UPROPERTY(VisibleAnywhere, Category= "Weapon Properties")
	UBoxComponent* HitBox;

	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	float Damage = 20.f;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* HitboxTraceStart;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* HitboxTraceEnd;

	UPROPERTY()
	TArray<AActor*> IgnoreActors;

	void BoxTrace(FHitResult& HitResult);
	
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);
	
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	bool bShowDebug = false; 
};
