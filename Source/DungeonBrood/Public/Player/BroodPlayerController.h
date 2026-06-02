#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BroodPlayerController.generated.h"

class UBroodPauseMenuWidget;
class ABroodEnemyCharacter;
class ABiomassPickup;

UCLASS()
class DUNGEONBROOD_API ABroodPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Exec)
	void BroodQuit();

	UFUNCTION(Exec)
	void BroodRestart();

	UFUNCTION(Exec)
	void BroodControlSmokeTest();

	UFUNCTION(Exec)
	void BroodCombatSmokeTest();

	UFUNCTION(Exec)
	void BroodWavePressureSmokeTest();

	UFUNCTION(Exec)
	void BroodMouseAimSmokeTest();

	void RestoreGameplayInputMode();

protected:
	virtual void SetupInputComponent() override;

private:
	void RunControlSmokeStep();
	void FinishControlSmokeTest();
	void RunCombatSmokeStep();
	void FinishCombatSmokeTest();
	void RunWavePressureSmokeStep();
	void RunMouseAimSmokeStep();
	void FinishMouseAimSmokeTest();
	void TogglePauseMenu();
	void ShowPauseMenu();
	void HidePauseMenu();

	UPROPERTY()
	TObjectPtr<UBroodPauseMenuWidget> PauseMenuWidget;

	FTimerHandle ControlSmokeTimerHandle;
	FTimerHandle CombatSmokeTimerHandle;
	FTimerHandle WavePressureSmokeTimerHandle;
	FTimerHandle MouseAimSmokeTimerHandle;
	FVector ControlSmokeStartLocation = FVector::ZeroVector;
	int32 ControlSmokeStep = 0;

	UPROPERTY()
	TObjectPtr<ABroodEnemyCharacter> CombatSmokeEnemy;

	UPROPERTY()
	TObjectPtr<ABiomassPickup> CombatSmokePickup;

	float CombatSmokeInitialBiomass = 0.0f;
	int32 CombatSmokeStep = 0;
	float WavePressureInitialHealth = 0.0f;
	int32 WavePressureSmokeStep = 0;
	FVector MouseAimSmokeStartLocation = FVector::ZeroVector;
	FVector MouseAimSmokeTargetDirection = FVector::ZeroVector;
	float MouseAimSmokeInitialCameraYaw = 0.0f;
	int32 MouseAimSmokeStep = 0;
	bool bMouseAimSmokeActive = false;
};
