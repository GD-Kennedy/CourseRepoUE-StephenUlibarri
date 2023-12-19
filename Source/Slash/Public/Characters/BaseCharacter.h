// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class UAttributeComponent;
class AWeapon;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;
	
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	AWeapon* EquippedWeapon;
	
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category="Combat")
	double WarpTargetDistance = 75.f;
	
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	virtual void Die();
	void DisableCapsuleCollision();
	void EnableMeshCollision();
	void DisableMeshCollision();
	virtual void Attack();
	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
	virtual bool IsAlive();
	virtual void GetHit_Implementation(const FVector& ImpactPoint,  AActor* Hitter) override;
	
	virtual int32 PlayOneHandAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	virtual FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual FVector GetRotationWarpTarget();
	
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void PlayHitReactMontage(const FName& SectionName);
	
	UFUNCTION(BlueprintCallable)
	virtual void HandleDamage(float Damage);
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type NewCollisionType);

	UFUNCTION(BlueprintCallable)
	virtual void HitReactEnd();

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;

	UPROPERTY(EditDefaultsOnly, Category = "Combat");
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat");
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat");
	UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;

	int32 PlayRandomMontageSection(UAnimMontage* Montage);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
};
