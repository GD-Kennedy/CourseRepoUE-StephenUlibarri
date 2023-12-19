#pragma once

UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
	EWT_Melee_OneHanded UMETA(DisplayName = "One-Handed"),
	EWT_Melee_TwoHanded UMETA(DisplayName = "Two-Handed"),
	EWT_Melee_RangedBow UMETA(DisplayName = "Ranged Bow")
};