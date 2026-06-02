#include "Player/BroodPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Characters/BroodCharacter.h"
#include "Enemies/BroodEnemyCharacter.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pickups/BiomassPickup.h"
#include "TimerManager.h"
#include "UI/BroodPauseMenuWidget.h"

void ABroodPlayerController::BeginPlay()
{
	Super::BeginPlay();
	RestoreGameplayInputMode();
	UE_LOG(LogTemp, Display, TEXT("BROOD_MOUSE_AIM_ENABLED: gameplay cursor visible."));
}

void ABroodPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bMouseAimSmokeActive)
	{
		if (ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn()))
		{
			BroodCharacter->AimAtWorldLocation(BroodCharacter->GetActorLocation() + MouseAimSmokeTargetDirection * 500.0f);
			BroodCharacter->AddMovementInput(FVector::XAxisVector, 1.0f, true);
		}
	}
}

void ABroodPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		FInputActionBinding& PauseBinding = InputComponent->BindAction(TEXT("TogglePauseMenu"), IE_Pressed, this, &ABroodPlayerController::TogglePauseMenu);
		PauseBinding.bExecuteWhenPaused = true;
	}
}

void ABroodPlayerController::BroodQuit()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void ABroodPlayerController::BroodRestart()
{
	SetPause(false);
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ABroodPlayerController::BroodControlSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_CONTROL_SMOKE_FAILED: player controller does not possess ABroodCharacter."));
		BroodQuit();
		return;
	}

	ControlSmokeStartLocation = BroodCharacter->GetActorLocation();
	ControlSmokeStep = 0;

	ShowPauseMenu();
	if (!PauseMenuWidget || !PauseMenuWidget->IsInViewport())
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_CONTROL_SMOKE_FAILED: pause menu did not enter the viewport."));
		BroodQuit();
		return;
	}
	HidePauseMenu();

	UE_LOG(LogTemp, Display, TEXT("BROOD_CONTROL_SMOKE_STARTED at %s."), *ControlSmokeStartLocation.ToCompactString());
	GetWorldTimerManager().SetTimer(ControlSmokeTimerHandle, this, &ABroodPlayerController::RunControlSmokeStep, 0.25f, true, 0.1f);
}

void ABroodPlayerController::BroodCombatSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_COMBAT_SMOKE_FAILED: player controller does not possess ABroodCharacter."));
		BroodQuit();
		return;
	}

	for (TActorIterator<ABroodEnemyCharacter> EnemyIterator(GetWorld()); EnemyIterator; ++EnemyIterator)
	{
		if (!EnemyIterator->IsDead())
		{
			CombatSmokeEnemy = *EnemyIterator;
			break;
		}
	}

	if (!CombatSmokeEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_COMBAT_SMOKE_FAILED: no living enemy found."));
		BroodQuit();
		return;
	}

	CombatSmokeInitialBiomass = BroodCharacter->GetBiomass();
	CombatSmokeStep = 0;
	CombatSmokeEnemy->SetActorLocation(BroodCharacter->GetActorLocation() + BroodCharacter->GetActorForwardVector() * 180.0f);
	UE_LOG(LogTemp, Display, TEXT("BROOD_COMBAT_SMOKE_STARTED against %s."), *CombatSmokeEnemy->GetName());
	GetWorldTimerManager().SetTimer(CombatSmokeTimerHandle, this, &ABroodPlayerController::RunCombatSmokeStep, 0.2f, true, 0.1f);
}

void ABroodPlayerController::BroodWavePressureSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_WAVE_PRESSURE_SMOKE_FAILED: player controller does not possess ABroodCharacter."));
		BroodQuit();
		return;
	}

	WavePressureInitialHealth = BroodCharacter->GetHealth();
	WavePressureSmokeStep = 0;
	UE_LOG(LogTemp, Display, TEXT("BROOD_WAVE_PRESSURE_SMOKE_STARTED: waiting for enemies to reach stationary player."));
	GetWorldTimerManager().SetTimer(WavePressureSmokeTimerHandle, this, &ABroodPlayerController::RunWavePressureSmokeStep, 0.25f, true, 0.25f);
}

