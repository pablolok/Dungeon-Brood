#include "Player/BroodPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Characters/BroodCharacter.h"
#include "Components/LightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PointLightComponent.h"
#include "Enemies/BroodEnemyCharacter.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextRenderActor.h"
#include "Engine/PointLight.h"
#include "Engine/DirectionalLight.h"
#include "Game/BroodGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HAL/FileManager.h"
#include "HighResScreenshot.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/Paths.h"
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

void ABroodPlayerController::BroodDungeonLevel5SmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_SMOKE_FAILED: player controller does not possess ABroodCharacter."));
		BroodQuit();
		return;
	}

	DungeonLevel5SmokeStep = 0;
	DungeonLevel5SmokeEvolutionSelections = 0;
	bDungeonLevel5SmokeBossSeen = false;
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SMOKE_STARTED: driving La Soglia Spezzata skeletons, remains, evolutions and Jailer Husk completion."));
	GetWorldTimerManager().SetTimer(DungeonLevel5SmokeTimerHandle, this, &ABroodPlayerController::RunDungeonLevel5SmokeStep, 0.25f, true, 0.25f);
}

void ABroodPlayerController::BroodCaptureWindowedSmokeScreenshot()
{
	ConsoleCommand(TEXT("DisableAllScreenMessages"));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_CLEAN_FIRST_LOOK_READY: screen debug messages hidden for first-look Level 5 screenshot."));
	if (GetPawn())
	{
		TArray<UTextRenderComponent*> PawnTextComponents;
		GetPawn()->GetComponents<UTextRenderComponent>(PawnTextComponents);
		for (UTextRenderComponent* TextComponent : PawnTextComponents)
		{
			if (TextComponent)
			{
				TextComponent->SetVisibility(false);
				TextComponent->SetHiddenInGame(true);
			}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ART_REVIEW_NAMEPLATES_HIDDEN_READY: player text render components hidden for first-look art screenshot."));
	RequestViewportScreenshot(TEXT("WindowedSmokeViewport.png"), TEXT("BROOD_WINDOWED_VIEWPORT_SCREENSHOT_REQUESTED"));
}

void ABroodPlayerController::BroodDungeonLevel5EnvironmentVisualSmokeTest()
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_ENVIRONMENT_VISUAL_SMOKE_FAILED: world missing."));
		BroodQuit();
		return;
	}

	ConsoleCommand(TEXT("DisableAllScreenMessages"));

	if (ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn()))
	{
		TArray<UTextRenderComponent*> PawnTextComponents;
		BroodCharacter->GetComponents<UTextRenderComponent>(PawnTextComponents);
		for (UTextRenderComponent* TextComponent : PawnTextComponents)
		{
			if (TextComponent)
			{
				TextComponent->SetVisibility(false);
				TextComponent->SetHiddenInGame(true);
			}
		}
	}

	for (TActorIterator<ATextRenderActor> TextIterator(GetWorld()); TextIterator; ++TextIterator)
	{
		if (ATextRenderActor* TextActor = *TextIterator)
		{
			TextActor->SetActorHiddenInGame(true);
		}
	}
	for (TActorIterator<ABroodEnemyCharacter> EnemyIterator(GetWorld()); EnemyIterator; ++EnemyIterator)
	{
		if (ABroodEnemyCharacter* Enemy = *EnemyIterator)
		{
			TArray<UTextRenderComponent*> EnemyTextComponents;
			Enemy->GetComponents<UTextRenderComponent>(EnemyTextComponents);
			for (UTextRenderComponent* TextComponent : EnemyTextComponents)
			{
				if (TextComponent)
				{
					TextComponent->SetVisibility(false);
					TextComponent->SetHiddenInGame(true);
				}
			}
		}
	}

	const FVector EnvironmentCameraLocation(0.0f, -350.0f, 5200.0f);
	const FVector EnvironmentLookAtLocation(0.0f, -350.0f, 0.0f);
	for (TActorIterator<AStaticMeshActor> MeshIterator(GetWorld()); MeshIterator; ++MeshIterator)
	{
		AStaticMeshActor* MeshActor = *MeshIterator;
		if (!MeshActor || !MeshActor->GetStaticMeshComponent() || !MeshActor->GetStaticMeshComponent()->GetStaticMesh())
		{
			continue;
		}

		const FString ActorName = MeshActor->GetName();
		const bool bFirstViewReviewCutOffender =
			ActorName.Contains(TEXT("DungeonLevel5FirstViewLeftPier")) ||
			ActorName.Contains(TEXT("DungeonLevel5FirstViewButtress")) ||
			ActorName.Contains(TEXT("DungeonLevel5FirstViewHangingChainA")) ||
			ActorName.Contains(TEXT("DungeonLevel5CreepyHangingChain_0")) ||
			ActorName.Contains(TEXT("DungeonLevel5CreepyChainShadow_0")) ||
			ActorName.Contains(TEXT("DungeonLevel5CeilingRoot_0")) ||
			ActorName.Contains(TEXT("DungeonLevel5CeilingRoot_1")) ||
			ActorName.Contains(TEXT("DungeonLevel5SurfaceLightCrack")) ||
			ActorName.Contains(TEXT("DungeonLevel5SurfaceDustBeam"));
		const bool bEnvironmentReviewProxyClutter =
			ActorName.Contains(TEXT("DungeonLevel5PrologueDryBlood")) ||
			ActorName.Contains(TEXT("DungeonLevel5PrologueFallScar")) ||
			ActorName.Contains(TEXT("DungeonLevel5PrologueSmallCarcass")) ||
			ActorName.Contains(TEXT("DungeonLevel5PrologueCarcassTail")) ||
			ActorName.Contains(TEXT("DungeonLevel5PrologueAncientMeat")) ||
			ActorName.Contains(TEXT("DungeonLevel5SurfaceTrailSlab")) ||
			ActorName.Contains(TEXT("DungeonLevel5DescentTrailSlab")) ||
			ActorName.Contains(TEXT("DungeonLevel5BrokenThresholdEdge")) ||
			ActorName.Contains(TEXT("DungeonLevel5SurfaceChoiceRune")) ||
			ActorName.Contains(TEXT("DungeonLevel5DescentChoiceRune")) ||
			ActorName.Contains(TEXT("DungeonLevel5CreepyChainShadow")) ||
			ActorName.Contains(TEXT("DungeonLevel5CreepyDryCorpseCloth")) ||
			ActorName.Contains(TEXT("DungeonLevel5CreepyDryCorpseBlood")) ||
			ActorName.Contains(TEXT("DungeonLevel5CreepySurfaceNooseShadow")) ||
			ActorName.Contains(TEXT("DungeonLevel5CreepyDescentScratch"));
		const FString MeshName = MeshActor->GetStaticMeshComponent()->GetStaticMesh()->GetName();
		const bool bReviewCameraOccluder =
			MeshName.Contains(TEXT("Ceiling")) ||
			MeshName.Contains(TEXT("Arch"));
		if (bFirstViewReviewCutOffender || bEnvironmentReviewProxyClutter || bReviewCameraOccluder)
		{
			MeshActor->SetActorHiddenInGame(true);
			MeshActor->SetActorEnableCollision(false);
		}
	}

	ACameraActor* EnvironmentCamera = GetWorld()->SpawnActor<ACameraActor>(EnvironmentCameraLocation, FRotator(-90.0f, 90.0f, 0.0f));
	if (EnvironmentCamera)
	{
		EnvironmentCamera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
		EnvironmentCamera->GetCameraComponent()->SetOrthoWidth(14000.0f);
		EnvironmentCamera->GetCameraComponent()->PostProcessSettings.bOverride_AutoExposureBias = true;
		EnvironmentCamera->GetCameraComponent()->PostProcessSettings.AutoExposureBias = 0.85f;
		EnvironmentCamera->GetCameraComponent()->PostProcessSettings.bOverride_VignetteIntensity = true;
		EnvironmentCamera->GetCameraComponent()->PostProcessSettings.VignetteIntensity = 0.0f;
		SetViewTarget(EnvironmentCamera);
	}
	if (ADirectionalLight* LayoutReviewLight = GetWorld()->SpawnActor<ADirectionalLight>(FVector(0.0f, -350.0f, 2400.0f), FRotator(-90.0f, 0.0f, 0.0f)))
	{
		LayoutReviewLight->SetActorLabel(TEXT("DungeonLevel5RuntimeLayoutReviewLight"));
		if (ULightComponent* ReviewLightComponent = LayoutReviewLight->GetLightComponent())
		{
			ReviewLightComponent->SetIntensity(2.2f);
			ReviewLightComponent->SetLightColor(FLinearColor(0.78f, 0.88f, 1.0f));
		}
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ENVIRONMENT_FOREGROUND_OCCLUDERS_HIDDEN_READY: Level 5 environment review uses a dedicated ARPG camera placed inside the spawn threshold sightline."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ENVIRONMENT_LEFT_EDGE_CLEANUP_READY: Level 5 environment review hides first-view edge-only cut offenders without changing gameplay placement."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ENVIRONMENT_PROXY_CLUTTER_HIDDEN_READY: Level 5 environment review hides primitive proxy dressing so asset floor, dungeon architecture, player and enemies carry the frame."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ENVIRONMENT_POE_CAMERA_READY: Level 5 environment review uses a top-down orthographic layout camera while the dungeon topology is being rebuilt."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ENVIRONMENT_VISUAL_SMOKE_STARTED: capturing Level 5 dungeon enclosure, modular floor, route graph and room scale reference."));
	GetWorldTimerManager().SetTimer(DungeonLevel5EnvironmentVisualScreenshotTimerHandle, this, &ABroodPlayerController::CaptureDungeonLevel5EnvironmentVisualScreenshot, 0.65f, false);
}

void ABroodPlayerController::BroodDungeonLevel5CompletionVisualSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_VISUAL_SMOKE_FAILED: player controller does not possess ABroodCharacter."));
		BroodQuit();
		return;
	}

	BroodCharacter->SetActorLocation(FVector(420.0f, -940.0f, 120.0f), false, nullptr, ETeleportType::TeleportPhysics);
	DungeonLevel5CompletionVisualSmokeStep = 0;
	DungeonLevel5CompletionVisualSmokeEvolutionSelections = 0;
	DungeonLevel5CompletionVisualReadyFrames = 0;
	bDungeonLevel5CompletionVisualScreenshotRequested = false;
	bDungeonLevel5ExitShowcaseSmokeActive = false;
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_VISUAL_SMOKE_STARTED: driving Level 5 to unlocked descent screenshot."));
	GetWorldTimerManager().SetTimer(DungeonLevel5CompletionVisualSmokeTimerHandle, this, &ABroodPlayerController::RunDungeonLevel5CompletionVisualSmokeStep, 0.25f, true, 0.25f);
}

void ABroodPlayerController::BroodDungeonLevel5ExitShowcaseSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_SMOKE_FAILED: player controller does not possess ABroodCharacter."));
		BroodQuit();
		return;
	}

	BroodCharacter->SetActorLocation(FVector(420.0f, -940.0f, 120.0f), false, nullptr, ETeleportType::TeleportPhysics);
	DungeonLevel5CompletionVisualSmokeStep = 0;
	DungeonLevel5CompletionVisualSmokeEvolutionSelections = 0;
	DungeonLevel5CompletionVisualReadyFrames = 0;
	bDungeonLevel5CompletionVisualScreenshotRequested = false;
	bDungeonLevel5ExitShowcaseSmokeActive = true;
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_SMOKE_STARTED: driving Level 5 to angled lower-exit art review screenshot."));
	GetWorldTimerManager().SetTimer(DungeonLevel5CompletionVisualSmokeTimerHandle, this, &ABroodPlayerController::RunDungeonLevel5CompletionVisualSmokeStep, 0.25f, true, 0.25f);
}

