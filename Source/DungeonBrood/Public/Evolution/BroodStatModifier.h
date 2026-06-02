#pragma once

#include "CoreMinimal.h"
#include "BroodStatModifier.generated.h"

UENUM(BlueprintType)
enum class EBroodAttribute : uint8
{
	Health,
	MaxHealth,
	Stamina,
	MaxStamina,
	AttackPower,
	Armor,
	MovementSpeed
};

USTRUCT(BlueprintType)
struct DUNGEONBROOD_API FBroodStatModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evolution")
	EBroodAttribute Attribute = EBroodAttribute::AttackPower;

	// Additive-only for the first pass. Replace or extend this when evolution scaling rules mature.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evolution")
	float AdditiveValue = 0.0f;
};
