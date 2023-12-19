// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "Enemy/Enemy.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"

ASlashCharacter::ASlashCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;

	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>("Hair");
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("Head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>("Eyebrows");
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("Head");

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	AddInputMapping();
	InitializeOverlay();

	Tags.Add(FName("EngageableTarget"));
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return;
	}
	
	const FVector2D MovementVector = Value.Get<FVector2d>();

	if (Controller && MovementVector != FVector2d::Zero())
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDir, MovementVector.Y);

		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDir, MovementVector.X);
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (Controller && LookAxisValue != FVector2D::Zero())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* OverlappingWeapon)
{
	const EWeaponTypes WeaponType = OverlappingWeapon->WeaponType;

	if (CharacterState != ECharacterState::ECS_Unequipped)
	{
		// TODO: Add proper unequipping swapping later maybe - not in a scope of this course
		return;
	}
	
	if (WeaponType == EWeaponTypes::EWT_Melee_OneHanded)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		OverlappingWeapon->SetInstigator(this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
	else if (WeaponType == EWeaponTypes::EWT_Melee_TwoHanded)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket_TwoHand"), this, this);
		CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
	}
	
	EquippedWeapon = OverlappingWeapon;
	OverlappingItem = nullptr;
}

void ASlashCharacter::ArmWeapon()
{
	if (!EquippedWeapon)
	{
		return;
	}
	
	PlayEquipMontage(FName("Equip"));
	ActionState = EActionState::EAS_EquippingWeapon;
	
	const EWeaponTypes WeaponType = EquippedWeapon->WeaponType;
	if (WeaponType == EWeaponTypes::EWT_Melee_OneHanded)
	{
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
	else if (WeaponType == EWeaponTypes::EWT_Melee_TwoHanded)
	{
		CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
	}
	else
	{
		CharacterState = ECharacterState::ECS_Unequipped;
	}	
}

void ASlashCharacter::DisarmWeapon()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_UnequippingWeapon;
}

void ASlashCharacter::UseItem()
{
	if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
	{
		EquipWeapon(OverlappingWeapon);
		return;
	}
	
	if (CanArm())
	{
		ArmWeapon();
	}
	else if (CanDisarm())
	{
		DisarmWeapon();
	}
}

void ASlashCharacter::Attack()
{
	if (CanAttack())
	{
		TArray<AActor*> OverlappingEnemies;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte ObjectType = UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2);
		ObjectTypes.Add(ObjectType);
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		UKismetSystemLibrary::SphereOverlapActors(
			this, 
			GetActorLocation(), 
			500.f, 
			ObjectTypes, 
			AEnemy::StaticClass(), 
			ActorsToIgnore, 
			OverlappingEnemies
		);

		AEnemy* ClosestEnemy = nullptr;
		float ClosestDistance = FLT_MAX;

		for(auto* Actor : OverlappingEnemies)
		{
			float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
			if(Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestEnemy = Cast<AEnemy>(Actor);
			}
		}

		if(ClosestEnemy)
		{
			CombatTarget = ClosestEnemy;
		}
		
		ActionState = EActionState::EAS_Attacking;
		PlayOneHandAttackMontage();
	}
}

void ASlashCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina())
	{
		return;
	}
	
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;

	if (Attributes)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
	}

	if (Overlay)
	{
		Overlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::PlayArmSound() const
{
	if (!EquippedWeapon)
	{
		return;
	}
	if (USoundBase* ArmSound = EquippedWeapon->GetArmSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ArmSound, EquippedWeapon->GetActorLocation());	
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	CombatTarget = nullptr;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied
	&& CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::PlayEquipMontage(const FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

bool ASlashCharacter::CanDisarm() const
{
	return ActionState == EActionState::EAS_Unoccupied
	&& CharacterState != ECharacterState::ECS_Unequipped
	&& EquippedWeapon;
}

bool ASlashCharacter::CanArm() const
{
	return ActionState == EActionState::EAS_Unoccupied 
	&& CharacterState == ECharacterState::ECS_Unequipped
	&& EquippedWeapon;
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::IframeStart()
{
	DisableMeshCollision();
}

void ASlashCharacter::IframeEnd()
{
	EnableMeshCollision();
}

void ASlashCharacter::Die()
{
	Super::Die();
	ActionState = EActionState::EAS_Dead;
	
	SetLifeSpan(5.f);
}

bool ASlashCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

FVector ASlashCharacter::GetTranslationWarpTarget()
{
	if (!CombatTarget)
	{
		return GetActorLocation();
	}
	
	return Super::GetTranslationWarpTarget();
}

FVector ASlashCharacter::GetRotationWarpTarget()
{
	if (!CombatTarget)
	{
		return GetActorLocation();
	}
	
	return Super::GetRotationWarpTarget();
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes)
	{
		Attributes->RegenStamina(DeltaTime);
	}

	if (Overlay)
	{
		Overlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Triggered, this, &ASlashCharacter::UseItem);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
	}
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes)
	{
		Attributes->AddSouls(Soul->GetSouls());
	}

	if (Overlay)
	{
		Overlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes)
	{
		Attributes->AddGold(Treasure->GetGold());
	}

	if (Overlay)
	{
		Overlay->SetGold(Attributes->GetGold());
	}
}


float ASlashCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	HandleDamage(Damage);
	SetHUDHealth();
	return Damage;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->IsAlive())
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	
	CombatTarget = nullptr;
}

void ASlashCharacter::AddInputMapping()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void ASlashCharacter::InitializeOverlay()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ASlashHUD* HUD = Cast<ASlashHUD>(PlayerController->GetHUD()))
		{
			Overlay = HUD->GetSlashOverlay();
			if (Overlay)
			{
				Overlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				Overlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
				Overlay->SetGold(0);
				Overlay->SetSouls(0);
			}
		}
	}
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}	
}

void ASlashCharacter::SetHUDHealth()
{
	if (Overlay && Attributes)
	{
		Overlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}