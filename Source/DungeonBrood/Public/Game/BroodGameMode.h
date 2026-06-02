#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enemies/BroodEnemyCharacter.h"
#include "BroodGameMode.generated.h"

class ABroodCharacter;
class AStaticMeshActor;

USTRUCT(BlueprintType)
struct DUNGEONBROOD_API FBroodEvolutionChoice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Evolution")
	FName EvolutionId;

	UPROPERTY(BlueprintReadOnly, Category = "Evolution")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Evolution")
	FText Description;
};

UCLASS()
class DUNGEONBROOD_API ABroodGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABroodGameMode();

	virtual void BeginPlay() override;

	void NotifyEnemyKilled(ABroodEnemyCharacter* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Evolution")
	void ChooseEvolution(int32 ChoiceIndex);

private:
	void EnsurePlayer();
	void BuildTestArena();
	void SpawnStaticCube(const FString& Name, const FVector& Location, const FVector& Scale);
	void StartNextEncounter();
	void SpawnEnemy(EBroodEnemyType EnemyType, const FVector& Location);
	void OfferEvolutionChoices();
	void PrintHud() const;
	TArray<FBroodEvolutionChoice> BuildEvolutionCatalog() const;

	UPROPERTY()
	TObjectPtr<ABroodCharacter> PlayerCharacter;

	UPROPERTY()
	TArray<TObjectPtr<ABroodEnemyCharacter>> AliveEnemies;

	TArray<FBroodEvolutionChoice> CurrentChoices;
	int32 EncounterIndex = -1;
	bool bAwaitingEvolutionChoice = false;
	bool bRunComplete = false;
};