void ABroodPlayerController::BroodDungeonLevel5JailerVisualSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VISUAL_SMOKE_FAILED: player controller does not possess ABroodCharacter."));
		BroodQuit();
		return;
	}

	BroodCharacter->SetActorLocation(FVector(620.0f, -860.0f, 120.0f), false, nullptr, ETeleportType::TeleportPhysics);
	DungeonLevel5JailerVisualSmokeStep = 0;
	DungeonLevel5JailerVisualSmokeEvolutionSelections = 0;
	DungeonLevel5JailerVisualReadyFrames = 0;
	bDungeonLevel5JailerVisualScreenshotRequested = false;
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VISUAL_SMOKE_STARTED: driving Level 5 to live Jailer Husk screenshot."));
	GetWorldTimerManager().SetTimer(DungeonLevel5JailerVisualSmokeTimerHandle, this, &ABroodPlayerController::RunDungeonLevel5JailerVisualSmokeStep, 0.25f, true, 0.25f);
}

void ABroodPlayerController::BroodDungeonLevel5JailerAssetReviewSmokeTest()
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_ASSET_REVIEW_FAILED: world missing."));
		BroodQuit();
		return;
	}

	if (ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn()))
	{
		BroodCharacter->SetActorHiddenInGame(true);
		BroodCharacter->SetActorEnableCollision(false);
	}

	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		if (Actor && Actor != GetPawn())
		{
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorEnableCollision(false);
		}
	}
	for (TActorIterator<AStaticMeshActor> MeshIterator(GetWorld()); MeshIterator; ++MeshIterator)
	{
		if (AStaticMeshActor* MeshActor = *MeshIterator)
		{
			MeshActor->SetActorHiddenInGame(true);
			MeshActor->SetActorEnableCollision(false);
		}
	}
	for (TActorIterator<ATextRenderActor> TextIterator(GetWorld()); TextIterator; ++TextIterator)
	{
		if (ATextRenderActor* TextActor = *TextIterator)
		{
			TextActor->SetActorHiddenInGame(true);
		}
	}
	for (TActorIterator<ABiomassPickup> PickupIterator(GetWorld()); PickupIterator; ++PickupIterator)
	{
		PickupIterator->SetActorHiddenInGame(true);
		PickupIterator->SetActorEnableCollision(false);
	}
	for (TActorIterator<ABroodEnemyCharacter> EnemyIterator(GetWorld()); EnemyIterator; ++EnemyIterator)
	{
		if (ABroodEnemyCharacter* Enemy = *EnemyIterator)
		{
			Enemy->SetActorHiddenInGame(true);
			Enemy->SetActorEnableCollision(false);
		}
	}

	UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"));
	UStaticMesh* WallMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"));
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	auto TintReviewMesh = [](UStaticMeshComponent* MeshComponent, const FLinearColor& Color)
	{
		if (!MeshComponent)
		{
			return;
		}

		if (UMaterialInstanceDynamic* Material = MeshComponent->CreateAndSetMaterialInstanceDynamic(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), Color);
			Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
		}
	};
	auto SpawnJailerReviewStageMesh = [this, FloorMesh, WallMesh, CubeMesh, TintReviewMesh](
		const FString& Name,
		UStaticMesh* Mesh,
		const FVector& Location,
		const FVector& Scale,
		const FLinearColor& FallbackColor,
		const FRotator& Rotation = FRotator::ZeroRotator)
	{
		if (!Mesh && !CubeMesh)
		{
			return;
		}

		AStaticMeshActor* StageActor = GetWorld()->SpawnActor<AStaticMeshActor>(Location, Rotation);
		if (!StageActor || !StageActor->GetStaticMeshComponent())
		{
			return;
		}

		StageActor->Rename(*Name);
		StageActor->SetMobility(EComponentMobility::Movable);
		StageActor->SetActorScale3D(Scale);
		UStaticMeshComponent* MeshComponent = StageActor->GetStaticMeshComponent();
		MeshComponent->SetMobility(EComponentMobility::Movable);
		MeshComponent->SetStaticMesh(Mesh ? Mesh : CubeMesh);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (!Mesh)
		{
			TintReviewMesh(MeshComponent, FallbackColor);
		}
	};

	for (int32 Index = -4; Index <= 4; ++Index)
	{
		SpawnJailerReviewStageMesh(
			FString::Printf(TEXT("JailerAssetReviewFloor_%d"), Index + 4),
			FloorMesh,
			FVector(Index * 250.0f, 0.0f, -54.0f),
			FloorMesh ? FVector(0.52f) : FVector(2.5f, 3.0f, 0.05f),
			FLinearColor(0.055f, 0.064f, 0.068f));
	}
	for (int32 Index = -3; Index <= 3; ++Index)
	{
		SpawnJailerReviewStageMesh(
			FString::Printf(TEXT("JailerAssetReviewBackWall_%d"), Index + 3),
			WallMesh,
			FVector(Index * 310.0f, -250.0f, 92.0f),
			WallMesh ? FVector(0.62f) : FVector(3.1f, 0.12f, 1.8f),
			FLinearColor(0.026f, 0.030f, 0.032f),
			FRotator(0.0f, 0.0f, 0.0f));
	}
	SpawnJailerReviewStageMesh(TEXT("JailerAssetReviewDarkBackdrop"), CubeMesh, FVector(0.0f, -360.0f, 210.0f), FVector(22.0f, 0.16f, 7.4f), FLinearColor(0.010f, 0.014f, 0.016f));
	SpawnJailerReviewStageMesh(TEXT("JailerAssetReviewDarkForegroundSeal"), CubeMesh, FVector(0.0f, 170.0f, -68.0f), FVector(22.0f, 4.0f, 0.10f), FLinearColor(0.010f, 0.014f, 0.016f));
	if (APointLight* KeyLight = GetWorld()->SpawnActor<APointLight>(FVector(-210.0f, 300.0f, 240.0f), FRotator::ZeroRotator))
	{
		KeyLight->Rename(TEXT("JailerAssetReviewKeyLight"));
		KeyLight->SetMobility(EComponentMobility::Movable);
		KeyLight->SetLightColor(FLinearColor(0.95f, 0.62f, 0.32f));
		KeyLight->SetBrightness(4200.0f);
		if (UPointLightComponent* PointLightComponent = KeyLight->FindComponentByClass<UPointLightComponent>())
		{
			PointLightComponent->SetAttenuationRadius(1100.0f);
		}
	}
	if (APointLight* FillLight = GetWorld()->SpawnActor<APointLight>(FVector(220.0f, -120.0f, 210.0f), FRotator::ZeroRotator))
	{
		FillLight->Rename(TEXT("JailerAssetReviewColdFillLight"));
		FillLight->SetMobility(EComponentMobility::Movable);
		FillLight->SetLightColor(FLinearColor(0.12f, 0.42f, 0.34f));
		FillLight->SetBrightness(2400.0f);
		if (UPointLightComponent* PointLightComponent = FillLight->FindComponentByClass<UPointLightComponent>())
		{
			PointLightComponent->SetAttenuationRadius(980.0f);
		}
	}
	if (APointLight* FrontFillLight = GetWorld()->SpawnActor<APointLight>(FVector(0.0f, 520.0f, 180.0f), FRotator::ZeroRotator))
	{
		FrontFillLight->Rename(TEXT("JailerAssetReviewFrontFillLight"));
		FrontFillLight->SetMobility(EComponentMobility::Movable);
		FrontFillLight->SetLightColor(FLinearColor(0.40f, 0.66f, 0.58f));
		FrontFillLight->SetBrightness(2200.0f);
		if (UPointLightComponent* PointLightComponent = FrontFillLight->FindComponentByClass<UPointLightComponent>())
		{
			PointLightComponent->SetAttenuationRadius(1250.0f);
			PointLightComponent->SetCastShadows(false);
		}
	}

	const FVector ReviewOrigin(0.0f, 0.0f, 8.0f);
	const TArray<FVector> ReviewLocations = {
		ReviewOrigin + FVector(-360.0f, 0.0f, 0.0f),
		ReviewOrigin + FVector(-120.0f, 0.0f, 0.0f),
		ReviewOrigin + FVector(120.0f, 0.0f, 0.0f),
		ReviewOrigin + FVector(360.0f, 0.0f, 0.0f)
	};
	const TArray<FRotator> ReviewRotations = {
		FRotator(0.0f, 90.0f, 0.0f),
		FRotator(0.0f, -90.0f, 0.0f),
		FRotator(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, 180.0f, 0.0f)
	};
	const TArray<FString> ReviewLabels = {
		TEXT("Front"),
		TEXT("Back"),
		TEXT("Left"),
		TEXT("Right")
	};

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int32 Index = 0; Index < ReviewLocations.Num(); ++Index)
	{
		ABroodEnemyCharacter* ReviewJailer = GetWorld()->SpawnActor<ABroodEnemyCharacter>(ReviewLocations[Index], ReviewRotations[Index], SpawnParams);
		if (!ReviewJailer)
		{
			continue;
		}

		ReviewJailer->Rename(*FString::Printf(TEXT("JailerHuskAssetReview_%s"), *ReviewLabels[Index]));
		ReviewJailer->InitializeEnemy(EBroodEnemyType::BossBrute);
		ReviewJailer->SetActorScale3D(FVector(1.35f));
		if (UCharacterMovementComponent* Movement = ReviewJailer->GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
		}
		TArray<UTextRenderComponent*> TextComponents;
		ReviewJailer->GetComponents<UTextRenderComponent>(TextComponents);
		for (UTextRenderComponent* TextComponent : TextComponents)
		{
			if (TextComponent)
			{
				TextComponent->SetVisibility(false);
				TextComponent->SetHiddenInGame(true);
			}
		}
	}

	const FVector CameraLocation(0.0f, 920.0f, 115.0f);
	const FVector LookAtLocation(0.0f, 0.0f, 72.0f);
	ACameraActor* ReviewCamera = GetWorld()->SpawnActor<ACameraActor>(CameraLocation, (LookAtLocation - CameraLocation).Rotation());
	if (ReviewCamera)
	{
		ReviewCamera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
		ReviewCamera->GetCameraComponent()->SetOrthoWidth(980.0f);
		SetViewTarget(ReviewCamera);
	}

	ConsoleCommand(TEXT("DisableAllScreenMessages"));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_ASSET_REVIEW_STARTED: isolated four-angle Jailer Husk asset review spawned without dungeon environment."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_ASSET_REVIEW_TURNAROUND_READY: front, back, left and right Jailer Husk instances are visible in one clean review shot."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_ASSET_REVIEW_CLOSED_STAGE_READY: Jailer Husk asset review hides world/background actors and uses dungeon floor, wall, dark seals and review lights."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_ASSET_REVIEW_LIGHTING_READY: Jailer Husk asset review uses stronger key, fill and front lights so all four silhouettes are inspectable."));
	GetWorldTimerManager().SetTimer(DungeonLevel5JailerAssetReviewScreenshotTimerHandle, this, &ABroodPlayerController::CaptureDungeonLevel5JailerAssetReviewScreenshot, 0.75f, false);
}

