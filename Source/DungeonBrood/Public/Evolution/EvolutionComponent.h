#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EvolutionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEvolutionAcquiredSignature, FName, EvolutionId);

UCLASS(ClassGroup = (Brood), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class DUNGEONBROOD_API UEvolutionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEvolutionComponent();

	UFUNCTION(BlueprintCallable, Category = "Evolution")
	bool AcquireEvolution(FName EvolutionId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evolution")
	bool HasEvolution(FName EvolutionId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evolution")
	TArray<FName> GetAcquiredEvolutionIds() const;

	UPROPERTY(BlueprintAssignable, Category = "Evolution")
	FEvolutionAcquiredSignature OnEvolutionAcquired;

private:
	// Save-game integration should persist these IDs rather than asset object references.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Evolution", meta = (AllowPrivateAccess = "true"))
	TArray<FName> AcquiredEvolutionIds;
};
