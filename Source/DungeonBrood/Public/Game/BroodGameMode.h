#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enemies/BroodEnemyCharacter.h"
#include "BroodGameMode.generated.h"

class ABroodCharacter;
class ABiomassPickup;
class AStaticMeshActor;
class AActor;

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
	bool IsDungeonLevel5Complete() const;
	void TriggerDungeonLevel5SurfaceTrial(ABroodCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Evolution")
	void ChooseEvolution(int32 ChoiceIndex);

private:
	void EnsurePlayer();
	bool IsEditorAuthoredDungeonLevel5Map() const;
	void BuildTestArena();
	void BuildDungeonDressing();
	void BuildDungeonLevel5MoodLighting();
	void BuildDungeonLevel5ModularFloor();
	void BuildDungeonLevel5RouteLayout();
	void BuildDungeonLevel5RouteCollision();
	void BuildDungeonLevel5FirstViewArchitecture();
	void BuildDungeonLevel5ThresholdCorridorArchitecture();
	void BuildDungeonLevel5Landmarks();
	void BuildDungeonLevel5SurfaceTemptation();
	void BuildDungeonLevel5PrologueDressing();
	void BuildDungeonLevel5TraversalComposition();
	void BuildDungeonLevel5CreepyDressing();
	void BuildDungeonLevel5JailerApproachDressing();
	void BuildDungeonLevel5JailerBossRoomArchitecture();
	void BuildDungeonLevel5ExitArchitecture();
	void BuildDungeonLevel5ExitDescentRoomArchitecture();
	void BuildDungeonLevel5BrokenThresholdDressing();
	void BuildDungeonLevel5ExitSeal();
	void BuildVillageCryptDressing();
	void CompleteDungeonLevel5();
	void SpawnStaticCube(const FString& Name, const FVector& Location, const FVector& Scale, const FLinearColor& Color, bool bVisible = true);
	void SpawnRouteCollisionBlock(const FString& Name, const FVector& Location, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);
	void SpawnDecorativeMesh(const FString& Name, const TCHAR* MeshPath, const FVector& Location, const FVector& Scale, const FLinearColor& Color, const FRotator& Rotation = FRotator::ZeroRotator, bool bApplyTint = true);
	void SpawnPointLight(const FString& Name, const FVector& Location, const FLinearColor& Color, float Intensity, float Radius);
	void SpawnLevelText(const FString& Name, const FString& Text, const FVector& Location, const FRotator& Rotation, const FColor& Color, float WorldSize);
	void StartNextEncounter();
	void SpawnEnemy(EBroodEnemyType EnemyType, const FVector& Location);
	void SpawnBiomassPickup(const FVector& Location, float Amount);
	UFUNCTION()
	void OnSurfaceTrialTriggerOverlap(AActor* OverlappedActor, AActor* OtherActor);
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
	bool bDungeonLevel5Complete = false;
	bool bSurfaceTrialTriggered = false;
};
