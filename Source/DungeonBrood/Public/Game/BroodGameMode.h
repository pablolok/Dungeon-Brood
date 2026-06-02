#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enemies/BroodEnemyCharacter.h"
#include "BroodGameMode.generated.h"

class ABroodCharacter;
class ABiomassPickup;
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
	void BuildDungeonDressing();
	void SpawnStaticCube(const FString& Name, const FVector& Location, const FVector& Scale, const FLinearColor& Color);
	void SpawnDecorativeMesh(const FString& Name, const TCHAR* MeshPath, const FVector& Location, const FVector& Scale, const FLinearColor& Color, const FRotator& Rotation = FRotator::ZeroRotator, bool bApplyTint = true);
	void SpawnPointLight(const FString& Name, const FVector& Location, const FLinearColor& Color, float Intensity, float Radius);
	void StartNextEncounter();
	void SpawnEnemy(EBroodEnemyType EnemyType, const FVector& Location);
	void SpawnBiomassPickup(const FVector& Location, float Amount);
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