void ABroodPlayerController::BroodDungeonLevel5LarvaAssetReviewSmokeTest()
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_FAILED: world missing."));
		BroodQuit();
		return;
	}

	ConsoleCommand(TEXT("DisableAllScreenMessages"));

	if (APawn* ExistingPawn = GetPawn())
	{
		ExistingPawn->SetActorHiddenInGame(true);
		ExistingPawn->SetActorEnableCollision(false);
	}
	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		if (Actor && Actor != GetPawn())
		{
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorEnableCollision(false);
		}
	}
	for (TActorIterator<AStaticMeshActor> MeshIterator(GetWorld()); MeshIterator; ++MeshIterator)
	{
		if (AStaticMeshActor* MeshActor = *MeshIterator)
		{
			MeshActor->SetActorHiddenInGame(true);
			MeshActor->SetActorEnableCollision(false);
		}
	}
	for (TActorIterator<ABroodEnemyCharacter> EnemyIterator(GetWorld()); EnemyIterator; ++EnemyIterator)
	{
		if (ABroodEnemyCharacter* Enemy = *EnemyIterator)
		{
			Enemy->SetActorHiddenInGame(true);
			Enemy->SetActorEnableCollision(false);
		}
	}
	for (TActorIterator<ATextRenderActor> TextIterator(GetWorld()); TextIterator; ++TextIterator)
	{
		if (ATextRenderActor* TextActor = *TextIterator)
		{
			TextActor->SetActorHiddenInGame(true);
		}
	}

	UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"));
	UStaticMesh* EdgeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"));
	UStaticMesh* WallMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"));
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	auto ApplyReviewTint = [](UStaticMeshComponent* MeshComponent, const FLinearColor& Color)
	{
		if (!MeshComponent)
		{
			return;
		}

		if (UMaterialInstanceDynamic* Material = MeshComponent->CreateAndSetMaterialInstanceDynamic(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), Color);
			Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
		}
	};

	for (int32 Index = -3; Index <= 3; ++Index)
	{
		if (AStaticMeshActor* FloorActor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(Index * 300.0f, 0.0f, 18.0f), FRotator::ZeroRotator))
		{
			FloorActor->Rename(*FString::Printf(TEXT("LarvaAssetReviewFloor_%d"), Index + 3));
			FloorActor->SetMobility(EComponentMobility::Movable);
			FloorActor->SetActorScale3D(FloorMesh ? FVector(1.35f) : FVector(3.0f, 3.5f, 0.05f));
			if (UStaticMeshComponent* FloorComponent = FloorActor->GetStaticMeshComponent())
			{
				FloorComponent->SetMobility(EComponentMobility::Movable);
				FloorComponent->SetStaticMesh(FloorMesh ? FloorMesh : CubeMesh);
				FloorComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
	for (int32 Index = -3; Index <= 3; ++Index)
	{
		if (AStaticMeshActor* RearEdgeActor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(Index * 300.0f, -178.0f, 28.0f), FRotator::ZeroRotator))
		{
			RearEdgeActor->Rename(*FString::Printf(TEXT("LarvaAssetReviewRearDungeonEdge_%d"), Index + 3));
			RearEdgeActor->SetMobility(EComponentMobility::Movable);
			RearEdgeActor->SetActorScale3D(EdgeMesh ? FVector(0.82f) : FVector(3.0f, 0.08f, 0.12f));
			if (UStaticMeshComponent* EdgeComponent = RearEdgeActor->GetStaticMeshComponent())
			{
				EdgeComponent->SetMobility(EComponentMobility::Movable);
				EdgeComponent->SetStaticMesh(EdgeMesh ? EdgeMesh : CubeMesh);
				EdgeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				if (!EdgeMesh)
				{
					ApplyReviewTint(EdgeComponent, FLinearColor(0.045f, 0.050f, 0.052f));
				}
			}
		}
	}
	for (int32 Index = -2; Index <= 2; ++Index)
	{
		if (AStaticMeshActor* WallActor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(Index * 330.0f, -310.0f, 142.0f), FRotator::ZeroRotator))
		{
			WallActor->Rename(*FString::Printf(TEXT("LarvaAssetReviewDungeonBackWall_%d"), Index + 2));
			WallActor->SetMobility(EComponentMobility::Movable);
			WallActor->SetActorScale3D(WallMesh ? FVector(0.88f) : FVector(3.2f, 0.14f, 1.95f));
			if (UStaticMeshComponent* WallComponent = WallActor->GetStaticMeshComponent())
			{
				WallComponent->SetMobility(EComponentMobility::Movable);
				WallComponent->SetStaticMesh(WallMesh ? WallMesh : CubeMesh);
				WallComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				if (!WallMesh)
				{
					ApplyReviewTint(WallComponent, FLinearColor(0.034f, 0.038f, 0.040f));
				}
			}
		}
	}
	if (AStaticMeshActor* BackdropActor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(0.0f, -365.0f, 260.0f), FRotator::ZeroRotator))
	{
		BackdropActor->Rename(TEXT("LarvaAssetReviewDarkBackdrop"));
		BackdropActor->SetMobility(EComponentMobility::Movable);
		BackdropActor->SetActorScale3D(FVector(26.0f, 0.16f, 12.0f));
		if (UStaticMeshComponent* BackdropComponent = BackdropActor->GetStaticMeshComponent())
		{
			BackdropComponent->SetMobility(EComponentMobility::Movable);
			BackdropComponent->SetStaticMesh(CubeMesh);
			BackdropComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ApplyReviewTint(BackdropComponent, FLinearColor(0.012f, 0.016f, 0.018f));
		}
	}
	if (AStaticMeshActor* CeilingSealActor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(0.0f, -180.0f, 445.0f), FRotator::ZeroRotator))
	{
		CeilingSealActor->Rename(TEXT("LarvaAssetReviewDarkCeilingSeal"));
		CeilingSealActor->SetMobility(EComponentMobility::Movable);
		CeilingSealActor->SetActorScale3D(FVector(26.0f, 5.0f, 0.34f));
		if (UStaticMeshComponent* CeilingSealComponent = CeilingSealActor->GetStaticMeshComponent())
		{
			CeilingSealComponent->SetMobility(EComponentMobility::Movable);
			CeilingSealComponent->SetStaticMesh(CubeMesh);
			CeilingSealComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ApplyReviewTint(CeilingSealComponent, FLinearColor(0.010f, 0.014f, 0.016f));
		}
	}
	if (AStaticMeshActor* BaseActor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(0.0f, 150.0f, 0.0f), FRotator::ZeroRotator))
	{
		BaseActor->Rename(TEXT("LarvaAssetReviewDarkForegroundBase"));
		BaseActor->SetMobility(EComponentMobility::Movable);
		BaseActor->SetActorScale3D(FVector(26.0f, 5.2f, 0.10f));
		if (UStaticMeshComponent* BaseComponent = BaseActor->GetStaticMeshComponent())
		{
			BaseComponent->SetMobility(EComponentMobility::Movable);
			BaseComponent->SetStaticMesh(CubeMesh);
			BaseComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ApplyReviewTint(BaseComponent, FLinearColor(0.010f, 0.014f, 0.016f));
		}
	}

	const FVector ReviewOrigin(0.0f, 0.0f, 96.0f);
	const TArray<FVector> ReviewLocations = {
		ReviewOrigin + FVector(-300.0f, 0.0f, 0.0f),
		ReviewOrigin + FVector(-100.0f, 0.0f, 0.0f),
		ReviewOrigin + FVector(100.0f, 0.0f, 0.0f),
		ReviewOrigin + FVector(300.0f, 0.0f, 0.0f)
	};
	const TArray<FRotator> ReviewRotations = {
		FRotator(0.0f, 90.0f, 0.0f),
		FRotator(0.0f, -90.0f, 0.0f),
		FRotator(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, 180.0f, 0.0f)
	};
	const TArray<FString> ReviewLabels = {
		TEXT("Front"),
		TEXT("Back"),
		TEXT("Left"),
		TEXT("Right")
	};

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int32 Index = 0; Index < ReviewLocations.Num(); ++Index)
	{
		ABroodCharacter* ReviewLarva = GetWorld()->SpawnActor<ABroodCharacter>(ReviewLocations[Index], ReviewRotations[Index], SpawnParams);
		if (!ReviewLarva)
		{
			continue;
		}

		ReviewLarva->Rename(*FString::Printf(TEXT("LarvaAssetReview_%s"), *ReviewLabels[Index]));
		ReviewLarva->SetActorScale3D(FVector(1.45f));
		if (UCharacterMovementComponent* Movement = ReviewLarva->GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
		}

		TArray<UTextRenderComponent*> TextComponents;
		ReviewLarva->GetComponents<UTextRenderComponent>(TextComponents);
		for (UTextRenderComponent* TextComponent : TextComponents)
		{
			if (TextComponent)
			{
				TextComponent->SetVisibility(false);
				TextComponent->SetHiddenInGame(true);
			}
		}

		TArray<UStaticMeshComponent*> MeshComponents;
		ReviewLarva->GetComponents<UStaticMeshComponent>(MeshComponents);
		for (UStaticMeshComponent* MeshComponent : MeshComponents)
		{
			if (MeshComponent && MeshComponent->GetName().Contains(TEXT("FacingVisual")))
			{
				MeshComponent->SetVisibility(false);
				MeshComponent->SetHiddenInGame(true);
			}
		}
	}

	if (APointLight* KeyLight = GetWorld()->SpawnActor<APointLight>(FVector(-160.0f, 300.0f, 260.0f), FRotator::ZeroRotator))
	{
		KeyLight->Rename(TEXT("LarvaAssetReviewKeyLight"));
		KeyLight->SetMobility(EComponentMobility::Movable);
		KeyLight->SetLightColor(FLinearColor(0.55f, 0.95f, 0.62f));
		KeyLight->SetBrightness(2200.0f);
		if (UPointLightComponent* PointLightComponent = KeyLight->FindComponentByClass<UPointLightComponent>())
		{
			PointLightComponent->SetAttenuationRadius(620.0f);
		}
	}
	if (APointLight* FillLight = GetWorld()->SpawnActor<APointLight>(FVector(180.0f, -220.0f, 210.0f), FRotator::ZeroRotator))
	{
		FillLight->Rename(TEXT("LarvaAssetReviewFillLight"));
		FillLight->SetMobility(EComponentMobility::Movable);
		FillLight->SetLightColor(FLinearColor(0.14f, 0.36f, 0.28f));
		FillLight->SetBrightness(1250.0f);
		if (UPointLightComponent* PointLightComponent = FillLight->FindComponentByClass<UPointLightComponent>())
		{
			PointLightComponent->SetAttenuationRadius(520.0f);
		}
	}

	const FVector CameraLocation(0.0f, 800.0f, 165.0f);
	const FVector LookAtLocation(0.0f, 0.0f, 72.0f);
	ACameraActor* ReviewCamera = GetWorld()->SpawnActor<ACameraActor>(CameraLocation, (LookAtLocation - CameraLocation).Rotation());
	if (ReviewCamera)
	{
		ReviewCamera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
		ReviewCamera->GetCameraComponent()->SetOrthoWidth(1120.0f);
		SetViewTarget(ReviewCamera);
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_STARTED: isolated four-angle larva asset review spawned without dungeon environment."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_TURNAROUND_READY: front, back, left and right larva instances are visible in one clean review shot."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_DIRECTION_MARKER_HIDDEN_READY: larva review hides the prototype FacingVisual marker so only creature silhouette remains."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_STAGE_READY: larva review uses repeated dungeon floor assets and a dark backdrop instead of technical viewport background."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_CLOSED_STAGE_READY: larva review hides editor/background actors and uses dungeon wall, edge, floor, dark backdrop, ceiling seal and foreground seal for a closed asset-review frame."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_FRAMING_READY: larva review widens the orthographic turnaround and improves key/fill light so all four silhouettes stay inspectable."));
	GetWorldTimerManager().SetTimer(DungeonLevel5LarvaAssetReviewScreenshotTimerHandle, this, &ABroodPlayerController::CaptureDungeonLevel5LarvaAssetReviewScreenshot, 0.75f, false);
}

void ABroodPlayerController::BroodDungeonLevel5SurfaceTrialSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr;
	if (!BroodCharacter || !BroodGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_FAILED: pawn or game mode missing."));
		BroodQuit();
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_STARTED: entering the Level 5 surface temptation branch."));
	BroodCharacter->SetActorLocation(FVector(-1180.0f, 1320.0f, 120.0f), false, nullptr, ETeleportType::TeleportPhysics);
	BroodGameMode->TriggerDungeonLevel5SurfaceTrial(BroodCharacter);
	GetWorldTimerManager().SetTimer(DungeonLevel5SurfaceTrialSmokeQuitTimerHandle, this, &ABroodPlayerController::FinishDungeonLevel5SurfaceTrialSmokeTest, 0.6f, false);
}

