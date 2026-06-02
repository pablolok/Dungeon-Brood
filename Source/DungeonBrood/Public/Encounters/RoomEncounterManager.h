#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomEncounterManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEncounterClearedSignature);

UCLASS()
class DUNGEONBROOD_API ARoomEncounterManager : public AActor
{
	GENERATED_BODY()

public:
	ARoomEncounterManager();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void RegisterEnemy(AActor* EnemyActor);

	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void NotifyEnemyDefeated(AActor* EnemyActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Encounter")
	bool IsEncounterCleared() const;

	UPROPERTY(BlueprintAssignable, Category = "Encounter")
	FEncounterClearedSignature OnEncounterCleared;

protected:
	// Designers can seed this from placed room enemies; spawners can call RegisterEnemy at runtime.
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Encounter")
	TArray<TObjectPtr<AActor>> TrackedEnemies;

private:
	UFUNCTION()
	void HandleTrackedEnemyDestroyed(AActor* DestroyedActor);

	void BindEnemyDestroyed(AActor* EnemyActor);
	void CheckEncounterCleared();

	bool bEncounterCleared = false;
};
