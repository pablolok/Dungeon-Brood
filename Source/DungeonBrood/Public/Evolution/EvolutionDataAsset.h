#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "Evolution/BroodStatModifier.h"
#include "EvolutionDataAsset.generated.h"

UCLASS(BlueprintType)
class DUNGEONBROOD_API UEvolutionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Stable designer-authored ID used for save data, prerequisites, and unlock checks.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Evolution")
	FName EvolutionId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Evolution")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Evolution", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Evolution")
	TArray<FName> RequiredEvolutionIds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Evolution")
	TArray<FBroodStatModifier> StatModifiers;

	// Extension point for active/passive abilities granted by this evolution.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Evolution")
	TSubclassOf<UGameplayAbility> GrantedAbilityClass;
};