void ABroodPlayerController::BroodDungeonLevel5SurfaceTrialVisualSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr;
	if (!BroodCharacter || !BroodGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_VISUAL_SMOKE_FAILED: pawn or game mode missing."));
		BroodQuit();
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_VISUAL_SMOKE_STARTED: capturing surface temptation/death review screenshot."));
	BroodCharacter->SetActorLocation(FVector(-1180.0f, 1320.0f, 120.0f), false, nullptr, ETeleportType::TeleportPhysics);
	BroodGameMode->TriggerDungeonLevel5SurfaceTrial(BroodCharacter);
	BroodCharacter->SetActorHiddenInGame(true);
	BroodCharacter->SetActorEnableCollision(false);

	for (TActorIterator<AStaticMeshActor> MeshIterator(GetWorld()); MeshIterator; ++MeshIterator)
	{
		AStaticMeshActor* MeshActor = *MeshIterator;
		if (!MeshActor)
		{
			continue;
		}

		const FString ActorName = MeshActor->GetName();
		if (ActorName.Contains(TEXT("DungeonLevel5RouteWall")) ||
			ActorName.Contains(TEXT("DungeonLevel5RouteShortWall")) ||
			ActorName.Contains(TEXT("DungeonLevel5RouteLow")) ||
			ActorName.Contains(TEXT("DungeonLevel5RouteEdge")) ||
			ActorName.Contains(TEXT("DungeonLevel5CellBlock")) ||
			ActorName.Contains(TEXT("UpperSurfaceDoorway")))
		{
			MeshActor->SetActorHiddenInGame(true);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_OCCLUDERS_HIDDEN_READY: known route and doorway occluders hidden for surface trial screenshot only."));

	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"));
	UStaticMesh* WallMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"));
	UStaticMesh* EdgeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"));
	auto TintMesh = [](UStaticMeshComponent* MeshComponent, const FLinearColor& Color)
	{
		if (!MeshComponent)
		{
			return;
		}

		UMaterialInstanceDynamic* Material = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
		if (Material)
		{
			Material->SetVectorParameterValue(TEXT("Color"), Color);
			Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
		}
	};
	auto SpawnReviewMesh = [this, TintMesh](const FString& Name, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FLinearColor& Color, const FRotator& Rotation = FRotator::ZeroRotator)
	{
		if (!Mesh)
		{
			return;
		}

		AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(Location, Rotation);
		if (!MeshActor)
		{
			return;
		}

		MeshActor->Rename(*Name);
		MeshActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
		MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
		MeshActor->SetActorScale3D(Scale);
		MeshActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (Color != FLinearColor::Transparent)
		{
			TintMesh(MeshActor->GetStaticMeshComponent(), Color);
		}
	};
	auto SpawnReviewHumanoid = [CubeMesh, CylinderMesh, SphereMesh, SpawnReviewMesh](const FString& Prefix, const FVector& BaseLocation, const FLinearColor& ArmorColor, bool bKnight, bool bArcher)
	{
		const FLinearColor FleshColor(0.72f, 0.52f, 0.34f);
		const FLinearColor WeaponColor(0.92f, 0.88f, 0.72f);
		SpawnReviewMesh(Prefix + TEXT("_ReviewBody"), CubeMesh, BaseLocation + FVector(0.0f, 0.0f, 42.0f), bKnight ? FVector(0.72f, 0.42f, 0.40f) : FVector(0.54f, 0.34f, 0.32f), ArmorColor);
		SpawnReviewMesh(Prefix + TEXT("_ReviewHead"), SphereMesh, BaseLocation + FVector(0.0f, 0.0f, 94.0f), bKnight ? FVector(0.34f) : FVector(0.28f), FleshColor);
		SpawnReviewMesh(Prefix + TEXT("_ReviewLegs"), CubeMesh, BaseLocation + FVector(0.0f, -48.0f, 30.0f), bKnight ? FVector(0.58f, 0.22f, 0.18f) : FVector(0.42f, 0.18f, 0.16f), ArmorColor);
		if (bArcher)
		{
			SpawnReviewMesh(Prefix + TEXT("_ReviewBow"), CylinderMesh, BaseLocation + FVector(72.0f, 0.0f, 60.0f), FVector(0.06f, 0.06f, 0.92f), WeaponColor, FRotator(0.0f, 0.0f, 86.0f));
		}
		else
		{
			SpawnReviewMesh(Prefix + TEXT("_ReviewWeapon"), CylinderMesh, BaseLocation + FVector(82.0f, 0.0f, 70.0f), FVector(0.07f, 0.07f, bKnight ? 1.25f : 0.88f), WeaponColor, FRotator(0.0f, 0.0f, 62.0f));
		}
		if (bKnight)
		{
			SpawnReviewMesh(Prefix + TEXT("_ReviewShield"), CubeMesh, BaseLocation + FVector(-84.0f, 0.0f, 62.0f), FVector(0.16f, 0.48f, 0.30f), ArmorColor);
		}
	};
	const FVector ReviewBoardBase(-1040.0f, 300.0f, 920.0f);
	const FLinearColor KeepMaterialColor = FLinearColor::Transparent;
	for (int32 X = -1; X <= 1; ++X)
	{
		for (int32 Y = -1; Y <= 1; ++Y)
		{
			SpawnReviewMesh(
				FString::Printf(TEXT("SurfaceTrialReviewDungeonFloor_%d_%d"), X, Y),
				FloorMesh ? FloorMesh : CubeMesh,
				ReviewBoardBase + FVector(X * 260.0f, Y * 230.0f, 0.0f),
				FloorMesh ? FVector(0.55f) : FVector(2.5f, 2.2f, 0.05f),
				FloorMesh ? KeepMaterialColor : FLinearColor(0.055f, 0.018f, 0.016f));
		}
	}
	SpawnReviewMesh(TEXT("SurfaceTrialReviewRearWall"), WallMesh ? WallMesh : CubeMesh, ReviewBoardBase + FVector(0.0f, -410.0f, 120.0f), WallMesh ? FVector(0.62f) : FVector(4.2f, 0.12f, 1.4f), WallMesh ? KeepMaterialColor : FLinearColor(0.075f, 0.064f, 0.058f), FRotator(0.0f, 0.0f, 0.0f));
	SpawnReviewMesh(TEXT("SurfaceTrialReviewFrontEdge"), EdgeMesh ? EdgeMesh : CubeMesh, ReviewBoardBase + FVector(0.0f, 420.0f, 20.0f), EdgeMesh ? FVector(0.74f) : FVector(4.2f, 0.10f, 0.08f), EdgeMesh ? KeepMaterialColor : FLinearColor(0.075f, 0.064f, 0.058f));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_ASSET_STAGE_READY: surface trial review uses MedievalDungeon floor, wall and edge assets instead of a checker platform."));
	SpawnReviewHumanoid(TEXT("SurfaceTrialReviewGuardLeft"), ReviewBoardBase + FVector(-260.0f, -80.0f, 60.0f), FLinearColor(0.76f, 0.74f, 0.64f), false, false);
	SpawnReviewHumanoid(TEXT("SurfaceTrialReviewGuardRight"), ReviewBoardBase + FVector(-80.0f, 90.0f, 60.0f), FLinearColor(0.76f, 0.74f, 0.64f), false, false);
	SpawnReviewHumanoid(TEXT("SurfaceTrialReviewArcher"), ReviewBoardBase + FVector(90.0f, -95.0f, 60.0f), FLinearColor(0.42f, 0.50f, 0.62f), false, true);
	SpawnReviewHumanoid(TEXT("SurfaceTrialReviewKnight"), ReviewBoardBase + FVector(280.0f, 40.0f, 60.0f), FLinearColor(0.94f, 0.86f, 0.54f), true, false);
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_STAGED_SILHOUETTES_READY: visual smoke spawned elevated guard, archer and knight silhouettes for screenshot review."));

	const FVector ReviewLocation(-1040.0f, 300.0f, 1820.0f);
	ACameraActor* ReviewCamera = GetWorld()->SpawnActor<ACameraActor>(ReviewLocation, FRotator(-90.0f, 0.0f, 0.0f));
	if (ReviewCamera)
	{
		ReviewCamera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
		ReviewCamera->GetCameraComponent()->SetOrthoWidth(1120.0f);
		SetViewTarget(ReviewCamera);
	}
	ConsoleCommand(TEXT("DisableAllScreenMessages"));
	GetWorldTimerManager().SetTimer(DungeonLevel5SurfaceTrialVisualScreenshotTimerHandle, this, &ABroodPlayerController::CaptureDungeonLevel5SurfaceTrialVisualScreenshot, 0.45f, false);
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

void ABroodPlayerController::FinishDungeonLevel5SurfaceTrialSmokeTest()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	if (!BroodCharacter || !BroodCharacter->IsDead())
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_FAILED: surface branch did not kill the larva."));
		BroodQuit();
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_PASSED: surface temptation kills the incomplete larva and restart returns to Dungeon Level 5."));
	BroodQuit();
}

void ABroodPlayerController::CaptureDungeonLevel5JailerAssetReviewScreenshot()
{
	RequestViewportScreenshot(TEXT("WindowedJailerHuskAssetReviewViewport.png"), TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_ASSET_REVIEW_SCREENSHOT_REQUESTED"));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_ASSET_REVIEW_PASSED: isolated four-angle Jailer Husk screenshot requested."));
	GetWorldTimerManager().SetTimer(DungeonLevel5JailerAssetReviewQuitTimerHandle, this, &ABroodPlayerController::BroodQuit, 1.25f, false);
}

void ABroodPlayerController::CaptureDungeonLevel5LarvaAssetReviewScreenshot()
{
	RequestViewportScreenshot(TEXT("WindowedLarvaAssetReviewViewport.png"), TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_SCREENSHOT_REQUESTED"));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_LARVA_ASSET_REVIEW_PASSED: isolated four-angle larva screenshot requested."));
	GetWorldTimerManager().SetTimer(DungeonLevel5LarvaAssetReviewQuitTimerHandle, this, &ABroodPlayerController::BroodQuit, 1.25f, false);
}

void ABroodPlayerController::CaptureDungeonLevel5EnvironmentVisualScreenshot()
{
	RequestViewportScreenshot(TEXT("WindowedLevel5EnvironmentViewport.png"), TEXT("BROOD_DUNGEON_LEVEL5_ENVIRONMENT_SCREENSHOT_REQUESTED"));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ENVIRONMENT_VISUAL_SMOKE_PASSED: Path-of-Exile-style Level 5 environment screenshot requested."));
	GetWorldTimerManager().SetTimer(DungeonLevel5EnvironmentVisualSmokeQuitTimerHandle, this, &ABroodPlayerController::BroodQuit, 1.25f, false);
}

void ABroodPlayerController::CaptureDungeonLevel5SurfaceTrialVisualScreenshot()
{
	ConsoleCommand(TEXT("DisableAllScreenMessages"));
	RequestViewportScreenshot(TEXT("WindowedSurfaceTrialViewport.png"), TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SCREENSHOT_REQUESTED"));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_VISUAL_SMOKE_PASSED: surface trial screenshot requested after staged silhouettes settled."));
	GetWorldTimerManager().SetTimer(DungeonLevel5SurfaceTrialVisualSmokeQuitTimerHandle, this, &ABroodPlayerController::FinishDungeonLevel5SurfaceTrialVisualSmokeTest, 1.25f, false);
}

