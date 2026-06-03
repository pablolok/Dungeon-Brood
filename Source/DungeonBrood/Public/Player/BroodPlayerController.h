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

	UFUNCTION(Exec)
	void BroodDungeonLevel5SmokeTest();

	UFUNCTION(Exec)
	void BroodCaptureWindowedSmokeScreenshot();

	UFUNCTION(Exec)
	void BroodDungeonLevel5EnvironmentVisualSmokeTest();

	UFUNCTION(Exec)
	void BroodDungeonLevel5CompletionVisualSmokeTest();

	UFUNCTION(Exec)
	void BroodDungeonLevel5ExitShowcaseSmokeTest();

	UFUNCTION(Exec)
	void BroodDungeonLevel5JailerVisualSmokeTest();

	UFUNCTION(Exec)
	void BroodDungeonLevel5JailerAssetReviewSmokeTest();

	UFUNCTION(Exec)
	void BroodDungeonLevel5LarvaAssetReviewSmokeTest();

	UFUNCTION(Exec)
	void BroodDungeonLevel5SurfaceTrialSmokeTest();

	UFUNCTION(Exec)
	void BroodDungeonLevel5SurfaceTrialVisualSmokeTest();

	void RestoreGameplayInputMode();

protected:
	virtual void SetupInputComponent() override;

private:
	void RunControlSmokeStep();
	void FinishControlSmokeTest();
	void RunCombatSmokeStep();
	void FinishCombatSmokeTest();
	void RunWavePressureSmokeStep();
	void RunDungeonLevel5SmokeStep();
	void RunDungeonLevel5CompletionVisualSmokeStep();
	void RunDungeonLevel5JailerVisualSmokeStep();
	void CaptureDungeonLevel5EnvironmentVisualScreenshot();
	void CaptureDungeonLevel5JailerAssetReviewScreenshot();
	void CaptureDungeonLevel5LarvaAssetReviewScreenshot();
	void FinishDungeonLevel5SurfaceTrialSmokeTest();
	void CaptureDungeonLevel5SurfaceTrialVisualScreenshot();
	void FinishDungeonLevel5SurfaceTrialVisualSmokeTest();
	void RunMouseAimSmokeStep();
	void FinishMouseAimSmokeTest();
	void TogglePauseMenu();
	void ShowPauseMenu();
	void HidePauseMenu();
	void RequestViewportScreenshot(const FString& FileName, const TCHAR* Marker);

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
	FTimerHandle DungeonLevel5SmokeTimerHandle;
	FTimerHandle DungeonLevel5EnvironmentVisualScreenshotTimerHandle;
	FTimerHandle DungeonLevel5EnvironmentVisualSmokeQuitTimerHandle;
	FTimerHandle DungeonLevel5CompletionVisualSmokeTimerHandle;
	FTimerHandle DungeonLevel5CompletionVisualSmokeQuitTimerHandle;
	FTimerHandle DungeonLevel5JailerVisualSmokeTimerHandle;
	FTimerHandle DungeonLevel5JailerVisualSmokeQuitTimerHandle;
	FTimerHandle DungeonLevel5JailerAssetReviewScreenshotTimerHandle;
	FTimerHandle DungeonLevel5JailerAssetReviewQuitTimerHandle;
	FTimerHandle DungeonLevel5LarvaAssetReviewScreenshotTimerHandle;
	FTimerHandle DungeonLevel5LarvaAssetReviewQuitTimerHandle;
	FTimerHandle DungeonLevel5SurfaceTrialSmokeQuitTimerHandle;
	FTimerHandle DungeonLevel5SurfaceTrialVisualScreenshotTimerHandle;
	FTimerHandle DungeonLevel5SurfaceTrialVisualSmokeQuitTimerHandle;
	int32 DungeonLevel5SmokeStep = 0;
	int32 DungeonLevel5SmokeEvolutionSelections = 0;
	bool bDungeonLevel5SmokeBossSeen = false;
	int32 DungeonLevel5CompletionVisualSmokeStep = 0;
	int32 DungeonLevel5CompletionVisualSmokeEvolutionSelections = 0;
	int32 DungeonLevel5CompletionVisualReadyFrames = 0;
	bool bDungeonLevel5CompletionVisualScreenshotRequested = false;
	bool bDungeonLevel5ExitShowcaseSmokeActive = false;
	int32 DungeonLevel5JailerVisualSmokeStep = 0;
	int32 DungeonLevel5JailerVisualSmokeEvolutionSelections = 0;
	int32 DungeonLevel5JailerVisualReadyFrames = 0;
	bool bDungeonLevel5JailerVisualScreenshotRequested = false;
	FVector MouseAimSmokeStartLocation = FVector::ZeroVector;
	FVector MouseAimSmokeTargetDirection = FVector::ZeroVector;
	float MouseAimSmokeInitialCameraYaw = 0.0f;
	int32 MouseAimSmokeStep = 0;
	bool bMouseAimSmokeActive = false;
};
