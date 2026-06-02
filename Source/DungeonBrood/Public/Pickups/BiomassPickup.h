#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BiomassPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

UCLASS()
class DUNGEONBROOD_API ABiomassPickup : public AActor
{
	GENERATED_BODY()

public:
	ABiomassPickup();

	void InitializeBiomass(float NewBiomassAmount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Biomass")
	float GetBiomassAmount() const;

private:
	UFUNCTION()
	void HandlePickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void UpdateVisuals();

	UPROPERTY(VisibleAnywhere, Category = "Biomass")
	TObjectPtr<USphereComponent> PickupSphere;

	UPROPERTY(VisibleAnywhere, Category = "Biomass")
	TObjectPtr<UStaticMeshComponent> PickupVisual;

	UPROPERTY(VisibleAnywhere, Category = "Biomass")
	TObjectPtr<UTextRenderComponent> AmountVisual;

	float BiomassAmount = 1.0f;
	bool bCollected = false;
};
