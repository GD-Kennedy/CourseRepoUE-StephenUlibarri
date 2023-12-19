#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_UnequippingWeapon UMETA(DisplayName = "Unequipping Weapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Dead UMETA(DisplayName = "Dead"),
};

UENUM(BlueprintType)
enum EDeathPose
{
	// Don't reorder - they're connected to Dying animation and switching the order will make them not match
	EDP_Death1 UMETA(DisplayName = "Death1"), // 1
	EDP_Death2 UMETA(DisplayName = "Death2"), // 2
	EDP_Death3 UMETA(DisplayName = "Death3"), // 3
	EDP_Death4 UMETA(DisplayName = "Death4"), // 4
	EDP_Death5 UMETA(DisplayName = "Death5"), // 5
	EDP_Death6 UMETA(DisplayName = "Death6"), // 6
	EDP_Death7 UMETA(DisplayName = "Death7"), // 7

	EDP_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),
	EES_HitReaction UMETA(DisplayName = "HitReaction"),
};