void ABroodPlayerController::FinishDungeonLevel5SurfaceTrialVisualSmokeTest()
{
	BroodQuit();
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

void ABroodPlayerController::RunDungeonLevel5SmokeStep()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr;
	if (!BroodCharacter || !BroodGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_SMOKE_FAILED: pawn or game mode missing."));
		GetWorldTimerManager().ClearTimer(DungeonLevel5SmokeTimerHandle);
		BroodQuit();
		return;
	}

	int32 LivingEnemies = 0;
	for (TActorIterator<ABroodEnemyCharacter> EnemyIterator(GetWorld()); EnemyIterator; ++EnemyIterator)
	{
		ABroodEnemyCharacter* Enemy = *EnemyIterator;
		if (!Enemy || Enemy->IsDead())
		{
			continue;
		}

		LivingEnemies++;
		const bool bLikelyBoss = Enemy->GetBiomassReward() >= 50.0f;
		if (bLikelyBoss && !bDungeonLevel5SmokeBossSeen)
		{
			bDungeonLevel5SmokeBossSeen = true;
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_BOSS_SEEN: Jailer Husk spawned."));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_SEEN: Jailer Husk guards the descent to Dungeon Level 4."));
		}

		Enemy->SetActorLocation(BroodCharacter->GetActorLocation() + FVector(180.0f + LivingEnemies * 24.0f, 0.0f, 0.0f), false, nullptr, ETeleportType::TeleportPhysics);
		Enemy->SetActorHiddenInGame(true);
		Enemy->SetActorEnableCollision(false);
		UGameplayStatics::ApplyDamage(Enemy, 10000.0f, this, BroodCharacter, UDamageType::StaticClass());
	}

	for (TActorIterator<ABiomassPickup> PickupIterator(GetWorld()); PickupIterator; ++PickupIterator)
	{
		PickupIterator->SetActorLocation(BroodCharacter->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (LivingEnemies > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_WAVE_CLEARED: cleared %d enemies at smoke step %d."), LivingEnemies, DungeonLevel5SmokeStep);
	}
	else if (!bDungeonLevel5SmokeBossSeen && DungeonLevel5SmokeEvolutionSelections < 3)
	{
		BroodGameMode->ChooseEvolution(0);
		DungeonLevel5SmokeEvolutionSelections++;
		UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EVOLUTION_SELECTED: choice 1 selected, total %d."), DungeonLevel5SmokeEvolutionSelections);
	}
	else if (bDungeonLevel5SmokeBossSeen)
	{
		GetWorldTimerManager().ClearTimer(DungeonLevel5SmokeTimerHandle);
		if (DungeonLevel5SmokeEvolutionSelections >= 3)
		{
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SMOKE_PASSED: selected %d evolutions and defeated Jailer Husk."), DungeonLevel5SmokeEvolutionSelections);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_SMOKE_FAILED: boss completed after only %d evolutions."), DungeonLevel5SmokeEvolutionSelections);
		}
		BroodQuit();
		return;
	}

	DungeonLevel5SmokeStep++;
	if (DungeonLevel5SmokeStep >= 32)
	{
		GetWorldTimerManager().ClearTimer(DungeonLevel5SmokeTimerHandle);
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_SMOKE_FAILED: timed out after %d steps, evolutions=%d, bossSeen=%s."),
			DungeonLevel5SmokeStep,
			DungeonLevel5SmokeEvolutionSelections,
			bDungeonLevel5SmokeBossSeen ? TEXT("true") : TEXT("false"));
		BroodQuit();
	}
}