void ABroodPlayerController::BroodMouseAimSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_MOUSE_AIM_SMOKE_FAILED: player controller does not possess ABroodCharacter."));
		BroodQuit();
		return;
	}

	BroodCharacter->SetActorLocation(FVector(-600.0f, 0.0f, 120.0f), false, nullptr, ETeleportType::TeleportPhysics);
	MouseAimSmokeStartLocation = BroodCharacter->GetActorLocation();
	MouseAimSmokeTargetDirection = FVector::YAxisVector;
	MouseAimSmokeInitialCameraYaw = BroodCharacter->GetCameraYaw();
	BroodCharacter->BeginCameraOrbit();
	BroodCharacter->OrbitCameraYaw(45.0f);
	BroodCharacter->EndCameraOrbit();
	MouseAimSmokeStep = 0;
	bMouseAimSmokeActive = true;
	UE_LOG(LogTemp, Display, TEXT("BROOD_MOUSE_AIM_SMOKE_STARTED: moving independently while aiming sideways."));
	GetWorldTimerManager().SetTimer(MouseAimSmokeTimerHandle, this, &ABroodPlayerController::RunMouseAimSmokeStep, 0.2f, true, 0.1f);
}

void ABroodPlayerController::RunControlSmokeStep()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_CONTROL_SMOKE_FAILED: pawn was lost during the test."));
		GetWorldTimerManager().ClearTimer(ControlSmokeTimerHandle);
		BroodQuit();
		return;
	}

	BroodCharacter->AddMovementInput(FVector::ForwardVector, 1.0f);

	if (ControlSmokeStep == 1)
	{
		BroodCharacter->BasicAttack();
	}
	else if (ControlSmokeStep == 2)
	{
		BroodCharacter->Dodge();
	}

	ControlSmokeStep++;
	if (ControlSmokeStep >= 6)
	{
		FinishControlSmokeTest();
	}
}

void ABroodPlayerController::RunCombatSmokeStep()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter || !CombatSmokeEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_COMBAT_SMOKE_FAILED: actor missing during test."));
		GetWorldTimerManager().ClearTimer(CombatSmokeTimerHandle);
		BroodQuit();
		return;
	}

	if (!CombatSmokeEnemy->IsDead())
	{
		CombatSmokeEnemy->SetActorLocation(BroodCharacter->GetActorLocation() + BroodCharacter->GetActorForwardVector() * 180.0f);
		BroodCharacter->BasicAttack();
	}

	CombatSmokeStep++;
	if (CombatSmokeEnemy->IsDead() && CombatSmokeStep < 20)
	{
		if (!CombatSmokePickup)
		{
			for (TActorIterator<ABiomassPickup> PickupIterator(GetWorld()); PickupIterator; ++PickupIterator)
			{
				CombatSmokePickup = *PickupIterator;
				break;
			}

			if (!CombatSmokePickup || !FMath::IsNearlyEqual(BroodCharacter->GetBiomass(), CombatSmokeInitialBiomass))
			{
				UE_LOG(LogTemp, Error, TEXT("BROOD_COMBAT_SMOKE_FAILED: biomass drop missing or reward granted before pickup."));
				GetWorldTimerManager().ClearTimer(CombatSmokeTimerHandle);
				BroodQuit();
				return;
			}

			UE_LOG(LogTemp, Display, TEXT("BROOD_COMBAT_SMOKE_DROP_VERIFIED: biomass unchanged before pickup."));
			CombatSmokePickup->SetActorLocation(BroodCharacter->GetActorLocation());
		}
	}

	if ((CombatSmokeEnemy->IsDead() && BroodCharacter->GetBiomass() > CombatSmokeInitialBiomass) || CombatSmokeStep >= 20)
	{
		FinishCombatSmokeTest();
	}
}

void ABroodPlayerController::RunWavePressureSmokeStep()
{
	const ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_WAVE_PRESSURE_SMOKE_FAILED: pawn missing during test."));
		GetWorldTimerManager().ClearTimer(WavePressureSmokeTimerHandle);
		BroodQuit();
		return;
	}

	if (BroodCharacter->GetHealth() < WavePressureInitialHealth)
	{
		GetWorldTimerManager().ClearTimer(WavePressureSmokeTimerHandle);
		UE_LOG(LogTemp, Display, TEXT("BROOD_WAVE_PRESSURE_SMOKE_PASSED: stationary player took %.1f damage."), WavePressureInitialHealth - BroodCharacter->GetHealth());
		BroodQuit();
		return;
	}

	WavePressureSmokeStep++;
	if (WavePressureSmokeStep >= 40)
	{
		GetWorldTimerManager().ClearTimer(WavePressureSmokeTimerHandle);
		UE_LOG(LogTemp, Error, TEXT("BROOD_WAVE_PRESSURE_SMOKE_FAILED: no enemy reached the stationary player."));
		BroodQuit();
	}
}

