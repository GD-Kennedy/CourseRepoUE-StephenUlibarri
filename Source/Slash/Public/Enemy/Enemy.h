// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class AAIController;
class UHealthBarComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void Destroyed() override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	virtual void AttackEnd() override;

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float Damage) override;
	virtual void HitReactEnd() override;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category="AI|Patrol")
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category="AI|Patrol")
	float PatrolWaitMin = 4.f;

	UPROPERTY(EditAnywhere, Category="AI|Patrol")
	float PatrolWaitMax = 8.f;

	UPROPERTY(EditAnywhere, Category="AI|Patrol")
	float PatrollingSpeed = 135.f;

	UPROPERTY(EditInstanceOnly, Category="AI|Patrol")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category="AI|Patrol")
	TArray<AActor*> PatrolTargets;

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditAnywhere, Category="Combat")
	float DeathLifeSpan = 8.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category="AI|Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category="AI|Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category="AI|Combat")
	float ChasingSpeed = 330.f;

	FTimerHandle AttackTimer;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	AActor* ChoosePatrolTarget();

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);

	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsAttacking();
	bool IsEngaged();
	bool IsChasing();
	bool IsDead();

	void StartAttackTimer();
	void ClearAttackTimer();

	void ChaseTarget();
	void LoseInterest();

	void StartPatrolling();
	void ClearPatrolTimer();

	void ShowHealthBar();
	void HideHealthBar();
};