void ABroodPlayerController::RunDungeonLevel5CompletionVisualSmokeStep()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr;
	if (!BroodCharacter || !BroodGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_VISUAL_SMOKE_FAILED: pawn or game mode missing."));
		GetWorldTimerManager().ClearTimer(DungeonLevel5CompletionVisualSmokeTimerHandle);
		BroodQuit();
		return;
	}

	if (BroodGameMode->IsDungeonLevel5Complete())
	{
		const FVector ExitLocation(1120.0f, -1210.0f, 120.0f);
		FVector ReviewFocus = ExitLocation;
		BroodCharacter->SetActorLocation(bDungeonLevel5ExitShowcaseSmokeActive ? FVector(780.0f, -965.0f, 120.0f) : FVector(860.0f, -1010.0f, 120.0f), false, nullptr, ETeleportType::TeleportPhysics);
		BroodCharacter->AimAtWorldLocation(ExitLocation);
		if (DungeonLevel5CompletionVisualReadyFrames == 0)
		{
			TArray<UTextRenderComponent*> TextComponents;
			BroodCharacter->GetComponents<UTextRenderComponent>(TextComponents);
			for (UTextRenderComponent* TextComponent : TextComponents)
			{
				if (TextComponent)
				{
					TextComponent->SetHiddenInGame(true);
					TextComponent->SetVisibility(false);
				}
			}

			if (bDungeonLevel5ExitShowcaseSmokeActive)
			{
				UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
				UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
				UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
				UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"));
				UStaticMesh* WallMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"));
				UStaticMesh* CeilingMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat"));
				UStaticMesh* EdgeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"));
				UStaticMesh* DoorwayMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay"));
				UStaticMesh* HalfArchMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Half_Arch.SM_Half_Arch"));
				UStaticMesh* PillarMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"));
				UStaticMesh* StairsMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_5.SM_Stairs_5"));
				UStaticMesh* KeyMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Key.SM_Lock_Key"));
				UStaticMesh* LockMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Open.SM_Lock_Open"));
				UStaticMesh* ChainMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"));
				UStaticMesh* TorchMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch"));

				auto TintMesh = [](UStaticMeshComponent* MeshComponent, const FLinearColor& Color)
				{
					if (!MeshComponent)
					{
						return;
					}

					UMaterialInstanceDynamic* Material = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
					if (Material)
					{
						Material->SetVectorParameterValue(TEXT("Color"), Color);
						Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
					}
				};
				auto SpawnShowcaseMesh = [this, TintMesh](const FString& Name, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FRotator& Rotation, const FLinearColor& Color, bool bTint)
				{
					if (!Mesh)
					{
						return;
					}

					AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(Location, Rotation);
					if (!MeshActor)
					{
						return;
					}

					MeshActor->Rename(*Name);
					MeshActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
					MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
					MeshActor->SetActorScale3D(Scale);
					MeshActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					if (bTint)
					{
						TintMesh(MeshActor->GetStaticMeshComponent(), Color);
					}
				};

				const FVector ShowcaseBase(3200.0f, -2600.0f, 820.0f);
				ReviewFocus = ShowcaseBase + FVector(65.0f, -105.0f, 105.0f);
				const FLinearColor GlowColor(0.00f, 0.78f, 0.34f);
				const FLinearColor KeyTint(0.62f, 0.52f, 0.26f);

				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_0"), FloorMesh, ShowcaseBase + FVector(-260.0f, -210.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_1"), FloorMesh, ShowcaseBase + FVector(260.0f, -210.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_2"), FloorMesh, ShowcaseBase + FVector(-260.0f, 260.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_3"), FloorMesh, ShowcaseBase + FVector(260.0f, 260.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 270.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_4"), FloorMesh, ShowcaseBase + FVector(-780.0f, -210.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_5"), FloorMesh, ShowcaseBase + FVector(780.0f, -210.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_6"), FloorMesh, ShowcaseBase + FVector(-780.0f, 260.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 270.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_7"), FloorMesh, ShowcaseBase + FVector(780.0f, 260.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_8"), FloorMesh, ShowcaseBase + FVector(0.0f, -680.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_9"), FloorMesh, ShowcaseBase + FVector(-520.0f, -680.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 270.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_10"), FloorMesh, ShowcaseBase + FVector(520.0f, -680.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_11"), FloorMesh, ShowcaseBase + FVector(0.0f, 730.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_12"), FloorMesh, ShowcaseBase + FVector(-520.0f, 730.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_13"), FloorMesh, ShowcaseBase + FVector(520.0f, 730.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_14"), FloorMesh, ShowcaseBase + FVector(-520.0f, 1200.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 270.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_15"), FloorMesh, ShowcaseBase + FVector(0.0f, 1200.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_16"), FloorMesh, ShowcaseBase + FVector(520.0f, 1200.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_17"), FloorMesh, ShowcaseBase + FVector(-520.0f, 495.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_18"), FloorMesh, ShowcaseBase + FVector(0.0f, 495.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_19"), FloorMesh, ShowcaseBase + FVector(520.0f, 495.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 270.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_20"), FloorMesh, ShowcaseBase + FVector(1040.0f, 730.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_21"), FloorMesh, ShowcaseBase + FVector(1040.0f, 1200.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_22"), FloorMesh, ShowcaseBase + FVector(-1040.0f, 730.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 270.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFloor_23"), FloorMesh, ShowcaseBase + FVector(-1040.0f, 1200.0f, -16.0f), FVector(1.18f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseUpperLanding"), FloorMesh, ShowcaseBase + FVector(80.0f, 490.0f, 106.0f), FVector(0.82f), FRotator(0.0f, 138.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseDarkBackplate"), CubeMesh, ShowcaseBase + FVector(0.0f, 1420.0f, 210.0f), FVector(18.0f, 0.24f, 3.6f), FRotator::ZeroRotator, FLinearColor(0.012f, 0.014f, 0.017f), true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseLeftBackplate"), CubeMesh, ShowcaseBase + FVector(-1240.0f, 260.0f, 210.0f), FVector(0.22f, 13.0f, 3.6f), FRotator::ZeroRotator, FLinearColor(0.012f, 0.014f, 0.017f), true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseRightBackplate"), CubeMesh, ShowcaseBase + FVector(1240.0f, 260.0f, 210.0f), FVector(0.22f, 13.0f, 3.6f), FRotator::ZeroRotator, FLinearColor(0.012f, 0.014f, 0.017f), true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseRearVoidSeal"), CubeMesh, ShowcaseBase + FVector(0.0f, 1640.0f, 265.0f), FVector(24.0f, 0.30f, 5.2f), FRotator::ZeroRotator, FLinearColor(0.006f, 0.007f, 0.009f), true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseLeftVoidSeal"), CubeMesh, ShowcaseBase + FVector(-1500.0f, 260.0f, 265.0f), FVector(0.30f, 18.0f, 5.2f), FRotator::ZeroRotator, FLinearColor(0.006f, 0.007f, 0.009f), true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseRightVoidSeal"), CubeMesh, ShowcaseBase + FVector(1500.0f, 260.0f, 265.0f), FVector(0.30f, 18.0f, 5.2f), FRotator::ZeroRotator, FLinearColor(0.006f, 0.007f, 0.009f), true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseHighVoidSeal"), CubeMesh, ShowcaseBase + FVector(0.0f, 380.0f, 590.0f), FVector(24.0f, 18.0f, 0.30f), FRotator::ZeroRotator, FLinearColor(0.006f, 0.007f, 0.009f), true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseBackWall_0"), WallMesh, ShowcaseBase + FVector(-720.0f, 1110.0f, 118.0f), FVector(0.88f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseBackWall_1"), WallMesh, ShowcaseBase + FVector(-200.0f, 1110.0f, 118.0f), FVector(0.88f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseBackWall_2"), WallMesh, ShowcaseBase + FVector(320.0f, 1110.0f, 118.0f), FVector(0.88f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseBackWall_3"), WallMesh, ShowcaseBase + FVector(840.0f, 1110.0f, 118.0f), FVector(0.88f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseBackWall_4"), WallMesh, ShowcaseBase + FVector(1360.0f, 1110.0f, 118.0f), FVector(0.88f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseBackWall_5"), WallMesh, ShowcaseBase + FVector(-1240.0f, 1110.0f, 118.0f), FVector(0.88f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseLeftWall_0"), WallMesh, ShowcaseBase + FVector(-1060.0f, -380.0f, 118.0f), FVector(0.88f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseLeftWall_1"), WallMesh, ShowcaseBase + FVector(-1060.0f, 160.0f, 118.0f), FVector(0.88f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseRightWall_0"), WallMesh, ShowcaseBase + FVector(1060.0f, -380.0f, 118.0f), FVector(0.88f), FRotator(0.0f, -90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseRightWall_1"), WallMesh, ShowcaseBase + FVector(1060.0f, 160.0f, 118.0f), FVector(0.88f), FRotator(0.0f, -90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseCeiling_0"), CeilingMesh, ShowcaseBase + FVector(-430.0f, 160.0f, 375.0f), FVector(0.78f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseCeiling_1"), CeilingMesh, ShowcaseBase + FVector(430.0f, 160.0f, 375.0f), FVector(0.78f), FRotator(0.0f, 90.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseCeiling_2"), CeilingMesh, ShowcaseBase + FVector(-430.0f, 760.0f, 375.0f), FVector(0.78f), FRotator(0.0f, 180.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseCeiling_3"), CeilingMesh, ShowcaseBase + FVector(430.0f, 760.0f, 375.0f), FVector(0.78f), FRotator(0.0f, 270.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFrontEdge_0"), EdgeMesh, ShowcaseBase + FVector(-520.0f, -860.0f, 18.0f), FVector(0.88f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFrontEdge_1"), EdgeMesh, ShowcaseBase + FVector(0.0f, -860.0f, 18.0f), FVector(0.88f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseFrontEdge_2"), EdgeMesh, ShowcaseBase + FVector(520.0f, -860.0f, 18.0f), FVector(0.88f), FRotator(0.0f, 0.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseStairs"), StairsMesh, ShowcaseBase + FVector(120.0f, -40.0f, 10.0f), FVector(0.95f), FRotator(0.0f, 138.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseDoorway"), DoorwayMesh, ShowcaseBase + FVector(250.0f, -250.0f, 105.0f), FVector(0.52f), FRotator(0.0f, 138.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseHalfArchLeft"), HalfArchMesh, ShowcaseBase + FVector(-180.0f, 80.0f, 90.0f), FVector(0.48f), FRotator(0.0f, 138.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseHalfArchRight"), HalfArchMesh, ShowcaseBase + FVector(275.0f, -365.0f, 90.0f), FVector(0.48f), FRotator(0.0f, -42.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcasePillarLeft"), PillarMesh, ShowcaseBase + FVector(-150.0f, -75.0f, 72.0f), FVector(0.55f), FRotator(0.0f, 138.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcasePillarRight"), PillarMesh, ShowcaseBase + FVector(205.0f, 82.0f, 72.0f), FVector(0.55f), FRotator(0.0f, 138.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseKeyRuneA"), KeyMesh ? KeyMesh : CubeMesh, ShowcaseBase + FVector(25.0f, -95.0f, 62.0f), KeyMesh ? FVector(0.58f) : FVector(0.68f, 0.045f, 0.04f), FRotator(0.0f, 60.0f, 88.0f), KeyTint, true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseKeyRuneB"), KeyMesh ? KeyMesh : CubeMesh, ShowcaseBase + FVector(100.0f, -55.0f, 64.0f), KeyMesh ? FVector(0.46f) : FVector(0.48f, 0.045f, 0.04f), FRotator(0.0f, -18.0f, 82.0f), KeyTint, true);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseLock"), LockMesh, ShowcaseBase + FVector(-135.0f, -155.0f, 62.0f), FVector(0.62f), FRotator(0.0f, 20.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseChain"), ChainMesh, ShowcaseBase + FVector(-60.0f, -245.0f, 82.0f), FVector(0.52f), FRotator(0.0f, 28.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseTorchLeft"), TorchMesh, ShowcaseBase + FVector(-240.0f, -150.0f, 108.0f), FVector(0.68f), FRotator(0.0f, 138.0f, 0.0f), FLinearColor::White, false);
				SpawnShowcaseMesh(TEXT("DungeonLevel5ExitShowcaseTorchRight"), TorchMesh, ShowcaseBase + FVector(245.0f, 140.0f, 108.0f), FVector(0.68f), FRotator(0.0f, -42.0f, 0.0f), FLinearColor::White, false);
				for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
				{
					AActor* Actor = *ActorIterator;
					if (!Actor || Actor == BroodCharacter || Actor->GetName().Contains(TEXT("DungeonLevel5ExitShowcase")))
					{
						continue;
					}

					Actor->SetActorHiddenInGame(true);
					Actor->SetActorEnableCollision(false);
					TArray<ULightComponent*> LightComponents;
					Actor->GetComponents<ULightComponent>(LightComponents);
					for (ULightComponent* LightComponent : LightComponents)
					{
						if (LightComponent)
						{
							LightComponent->SetVisibility(false);
						}
					}
				}
				if (APointLight* ShowcaseKeyLight = GetWorld()->SpawnActor<APointLight>(ShowcaseBase + FVector(-260.0f, -360.0f, 245.0f), FRotator::ZeroRotator))
				{
					ShowcaseKeyLight->Rename(TEXT("DungeonLevel5ExitShowcaseKeyLight"));
					ShowcaseKeyLight->SetMobility(EComponentMobility::Movable);
					ShowcaseKeyLight->SetLightColor(FLinearColor(1.0f, 0.58f, 0.24f));
					ShowcaseKeyLight->SetBrightness(4200.0f);
					if (UPointLightComponent* PointLightComponent = ShowcaseKeyLight->FindComponentByClass<UPointLightComponent>())
					{
						PointLightComponent->SetAttenuationRadius(880.0f);
					}
				}
				if (APointLight* ShowcaseFillLight = GetWorld()->SpawnActor<APointLight>(ShowcaseBase + FVector(360.0f, 280.0f, 300.0f), FRotator::ZeroRotator))
				{
					ShowcaseFillLight->Rename(TEXT("DungeonLevel5ExitShowcaseColdFillLight"));
					ShowcaseFillLight->SetMobility(EComponentMobility::Movable);
					ShowcaseFillLight->SetLightColor(FLinearColor(0.10f, 0.30f, 0.42f));
					ShowcaseFillLight->SetBrightness(2100.0f);
					if (UPointLightComponent* PointLightComponent = ShowcaseFillLight->FindComponentByClass<UPointLightComponent>())
					{
						PointLightComponent->SetAttenuationRadius(1050.0f);
					}
				}
				UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_STAGED_READY: isolated lower-exit architecture showcase spawned with pack assets for angled review."));
				UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_ASSET_FLOOR_READY: angled lower-exit showcase uses MedievalDungeon SM_Floor tiles instead of debug platform geometry."));
				UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_CLOSED_FRAME_READY: angled lower-exit showcase uses pack walls and edge stones to hide technical review floor edges."));
				UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_DUNGEON_CLOSED_READY: lower-exit showcase now has side walls, dark backplates, ceiling pieces and pack key assets instead of visible void/primitive portal forms."));
				UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_ISOLATED_STAGE_READY: showcase hides non-showcase actors and scene lights so the screenshot reads as a closed dungeon set."));
			}

			const FVector CompletionCameraLocation = ExitLocation + FVector(-210.0f, 250.0f, 330.0f);
			const FVector CompletionLookAtLocation = ExitLocation + FVector(25.0f, -70.0f, 62.0f);
			const FVector ExitShowcaseCameraLocation = ReviewFocus + FVector(-520.0f, -470.0f, 285.0f);
			const FVector ExitShowcaseLookAtLocation = ReviewFocus + FVector(45.0f, -40.0f, 18.0f);
			ACameraActor* ReviewCamera = GetWorld()->SpawnActor<ACameraActor>(
				bDungeonLevel5ExitShowcaseSmokeActive ? ExitShowcaseCameraLocation : CompletionCameraLocation,
				bDungeonLevel5ExitShowcaseSmokeActive ? (ExitShowcaseLookAtLocation - ExitShowcaseCameraLocation).Rotation() : (CompletionLookAtLocation - CompletionCameraLocation).Rotation());
			if (ReviewCamera)
			{
				if (bDungeonLevel5ExitShowcaseSmokeActive)
				{
					ReviewCamera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Perspective);
					ReviewCamera->GetCameraComponent()->SetFieldOfView(46.0f);
				}
				else
				{
					for (TActorIterator<AStaticMeshActor> MeshIterator(GetWorld()); MeshIterator; ++MeshIterator)
					{
						AStaticMeshActor* MeshActor = *MeshIterator;
						if (!MeshActor)
						{
							continue;
						}

						const FString ActorName = MeshActor->GetName();
						if (ActorName.Contains(TEXT("DungeonLevel5ExitRoomCeiling")) ||
							ActorName.Contains(TEXT("DungeonLevel5JailerRoomCeiling")) ||
							ActorName.Contains(TEXT("DungeonLevel5JailerRoomBackWall")) ||
							ActorName.Contains(TEXT("DungeonLevel5JailerRoomLeftWall")) ||
							ActorName.Contains(TEXT("DungeonLevel5JailerRoomRightWall")) ||
							ActorName.Contains(TEXT("DungeonLevel5JailerRoomDarkRear")) ||
							ActorName.Contains(TEXT("DungeonLevel5ExitRoomDarkBack")) ||
							ActorName.Contains(TEXT("DungeonLevel5ExitRoomBackWall")) ||
							ActorName.Contains(TEXT("DungeonLevel5ExitRoomLeftWall")) ||
							ActorName.Contains(TEXT("DungeonLevel5ExitRoomRightWall")) ||
							ActorName.Contains(TEXT("NorthDungeonWall")) ||
							ActorName.Contains(TEXT("SouthDungeonWall")) ||
							ActorName.Contains(TEXT("EastDungeonWall")) ||
							ActorName.Contains(TEXT("WestDungeonWall")) ||
							ActorName.Contains(TEXT("DungeonWallDecor")) ||
							ActorName.Contains(TEXT("DungeonLevel5ExitArchDoorway")) ||
							ActorName.Contains(TEXT("DungeonLevel5ExitRoomCryptDoorway")) ||
							ActorName.Contains(TEXT("LowerDungeonDoorway")))
						{
							MeshActor->SetActorHiddenInGame(true);
							MeshActor->SetActorEnableCollision(false);
						}
					}
					for (TActorIterator<ATextRenderActor> TextIterator(GetWorld()); TextIterator; ++TextIterator)
					{
						if (ATextRenderActor* TextActor = *TextIterator)
						{
							TextActor->SetActorHiddenInGame(true);
						}
					}
					ReviewCamera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Perspective);
					ReviewCamera->GetCameraComponent()->SetFieldOfView(58.0f);
					FHitResult CompletionSightlineHit;
					FCollisionQueryParams CompletionSightlineParams(SCENE_QUERY_STAT(DungeonLevel5CompletionSightline), true);
					CompletionSightlineParams.AddIgnoredActor(ReviewCamera);
					if (GetWorld()->LineTraceSingleByChannel(CompletionSightlineHit, CompletionCameraLocation, CompletionLookAtLocation, ECC_Visibility, CompletionSightlineParams) && CompletionSightlineHit.GetActor())
					{
						UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_SIGHTLINE_HIT: angled completion camera first visibility hit is %s at distance %.1f."),
							*CompletionSightlineHit.GetActor()->GetName(),
							CompletionSightlineHit.Distance);
					}
					UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_ANGLED_OCCLUDERS_HIDDEN_READY: completion angled review hides in-level ceiling and dark-back staging pieces that would cover the descent."));
				}
				SetViewTarget(ReviewCamera);
				if (bDungeonLevel5ExitShowcaseSmokeActive)
				{
					UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_CAMERA_READY: lower-exit showcase uses angled review camera and hides foreground occluders."));
				}
				else
				{
					UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_REVIEW_CAMERA_READY: completion visual smoke uses an angled in-level dungeon-exit review camera and hides pawn text clutter."));
				}
			}
		}
		DungeonLevel5CompletionVisualReadyFrames++;
		if (DungeonLevel5CompletionVisualReadyFrames < 8)
		{
			return;
		}

		if (!bDungeonLevel5CompletionVisualScreenshotRequested)
		{
			if (GEngine)
			{
				GEngine->ClearOnScreenDebugMessages();
			}
			bDungeonLevel5CompletionVisualScreenshotRequested = true;
			if (bDungeonLevel5ExitShowcaseSmokeActive)
			{
				RequestViewportScreenshot(TEXT("WindowedLevel5ExitShowcaseViewport.png"), TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_SCREENSHOT_REQUESTED"));
				UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_SMOKE_PASSED: angled lower-exit showcase screenshot requested."));
			}
			else
			{
				RequestViewportScreenshot(TEXT("WindowedLevel5CompleteViewport.png"), TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_SCREENSHOT_REQUESTED"));
				UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_VISUAL_SMOKE_PASSED: completion screenshot requested after unlocked descent."));
			}
			GetWorldTimerManager().ClearTimer(DungeonLevel5CompletionVisualSmokeTimerHandle);
			GetWorldTimerManager().SetTimer(DungeonLevel5CompletionVisualSmokeQuitTimerHandle, this, &ABroodPlayerController::BroodQuit, 1.25f, false);
		}
		return;
	}

	int32 LivingEnemies = 0;
	for (TActorIterator<ABroodEnemyCharacter> EnemyIterator(GetWorld()); EnemyIterator; ++EnemyIterator)
	{
		ABroodEnemyCharacter* Enemy = *EnemyIterator;
		if (!Enemy || Enemy->IsDead())
		{
			continue;
		}

		LivingEnemies++;
		Enemy->SetActorLocation(BroodCharacter->GetActorLocation() + FVector(180.0f + LivingEnemies * 24.0f, 0.0f, 0.0f), false, nullptr, ETeleportType::TeleportPhysics);
		UGameplayStatics::ApplyDamage(Enemy, 10000.0f, this, BroodCharacter, UDamageType::StaticClass());
	}

	for (TActorIterator<ABiomassPickup> PickupIterator(GetWorld()); PickupIterator; ++PickupIterator)
	{
		PickupIterator->SetActorLocation(BroodCharacter->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (LivingEnemies == 0 && DungeonLevel5CompletionVisualSmokeEvolutionSelections < 3)
	{
		BroodGameMode->ChooseEvolution(0);
		DungeonLevel5CompletionVisualSmokeEvolutionSelections++;
	}

	DungeonLevel5CompletionVisualSmokeStep++;
	if (DungeonLevel5CompletionVisualSmokeStep >= 36)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETION_VISUAL_SMOKE_FAILED: timed out after %d steps."), DungeonLevel5CompletionVisualSmokeStep);
		GetWorldTimerManager().ClearTimer(DungeonLevel5CompletionVisualSmokeTimerHandle);
		BroodQuit();
	}
}

void ABroodPlayerController::RunDungeonLevel5JailerVisualSmokeStep()
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(GetPawn());
	ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr;
	if (!BroodCharacter || !BroodGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VISUAL_SMOKE_FAILED: pawn or game mode missing."));
		GetWorldTimerManager().ClearTimer(DungeonLevel5JailerVisualSmokeTimerHandle);
		BroodQuit();
		return;
	}

	ABroodEnemyCharacter* JailerHusk = nullptr;
	int32 LivingEnemies = 0;
	for (TActorIterator<ABroodEnemyCharacter> EnemyIterator(GetWorld()); EnemyIterator; ++EnemyIterator)
	{
		ABroodEnemyCharacter* Enemy = *EnemyIterator;
		if (!Enemy || Enemy->IsDead())
		{
			continue;
		}

		LivingEnemies++;
		if (Enemy->GetBiomassReward() >= 50.0f)
		{
			JailerHusk = Enemy;
			continue;
		}

		Enemy->SetActorLocation(BroodCharacter->GetActorLocation() + FVector(180.0f + LivingEnemies * 24.0f, 0.0f, 0.0f), false, nullptr, ETeleportType::TeleportPhysics);
		UGameplayStatics::ApplyDamage(Enemy, 10000.0f, this, BroodCharacter, UDamageType::StaticClass());
	}

	for (TActorIterator<ABiomassPickup> PickupIterator(GetWorld()); PickupIterator; ++PickupIterator)
	{
		PickupIterator->SetActorLocation(BroodCharacter->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (JailerHusk)
	{
		const FVector BossLocation(1120.0f, -1270.0f, 130.0f);
		const FVector PlayerLocation = BossLocation + FVector(760.0f, 760.0f, 0.0f);
		JailerHusk->SetActorLocation(BossLocation, false, nullptr, ETeleportType::TeleportPhysics);
		JailerHusk->SetActorRotation(FRotator(0.0f, -138.0f, 0.0f));
		JailerHusk->SetActorScale3D(FVector(1.45f));
		if (UCharacterMovementComponent* JailerMovement = JailerHusk->GetCharacterMovement())
		{
			JailerMovement->StopMovementImmediately();
			JailerMovement->DisableMovement();
		}
		TArray<UTextRenderComponent*> TextComponents;
		JailerHusk->GetComponents<UTextRenderComponent>(TextComponents);
		for (UTextRenderComponent* TextComponent : TextComponents)
		{
			if (TextComponent)
			{
				TextComponent->SetVisibility(false);
				TextComponent->SetHiddenInGame(true);
			}
		}
		BroodCharacter->SetActorLocation(PlayerLocation, false, nullptr, ETeleportType::TeleportPhysics);
		BroodCharacter->AimAtWorldLocation(BossLocation);
		TArray<UTextRenderComponent*> PlayerTextComponents;
		BroodCharacter->GetComponents<UTextRenderComponent>(PlayerTextComponents);
		for (UTextRenderComponent* TextComponent : PlayerTextComponents)
		{
			if (TextComponent)
			{
				TextComponent->SetVisibility(false);
				TextComponent->SetHiddenInGame(true);
			}
		}

		if (DungeonLevel5JailerVisualReadyFrames == 0)
		{
			UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"));
			UStaticMesh* WallMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"));
			UStaticMesh* CeilingMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat"));
			UStaticMesh* EdgeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"));
			UStaticMesh* PillarMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"));
			UStaticMesh* CellFrontMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front"));
			UStaticMesh* CellDoorMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door"));
			UStaticMesh* ChainMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"));
			UStaticMesh* TorchMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch"));
			UStaticMesh* CoffinMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Stone_Coffin.SM_Stone_Coffin"));
			UStaticMesh* StocksMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Stocks_Game_Ready.SM_Stocks_Game_Ready"));
			UStaticMesh* GibbetMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Wall_Mount.SM_Gibbet_Cage_Wall_Mount"));
			UStaticMesh* SkullMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull"));
			UStaticMesh* RibMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"));
			UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));

			auto TintMesh = [](UStaticMeshComponent* MeshComponent, const FLinearColor& Color)
			{
				if (!MeshComponent)
				{
					return;
				}

				UMaterialInstanceDynamic* Material = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
				if (Material)
				{
					Material->SetVectorParameterValue(TEXT("Color"), Color);
					Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
				}
			};
			auto SpawnReviewMesh = [this, TintMesh](const FString& Name, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FLinearColor& Color, const FRotator& Rotation = FRotator::ZeroRotator)
			{
				if (!Mesh || !GetWorld())
				{
					return;
				}

				AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(Location, Rotation);
				if (!MeshActor || !MeshActor->GetStaticMeshComponent())
				{
					return;
				}

				MeshActor->Rename(*Name);
				MeshActor->SetMobility(EComponentMobility::Movable);
				MeshActor->SetActorScale3D(Scale);
				MeshActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
				MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
				MeshActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				if (Color != FLinearColor::Transparent)
				{
					TintMesh(MeshActor->GetStaticMeshComponent(), Color);
				}
			};

			const FLinearColor KeepMaterialColor = FLinearColor::Transparent;
			const FLinearColor OldBoneColor(0.62f, 0.56f, 0.44f);
			const FLinearColor DeadStoneColor(0.12f, 0.13f, 0.14f);
			const FLinearColor RustColor(0.34f, 0.24f, 0.18f);
			const FVector StageOrigin = BossLocation + FVector(0.0f, 0.0f, -106.0f);
			for (int32 X = -4; X <= 4; ++X)
			{
				for (int32 Y = -4; Y <= 4; ++Y)
				{
					SpawnReviewMesh(
						FString::Printf(TEXT("DungeonLevel5JailerReviewFloor_%d_%d"), X, Y),
						FloorMesh ? FloorMesh : CubeMesh,
						StageOrigin + FVector(X * 260.0f, Y * 260.0f, 0.0f),
						FloorMesh ? FVector(0.52f) : FVector(2.6f, 2.6f, 0.04f),
						FloorMesh ? KeepMaterialColor : DeadStoneColor,
						FRotator::ZeroRotator);
				}
			}

			for (int32 Index = -5; Index <= 5; ++Index)
			{
				SpawnReviewMesh(FString::Printf(TEXT("DungeonLevel5JailerReviewBackWall_%d"), Index), WallMesh ? WallMesh : CubeMesh, StageOrigin + FVector(Index * 150.0f, -610.0f, 88.0f), WallMesh ? FVector(0.58f) : FVector(2.6f, 0.12f, 1.6f), WallMesh ? KeepMaterialColor : DeadStoneColor, FRotator(0.0f, 0.0f, 0.0f));
				SpawnReviewMesh(FString::Printf(TEXT("DungeonLevel5JailerReviewLeftWall_%d"), Index), WallMesh ? WallMesh : CubeMesh, StageOrigin + FVector(-780.0f, Index * 150.0f, 88.0f), WallMesh ? FVector(0.58f) : FVector(2.6f, 0.12f, 1.6f), WallMesh ? KeepMaterialColor : DeadStoneColor, FRotator(0.0f, 90.0f, 0.0f));
				SpawnReviewMesh(FString::Printf(TEXT("DungeonLevel5JailerReviewRightWall_%d"), Index), WallMesh ? WallMesh : CubeMesh, StageOrigin + FVector(780.0f, Index * 150.0f, 88.0f), WallMesh ? FVector(0.58f) : FVector(2.6f, 0.12f, 1.6f), WallMesh ? KeepMaterialColor : DeadStoneColor, FRotator(0.0f, 90.0f, 0.0f));
			}
			for (int32 X = -1; X <= 1; ++X)
			{
				for (int32 Y = -1; Y <= 1; ++Y)
				{
					SpawnReviewMesh(
						FString::Printf(TEXT("DungeonLevel5JailerReviewCeiling_%d_%d"), X, Y),
						CeilingMesh ? CeilingMesh : CubeMesh,
						StageOrigin + FVector(X * 430.0f, Y * 430.0f - 120.0f, 355.0f),
						CeilingMesh ? FVector(0.74f) : FVector(4.0f, 4.0f, 0.08f),
						CeilingMesh ? KeepMaterialColor : DeadStoneColor,
						FRotator(0.0f, (X + Y) * 90.0f, 0.0f));
				}
			}

			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewFrontEdge"), EdgeMesh ? EdgeMesh : CubeMesh, StageOrigin + FVector(0.0f, 590.0f, 18.0f), EdgeMesh ? FVector(0.62f) : FVector(5.2f, 0.12f, 0.08f), EdgeMesh ? KeepMaterialColor : DeadStoneColor, FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewPillarLeft"), PillarMesh ? PillarMesh : CubeMesh, StageOrigin + FVector(-380.0f, -360.0f, 120.0f), PillarMesh ? FVector(0.58f) : FVector(0.36f, 0.36f, 1.9f), PillarMesh ? KeepMaterialColor : DeadStoneColor, FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewPillarRight"), PillarMesh ? PillarMesh : CubeMesh, StageOrigin + FVector(380.0f, -360.0f, 120.0f), PillarMesh ? FVector(0.58f) : FVector(0.36f, 0.36f, 1.9f), PillarMesh ? KeepMaterialColor : DeadStoneColor, FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewRearCellBars"), CellFrontMesh ? CellFrontMesh : CubeMesh, StageOrigin + FVector(0.0f, -545.0f, 118.0f), CellFrontMesh ? FVector(0.95f) : FVector(4.8f, 0.10f, 1.8f), CellFrontMesh ? KeepMaterialColor : RustColor, FRotator(0.0f, 0.0f, 0.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewBrokenCellDoor"), CellDoorMesh ? CellDoorMesh : CubeMesh, StageOrigin + FVector(-255.0f, -350.0f, 82.0f), CellDoorMesh ? FVector(0.82f) : FVector(0.12f, 1.1f, 1.2f), CellDoorMesh ? KeepMaterialColor : RustColor, FRotator(0.0f, 24.0f, -8.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewStocks"), StocksMesh ? StocksMesh : CubeMesh, StageOrigin + FVector(-330.0f, -135.0f, 42.0f), StocksMesh ? FVector(0.64f) : FVector(1.0f, 0.38f, 0.34f), StocksMesh ? KeepMaterialColor : RustColor, FRotator(0.0f, 22.0f, 0.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewWallGibbet"), GibbetMesh ? GibbetMesh : CubeMesh, StageOrigin + FVector(365.0f, -502.0f, 170.0f), GibbetMesh ? FVector(0.70f) : FVector(0.42f, 0.16f, 0.70f), GibbetMesh ? KeepMaterialColor : RustColor, FRotator(0.0f, 0.0f, 0.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewFarBackdrop"), CubeMesh, StageOrigin + FVector(0.0f, -700.0f, 260.0f), FVector(22.0f, 0.22f, 5.0f), FLinearColor(0.010f, 0.012f, 0.014f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewLeftBackdrop"), CubeMesh, StageOrigin + FVector(-850.0f, -40.0f, 250.0f), FVector(0.22f, 16.0f, 4.8f), FLinearColor(0.010f, 0.012f, 0.014f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewRightBackdrop"), CubeMesh, StageOrigin + FVector(850.0f, -40.0f, 250.0f), FVector(0.22f, 16.0f, 4.8f), FLinearColor(0.010f, 0.012f, 0.014f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewHighBackplate"), CubeMesh, StageOrigin + FVector(0.0f, -620.0f, 360.0f), FVector(18.0f, 0.20f, 4.8f), FLinearColor(0.018f, 0.020f, 0.022f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewCeilingVoidSeal"), CubeMesh, StageOrigin + FVector(0.0f, -30.0f, 535.0f), FVector(22.0f, 16.0f, 0.28f), FLinearColor(0.010f, 0.012f, 0.014f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewMidBackplate"), CubeMesh, StageOrigin + FVector(0.0f, -525.0f, 130.0f), FVector(12.0f, 0.20f, 1.7f), FLinearColor(0.025f, 0.028f, 0.030f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewLeftSightlineSeal"), CubeMesh, StageOrigin + FVector(-820.0f, -50.0f, 245.0f), FVector(0.22f, 15.0f, 4.0f), FLinearColor(0.018f, 0.020f, 0.022f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewRightSightlineSeal"), CubeMesh, StageOrigin + FVector(820.0f, -50.0f, 245.0f), FVector(0.22f, 15.0f, 4.0f), FLinearColor(0.018f, 0.020f, 0.022f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewFloorVoidSealBack"), CubeMesh, StageOrigin + FVector(0.0f, -520.0f, -18.0f), FVector(12.0f, 0.20f, 0.34f), FLinearColor(0.035f, 0.038f, 0.042f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewFloorVoidSealLeft"), CubeMesh, StageOrigin + FVector(-720.0f, 0.0f, -18.0f), FVector(0.20f, 14.0f, 0.34f), FLinearColor(0.035f, 0.038f, 0.042f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewFloorVoidSealRight"), CubeMesh, StageOrigin + FVector(720.0f, 0.0f, -18.0f), FVector(0.20f, 14.0f, 0.34f), FLinearColor(0.035f, 0.038f, 0.042f), FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewBackWallBase"), CubeMesh, StageOrigin + FVector(0.0f, -590.0f, 58.0f), FVector(18.0f, 0.18f, 0.52f), DeadStoneColor, FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewLeftWallBase"), CubeMesh, StageOrigin + FVector(-800.0f, 0.0f, 58.0f), FVector(0.18f, 18.0f, 0.52f), DeadStoneColor, FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewRightWallBase"), CubeMesh, StageOrigin + FVector(800.0f, 0.0f, 58.0f), FVector(0.18f, 18.0f, 0.52f), DeadStoneColor, FRotator::ZeroRotator);
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewFloorChains"), ChainMesh ? ChainMesh : CubeMesh, BossLocation + FVector(62.0f, -95.0f, -54.0f), ChainMesh ? FVector(0.62f) : FVector(0.08f, 1.2f, 0.04f), ChainMesh ? KeepMaterialColor : RustColor, FRotator(0.0f, 28.0f, 0.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewHangingChains"), ChainMesh ? ChainMesh : CubeMesh, BossLocation + FVector(-190.0f, -260.0f, 92.0f), ChainMesh ? FVector(0.52f) : FVector(0.06f, 0.06f, 1.2f), ChainMesh ? KeepMaterialColor : RustColor, FRotator(12.0f, -22.0f, 86.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewTorchLeft"), TorchMesh ? TorchMesh : CubeMesh, BossLocation + FVector(-300.0f, -505.0f, 75.0f), TorchMesh ? FVector(0.62f) : FVector(0.08f, 0.08f, 0.62f), TorchMesh ? KeepMaterialColor : RustColor, FRotator(0.0f, 0.0f, 0.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewTorchRight"), TorchMesh ? TorchMesh : CubeMesh, BossLocation + FVector(300.0f, -505.0f, 75.0f), TorchMesh ? FVector(0.62f) : FVector(0.08f, 0.08f, 0.62f), TorchMesh ? KeepMaterialColor : RustColor, FRotator(0.0f, 0.0f, 0.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewCoffin"), CoffinMesh ? CoffinMesh : CubeMesh, BossLocation + FVector(-350.0f, 240.0f, -58.0f), CoffinMesh ? FVector(0.62f) : FVector(1.1f, 0.42f, 0.22f), CoffinMesh ? KeepMaterialColor : DeadStoneColor, FRotator(0.0f, 18.0f, 0.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewSkull"), SkullMesh ? SkullMesh : CubeMesh, BossLocation + FVector(195.0f, 185.0f, -55.0f), SkullMesh ? FVector(0.58f) : FVector(0.12f), SkullMesh ? KeepMaterialColor : OldBoneColor, FRotator(0.0f, -40.0f, 0.0f));
			SpawnReviewMesh(TEXT("DungeonLevel5JailerReviewRibCage"), RibMesh ? RibMesh : CubeMesh, BossLocation + FVector(255.0f, 100.0f, -56.0f), RibMesh ? FVector(0.62f) : FVector(0.34f, 0.22f, 0.12f), RibMesh ? KeepMaterialColor : OldBoneColor, FRotator(0.0f, 35.0f, 0.0f));

			if (APointLight* KeyLight = GetWorld()->SpawnActor<APointLight>(BossLocation + FVector(-170.0f, -290.0f, 150.0f), FRotator::ZeroRotator))
			{
				KeyLight->Rename(TEXT("DungeonLevel5JailerReviewKeyLight"));
				KeyLight->SetMobility(EComponentMobility::Movable);
				KeyLight->SetLightColor(FLinearColor(1.0f, 0.55f, 0.22f));
				KeyLight->SetBrightness(950.0f);
				if (UPointLightComponent* PointLightComponent = KeyLight->FindComponentByClass<UPointLightComponent>())
				{
					PointLightComponent->SetAttenuationRadius(520.0f);
				}
			}
			if (APointLight* ColdFillLight = GetWorld()->SpawnActor<APointLight>(BossLocation + FVector(210.0f, 210.0f, 150.0f), FRotator::ZeroRotator))
			{
				ColdFillLight->Rename(TEXT("DungeonLevel5JailerReviewColdFillLight"));
				ColdFillLight->SetMobility(EComponentMobility::Movable);
				ColdFillLight->SetLightColor(FLinearColor(0.14f, 0.32f, 0.46f));
				ColdFillLight->SetBrightness(320.0f);
				if (UPointLightComponent* PointLightComponent = ColdFillLight->FindComponentByClass<UPointLightComponent>())
				{
					PointLightComponent->SetAttenuationRadius(520.0f);
				}
			}

			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CHAMBER_READY: Jailer Husk visual smoke staged a closed dungeon boss chamber around the live boss."));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PACK_ASSETS_READY: Jailer Husk review chamber uses MedievalDungeon floor, walls, pillars, chains, torch, coffin and bone assets with code fallbacks."));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CEILING_CLOSED_READY: Jailer Husk review chamber seals side sightlines and adds ceiling assets so the boss reads inside a dungeon room instead of a void."));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PRISON_ROOM_READY: Jailer Husk review chamber anchors wall modules to the floor and adds cell bars, broken door, stocks, gibbet and dark backplates."));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_IN_LEVEL_ROOM_READY: Jailer Husk visual smoke reviews the boss inside the authored gameplay boss room instead of an off-map review stage."));
			for (int32 BackdropIndex = -1; BackdropIndex <= 1; ++BackdropIndex)
			{
				SpawnReviewMesh(
					FString::Printf(TEXT("DungeonLevel5JailerInLevelWallBackdrop_%d"), BackdropIndex),
					WallMesh ? WallMesh : CubeMesh,
					BossLocation + FVector(BackdropIndex * 430.0f, -270.0f, 72.0f),
					WallMesh ? FVector(0.82f) : FVector(4.0f, 0.16f, 1.6f),
					WallMesh ? KeepMaterialColor : FLinearColor(0.020f, 0.024f, 0.026f),
					FRotator(0.0f, 0.0f, 0.0f));
			}
			for (TActorIterator<AStaticMeshActor> MeshIterator(GetWorld()); MeshIterator; ++MeshIterator)
			{
				AStaticMeshActor* MeshActor = *MeshIterator;
				if (!MeshActor)
				{
					continue;
				}

				const FString ActorName = MeshActor->GetName();
				if (!ActorName.Contains(TEXT("DungeonLevel5JailerReview")) ||
					ActorName.Contains(TEXT("DungeonLevel5JailerApproachGate")) ||
					ActorName.Contains(TEXT("DungeonLevel5JailerApproachBrokenDoor")) ||
					ActorName.Contains(TEXT("DungeonLevel5Exit")) ||
					ActorName.Contains(TEXT("DungeonLevel5ExitRoom")) ||
					ActorName.Contains(TEXT("LowerDungeonStair")) ||
					ActorName.Contains(TEXT("LowerDungeonDoorway")))
				{
					MeshActor->SetActorHiddenInGame(true);
					MeshActor->SetActorEnableCollision(false);
				}
			}
			for (TActorIterator<ATextRenderActor> TextIterator(GetWorld()); TextIterator; ++TextIterator)
			{
				if (ATextRenderActor* TextActor = *TextIterator)
				{
					TextActor->SetActorHiddenInGame(true);
				}
			}
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_FOREGROUND_BARS_HIDDEN_READY: Jailer review hides foreground approach bars and lower-exit pieces that would block the authored boss room screenshot."));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_TEXT_AND_BACKDROP_READY: Jailer review hides level text and adds a dark in-level backdrop behind the boss."));

			const FVector CameraLocation = BossLocation + FVector(0.0f, 760.0f, 270.0f);
			const FVector LookAtLocation = BossLocation + FVector(0.0f, -60.0f, 115.0f);
			ACameraActor* ReviewCamera = GetWorld()->SpawnActor<ACameraActor>(
				CameraLocation,
				(LookAtLocation - CameraLocation).Rotation());
			if (ReviewCamera)
			{
				ReviewCamera->GetCameraComponent()->SetFieldOfView(48.0f);
				SetViewTarget(ReviewCamera);
			}
			ConsoleCommand(TEXT("DisableAllScreenMessages"));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_STABILIZED_READY: Jailer Husk visual smoke freezes boss movement and disables screen messages before screenshot."));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_GROUNDED_READY: Jailer Husk visual smoke places boss at gameplay floor height before screenshot."));
			BroodCharacter->SetActorHiddenInGame(true);
			BroodCharacter->SetActorEnableCollision(false);
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_DISTRACTIONS_HIDDEN_READY: non-boss enemies and player pawn hidden for Jailer Husk art review screenshot."));
		}

		DungeonLevel5JailerVisualReadyFrames++;
		if (DungeonLevel5JailerVisualReadyFrames < 8)
		{
			return;
		}

		if (!bDungeonLevel5JailerVisualScreenshotRequested)
		{
			bDungeonLevel5JailerVisualScreenshotRequested = true;
			RequestViewportScreenshot(TEXT("WindowedJailerHuskViewport.png"), TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_SCREENSHOT_REQUESTED"));
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VISUAL_SMOKE_PASSED: live Jailer Husk screenshot requested."));
			GetWorldTimerManager().ClearTimer(DungeonLevel5JailerVisualSmokeTimerHandle);
			GetWorldTimerManager().SetTimer(DungeonLevel5JailerVisualSmokeQuitTimerHandle, this, &ABroodPlayerController::BroodQuit, 1.25f, false);
		}
		return;
	}

	if (LivingEnemies == 0 && DungeonLevel5JailerVisualSmokeEvolutionSelections < 3)
	{
		BroodGameMode->ChooseEvolution(0);
		DungeonLevel5JailerVisualSmokeEvolutionSelections++;
	}

	DungeonLevel5JailerVisualSmokeStep++;
	if (DungeonLevel5JailerVisualSmokeStep >= 36)
	{
		UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VISUAL_SMOKE_FAILED: timed out after %d steps."), DungeonLevel5JailerVisualSmokeStep);
		GetWorldTimerManager().ClearTimer(DungeonLevel5JailerVisualSmokeTimerHandle);
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

void ABroodPlayerController::RequestViewportScreenshot(const FString& FileName, const TCHAR* Marker)
{
	const FString AutomationDir = FPaths::ProjectSavedDir() / TEXT("Automation");
	IFileManager::Get().MakeDirectory(*AutomationDir, true);
	const FString ScreenshotPath = AutomationDir / FileName;
	FScreenshotRequest::RequestScreenshot(ScreenshotPath, true, false);
	UE_LOG(LogTemp, Display, TEXT("%s: %s"), Marker, *ScreenshotPath);
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