void ABroodPlayerController::RunMouseAimSmokeStep()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_MOUSE_AIM_SMOKE_FAILED: pawn missing during test."));
		GetWorldTimerManager().ClearTimer(MouseAimSmokeTimerHandle);
		BroodQuit();
		return;
	}

	if (MouseAimSmokeStep == 1)
	{
		BroodCharacter->BasicAttack();
	}

	MouseAimSmokeStep++;
	if (MouseAimSmokeStep >= 6)
	{
		FinishMouseAimSmokeTest();
	}
}

void ABroodPlayerController::FinishControlSmokeTest()
{
	GetWorldTimerManager().ClearTimer(ControlSmokeTimerHandle);

	const ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	const float DistanceMoved = BroodCharacter
		? FVector::Dist2D(ControlSmokeStartLocation, BroodCharacter->GetActorLocation())
		: 0.0f;

	if (DistanceMoved > 1.0f)
	{
		UE_LOG(LogTemp, Display, TEXT("BROOD_CONTROL_SMOKE_PASSED: moved %.1f units, attack and dodge invoked."), DistanceMoved);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_CONTROL_SMOKE_FAILED: pawn did not move."));
	}

	BroodQuit();
}

void ABroodPlayerController::FinishCombatSmokeTest()
{
	GetWorldTimerManager().ClearTimer(CombatSmokeTimerHandle);

	const ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	const bool bEnemyDefeated = CombatSmokeEnemy && CombatSmokeEnemy->IsDead();
	const float BiomassGained = BroodCharacter
		? BroodCharacter->GetBiomass() - CombatSmokeInitialBiomass
		: 0.0f;

	if (bEnemyDefeated && CombatSmokePickup && BiomassGained > 0.0f)
	{
		UE_LOG(LogTemp, Display, TEXT("BROOD_COMBAT_SMOKE_PASSED: enemy defeated, drop collected, gained %.1f biomass."), BiomassGained);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_COMBAT_SMOKE_FAILED: defeated=%s biomass=%.1f."), bEnemyDefeated ? TEXT("true") : TEXT("false"), BiomassGained);
	}

	BroodQuit();
}

void ABroodPlayerController::FinishMouseAimSmokeTest()
{
	GetWorldTimerManager().ClearTimer(MouseAimSmokeTimerHandle);
	bMouseAimSmokeActive = false;

	const ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	const float DistanceMoved = BroodCharacter
		? FVector::Dist2D(MouseAimSmokeStartLocation, BroodCharacter->GetActorLocation())
		: 0.0f;
	const float AimDot = BroodCharacter
		? FVector::DotProduct(BroodCharacter->GetActorForwardVector().GetSafeNormal2D(), MouseAimSmokeTargetDirection)
		: 0.0f;
	const float CameraYawDelta = BroodCharacter
		? FMath::Abs(FMath::FindDeltaAngleDegrees(MouseAimSmokeInitialCameraYaw, BroodCharacter->GetCameraYaw()))
		: 0.0f;

	if (DistanceMoved > 1.0f && AimDot > 0.95f && CameraYawDelta > 40.0f)
	{
		UE_LOG(LogTemp, Display, TEXT("BROOD_MOUSE_AIM_SMOKE_PASSED: moved %.1f units while aim remained independent (dot %.2f), camera orbit delta %.1f."), DistanceMoved, AimDot, CameraYawDelta);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_MOUSE_AIM_SMOKE_FAILED: distance %.1f aim dot %.2f camera orbit delta %.1f."), DistanceMoved, AimDot, CameraYawDelta);
	}

	BroodQuit();
}

void ABroodPlayerController::TogglePauseMenu()
{
	if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}

void ABroodPlayerController::ShowPauseMenu()
{
	if (!PauseMenuWidget)
	{
		PauseMenuWidget = CreateWidget<UBroodPauseMenuWidget>(this, UBroodPauseMenuWidget::StaticClass());
	}

	if (!PauseMenuWidget)
	{
		return;
	}

	SetPause(true);
	PauseMenuWidget->AddToViewport(100);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	UE_LOG(LogTemp, Display, TEXT("Brood pause menu shown."));
}

void ABroodPlayerController::HidePauseMenu()
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
	}

	SetPause(false);
	RestoreGameplayInputMode();
	UE_LOG(LogTemp, Display, TEXT("Brood pause menu hidden."));
}

void ABroodPlayerController::RestoreGameplayInputMode()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}
