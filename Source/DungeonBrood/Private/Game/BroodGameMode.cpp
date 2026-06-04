#include "Game/BroodGameMode.h"

#include "Characters/BroodCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/PointLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextRenderActor.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Pickups/BiomassPickup.h"
#include "Player/BroodPlayerController.h"
#include "Components/TextRenderComponent.h"

namespace
{
	void ApplyPlaceholderColor(UStaticMeshComponent* MeshComponent, const FLinearColor& Color)
	{
		if (!MeshComponent)
		{
			return;
		}

		UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
			DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
		}
	}
}

ABroodGameMode::ABroodGameMode()
{
	DefaultPawnClass = ABroodCharacter::StaticClass();
	PlayerControllerClass = ABroodPlayerController::StaticClass();
}

void ABroodGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (IsEditorAuthoredDungeonLevel5Map())
	{
		UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EDITOR_AUTHORED_MAP_READY: using saved editor-built Level 5 map content; runtime environment blockout is skipped."));
	}
	else
	{
		BuildTestArena();
		BuildDungeonDressing();
		BuildDungeonLevel5MoodLighting();
		BuildDungeonLevel5ModularFloor();
		BuildDungeonLevel5RouteLayout();
		BuildDungeonLevel5RouteCollision();
		BuildDungeonLevel5FirstViewArchitecture();
		BuildDungeonLevel5ThresholdCorridorArchitecture();
		BuildDungeonLevel5Landmarks();
		BuildDungeonLevel5SurfaceTemptation();
		BuildDungeonLevel5PrologueDressing();
		BuildDungeonLevel5TraversalComposition();
		BuildDungeonLevel5CreepyDressing();
		BuildDungeonLevel5JailerApproachDressing();
		BuildDungeonLevel5JailerBossRoomArchitecture();
		BuildDungeonLevel5ExitArchitecture();
		BuildDungeonLevel5ExitDescentRoomArchitecture();
		BuildDungeonLevel5BrokenThresholdDressing();
		BuildDungeonLevel5ExitSeal();
		BuildVillageCryptDressing();
	}
	EnsurePlayer();
	StartNextEncounter();

	if (IsEditorAuthoredDungeonLevel5Map())
	{
		FTimerHandle FloorCheckTimerHandle;
		GetWorldTimerManager().SetTimer(
			FloorCheckTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (!PlayerCharacter)
				{
					UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_EDITOR_FLOOR_COLLISION_FAILED: player character missing during floor check."));
					return;
				}

				const float PlayerZ = PlayerCharacter->GetActorLocation().Z;
				if (PlayerZ < 40.0f)
				{
					UE_LOG(LogTemp, Error, TEXT("BROOD_DUNGEON_LEVEL5_EDITOR_FLOOR_COLLISION_FAILED: player fell through editor-authored Level 5 floor, Z=%.1f."), PlayerZ);
					return;
				}

				UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EDITOR_FLOOR_COLLISION_READY: player remained above the editor-authored Level 5 floor, Z=%.1f."), PlayerZ);
			}),
			2.0f,
			false);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 12.0f, FColor::Cyan, TEXT("Dungeon Brood: WASD move | Mouse aim | RMB orbit | LMB/Space attack | Shift dodge | R restart"));
	}
}

void ABroodGameMode::NotifyEnemyKilled(ABroodEnemyCharacter* Enemy)
{
	if (!Enemy || bRunComplete)
	{
		return;
	}

	AliveEnemies.RemoveAll([Enemy](const TObjectPtr<ABroodEnemyCharacter>& TrackedEnemy)
	{
		return TrackedEnemy == Enemy;
	});
	float BiomassReward = Enemy->GetBiomassReward();
	if (PlayerCharacter)
	{
		BiomassReward *= PlayerCharacter->GetBiomassRewardMultiplier();
		PlayerCharacter->OnEnemyKilled();
	}
	SpawnBiomassPickup(Enemy->GetActorLocation() + FVector(0.0f, 0.0f, 34.0f), BiomassReward);

	PrintHud();

	if (AliveEnemies.IsEmpty())
	{
		if (EncounterIndex >= 3)
		{
			CompleteDungeonLevel5();
			return;
		}

		OfferEvolutionChoices();
	}
}

bool ABroodGameMode::IsEditorAuthoredDungeonLevel5Map() const
{
	const UWorld* World = GetWorld();
	return World && World->GetMapName().Contains(TEXT("L_Dungeon_Level5_Editor"));
}

bool ABroodGameMode::IsDungeonLevel5Complete() const
{
	return bDungeonLevel5Complete;
}

void ABroodGameMode::TriggerDungeonLevel5SurfaceTrial(ABroodCharacter* Player)
{
	if (bSurfaceTrialTriggered || !Player || Player->IsDead())
	{
		return;
	}

	bSurfaceTrialTriggered = true;
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_ENTERED: larva climbed toward surface light from Dungeon Level 5."));

	const FLinearColor GuardColor(0.74f, 0.72f, 0.66f);
	const FLinearColor ArcherColor(0.46f, 0.50f, 0.58f);
	const FLinearColor KnightColor(0.92f, 0.86f, 0.62f);
	const FLinearColor FleshColor(0.68f, 0.50f, 0.36f);
	const FLinearColor WeaponColor(0.82f, 0.82f, 0.76f);
	const FVector SurfaceBase(-1040.0f, 840.0f, 120.0f);
	const FVector GuardBase(-1120.0f, 260.0f, 300.0f);

	auto SpawnSurfaceHumanoid = [this, FleshColor, WeaponColor](
		const FString& Prefix,
		const FVector& BaseLocation,
		const FLinearColor& ArmorColor,
		const FRotator& FacingRotation,
		bool bKnight,
		bool bArcher)
	{
		SpawnDecorativeMesh(Prefix + TEXT("_ThreatSigil"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BaseLocation + FVector(0.0f, 0.0f, 24.0f), bKnight ? FVector(0.98f, 0.72f, 0.026f) : FVector(0.78f, 0.54f, 0.024f), FLinearColor(1.0f, 0.08f, 0.02f), FRotator::ZeroRotator);
		SpawnDecorativeMesh(Prefix + TEXT("_Torso"), TEXT("/Engine/BasicShapes/Cube.Cube"), BaseLocation + FVector(0.0f, 0.0f, 128.0f), bKnight ? FVector(0.96f, 0.64f, 1.28f) : FVector(0.74f, 0.50f, 1.04f), ArmorColor, FacingRotation);
		SpawnDecorativeMesh(Prefix + TEXT("_Head"), TEXT("/Engine/BasicShapes/Sphere.Sphere"), BaseLocation + FVector(0.0f, 0.0f, bKnight ? 254.0f : 218.0f), bKnight ? FVector(0.52f) : FVector(0.42f), FleshColor, FacingRotation);
		SpawnDecorativeMesh(Prefix + TEXT("_LeftLeg"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BaseLocation + FVector(-48.0f, -30.0f, 54.0f), FVector(0.14f, 0.14f, 0.64f), ArmorColor, FRotator(0.0f, 0.0f, 4.0f));
		SpawnDecorativeMesh(Prefix + TEXT("_RightLeg"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BaseLocation + FVector(48.0f, 30.0f, 54.0f), FVector(0.14f, 0.14f, 0.64f), ArmorColor, FRotator(0.0f, 0.0f, -4.0f));

		if (bArcher)
		{
			SpawnDecorativeMesh(Prefix + TEXT("_BowUpperLimb"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BaseLocation + FVector(86.0f, -74.0f, 198.0f), FVector(0.055f, 0.055f, 0.72f), WeaponColor, FRotator(28.0f, 18.0f, 0.0f));
			SpawnDecorativeMesh(Prefix + TEXT("_BowLowerLimb"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BaseLocation + FVector(86.0f, -74.0f, 142.0f), FVector(0.055f, 0.055f, 0.72f), WeaponColor, FRotator(-28.0f, 18.0f, 0.0f));
			SpawnDecorativeMesh(Prefix + TEXT("_BowString"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BaseLocation + FVector(108.0f, -82.0f, 170.0f), FVector(0.025f, 0.025f, 0.86f), FLinearColor(0.92f, 0.88f, 0.74f), FRotator(0.0f, 18.0f, 0.0f));
			SpawnDecorativeMesh(Prefix + TEXT("_Arrow"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BaseLocation + FVector(48.0f, -96.0f, 170.0f), FVector(0.026f, 0.026f, 0.92f), WeaponColor, FRotator(78.0f, -40.0f, 0.0f));
		}
		else
		{
			SpawnDecorativeMesh(Prefix + TEXT("_Weapon"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), BaseLocation + FVector(96.0f, -70.0f, bKnight ? 194.0f : 164.0f), FVector(0.095f, 0.095f, bKnight ? 1.72f : 1.28f), WeaponColor, FRotator(62.0f, -28.0f, 0.0f));
		}

		if (bKnight)
		{
			SpawnDecorativeMesh(Prefix + TEXT("_Shield"), TEXT("/Engine/BasicShapes/Cube.Cube"), BaseLocation + FVector(-100.0f, 76.0f, 170.0f), FVector(0.18f, 0.62f, 0.58f), ArmorColor, FRotator(0.0f, 24.0f, 0.0f));
		}
	};

	SpawnSurfaceHumanoid(TEXT("SurfaceTrialGuard_Left"), GuardBase + FVector(-80.0f, 0.0f, 0.0f), GuardColor, FRotator(0.0f, 24.0f, 0.0f), false, false);
	SpawnSurfaceHumanoid(TEXT("SurfaceTrialGuard_Right"), GuardBase + FVector(100.0f, 120.0f, 0.0f), GuardColor, FRotator(0.0f, -18.0f, 0.0f), false, false);
	SpawnSurfaceHumanoid(TEXT("SurfaceTrialArcher"), GuardBase + FVector(135.0f, -110.0f, 0.0f), ArcherColor, FRotator(0.0f, 34.0f, 0.0f), false, true);
	SpawnSurfaceHumanoid(TEXT("SurfaceTrialKnight"), GuardBase + FVector(320.0f, 35.0f, 0.0f), KnightColor, FRotator(0.0f, -42.0f, 0.0f), true, false);
	SpawnPointLight(TEXT("SurfaceTrialKillingLight"), SurfaceBase + FVector(20.0f, 0.0f, 260.0f), FLinearColor(1.0f, 0.86f, 0.56f), 5200.0f, 820.0f);
	SpawnPointLight(TEXT("SurfaceTrialReviewLight"), GuardBase + FVector(20.0f, 40.0f, 430.0f), FLinearColor(1.0f, 0.92f, 0.72f), 8600.0f, 1050.0f);
	SpawnLevelText(TEXT("SurfaceTrialDeathText"), TEXT("THIS BODY WAS NOT READY"), SurfaceBase + FVector(0.0f, 0.0f, 210.0f), FRotator(0.0f, 210.0f, 0.0f), FColor(255, 80, 50), 34.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_GUARDS_READY: surface guard, archer and knight silhouettes spawned."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_HUMANOID_SILHOUETTES_READY: surface trial attackers use torso, head, limb and weapon props instead of single blocker meshes."));
	UE_LOG(LogTemp, Display, TEXT("THIS_BODY_WAS_NOT_READY: surface trial lesson delivered."));

	UGameplayStatics::ApplyDamage(Player, 10000.0f, nullptr, this, UDamageType::StaticClass());
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_DEATH_READY: surface trial applied lethal damage and returns player to Level 5 on restart."));
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(3100, 10.0f, FColor::Red, TEXT("THIS BODY WAS NOT READY - Press R to restart in Dungeon Level 5"));
	}
}

void ABroodGameMode::ChooseEvolution(int32 ChoiceIndex)
{
	if (!bAwaitingEvolutionChoice || !CurrentChoices.IsValidIndex(ChoiceIndex) || !PlayerCharacter)
	{
		return;
	}

	const FBroodEvolutionChoice Choice = CurrentChoices[ChoiceIndex];
	if (PlayerCharacter->ApplyEvolution(Choice.EvolutionId))
	{
		bAwaitingEvolutionChoice = false;
		CurrentChoices.Reset();
		StartNextEncounter();
	}
}

void ABroodGameMode::EnsurePlayer()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		return;
	}

	PlayerCharacter = Cast<ABroodCharacter>(PlayerController->GetPawn());
	if (!PlayerCharacter)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		PlayerCharacter = GetWorld()->SpawnActor<ABroodCharacter>(DefaultPawnClass, FVector(0.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
		PlayerController->Possess(PlayerCharacter);
	}
}

void ABroodGameMode::BuildTestArena()
{
	SpawnStaticCube(TEXT("ArenaFloorCollision"), FVector(0.0f, 0.0f, -55.0f), FVector(18.0f, 18.0f, 0.5f), FLinearColor(0.08f, 0.08f, 0.09f), false);
	SpawnStaticCube(TEXT("NorthWallCollision"), FVector(0.0f, 1800.0f, 120.0f), FVector(18.0f, 0.4f, 2.8f), FLinearColor(0.18f, 0.18f, 0.22f), false);
	SpawnStaticCube(TEXT("SouthWallCollision"), FVector(0.0f, -1800.0f, 120.0f), FVector(18.0f, 0.4f, 2.8f), FLinearColor(0.18f, 0.18f, 0.22f), false);
	SpawnStaticCube(TEXT("EastWallCollision"), FVector(1800.0f, 0.0f, 120.0f), FVector(0.4f, 18.0f, 2.8f), FLinearColor(0.18f, 0.18f, 0.22f), false);
	SpawnStaticCube(TEXT("WestWallCollision"), FVector(-1800.0f, 0.0f, 120.0f), FVector(0.4f, 18.0f, 2.8f), FLinearColor(0.18f, 0.18f, 0.22f), false);
}

void ABroodGameMode::BuildDungeonDressing()
{
	const FLinearColor StoneColor(0.12f, 0.14f, 0.16f);
	const FLinearColor DarkStoneColor(0.06f, 0.07f, 0.08f);
	const FLinearColor MetalColor(0.20f, 0.16f, 0.10f);
	const FLinearColor EmberColor(1.0f, 0.18f, 0.03f);
	const FLinearColor BiomassColor(0.05f, 0.85f, 0.30f);

	for (int32 X = -3; X <= 3; ++X)
	{
		for (int32 Y = -3; Y <= 3; ++Y)
		{
			SpawnDecorativeMesh(
				FString::Printf(TEXT("DungeonFloor_%d_%d"), X, Y),
				TEXT("/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Floor.SM_Crypt_Floor"),
				FVector(X * 500.0f, Y * 500.0f, 1.0f),
				FVector(2.15f),
				StoneColor,
				FRotator(0.0f, (X + Y) % 2 == 0 ? 0.0f : 90.0f, 0.0f),
				true);
		}
	}

	for (int32 Index = -3; Index <= 3; ++Index)
	{
		const float Offset = Index * 520.0f;
		SpawnDecorativeMesh(FString::Printf(TEXT("NorthDungeonWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), FVector(Offset, 1700.0f, 120.0f), FVector(1.55f), StoneColor, FRotator(0.0f, 180.0f, 0.0f), false);
		SpawnDecorativeMesh(FString::Printf(TEXT("SouthDungeonWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), FVector(Offset, -1700.0f, 120.0f), FVector(1.55f), StoneColor, FRotator::ZeroRotator, false);
		if (Index != 0)
		{
			SpawnDecorativeMesh(FString::Printf(TEXT("EastDungeonWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), FVector(1700.0f, Offset, 120.0f), FVector(1.55f), StoneColor, FRotator(0.0f, -90.0f, 0.0f), false);
			SpawnDecorativeMesh(FString::Printf(TEXT("WestDungeonWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), FVector(-1700.0f, Offset, 120.0f), FVector(1.55f), StoneColor, FRotator(0.0f, 90.0f, 0.0f), false);
		}
	}

	SpawnDecorativeMesh(TEXT("EastDungeonDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay"), FVector(1700.0f, 0.0f, 120.0f), FVector(1.65f), StoneColor, FRotator(0.0f, -90.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("WestDungeonDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay"), FVector(-1700.0f, 0.0f, 120.0f), FVector(1.65f), StoneColor, FRotator(0.0f, 90.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("NorthDungeonArch"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Decorative_Arches.SM_Decorative_Arches"), FVector(0.0f, 1660.0f, 170.0f), FVector(1.8f), StoneColor, FRotator(0.0f, 180.0f, 0.0f), false);

	const TArray<FVector> PillarLocations = {
		FVector(-1420.0f, -1420.0f, 170.0f), FVector(-1420.0f, 1420.0f, 170.0f),
		FVector(1420.0f, -1420.0f, 170.0f), FVector(1420.0f, 1420.0f, 170.0f),
		FVector(-520.0f, -1420.0f, 170.0f), FVector(-520.0f, 1420.0f, 170.0f),
		FVector(520.0f, -1420.0f, 170.0f), FVector(520.0f, 1420.0f, 170.0f)
	};
	for (int32 Index = 0; Index < PillarLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonPillar_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"), PillarLocations[Index], FVector(1.6f), StoneColor, FRotator::ZeroRotator, false);
	}

	const TArray<FVector> BrazierLocations = {
		FVector(-1050.0f, -1050.0f, 75.0f), FVector(-1050.0f, 1050.0f, 75.0f),
		FVector(1050.0f, -1050.0f, 75.0f), FVector(1050.0f, 1050.0f, 75.0f)
	};
	for (int32 Index = 0; Index < BrazierLocations.Num(); ++Index)
	{
		const FVector& Location = BrazierLocations[Index];
		SpawnDecorativeMesh(FString::Printf(TEXT("BrazierStand_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Fire_Pit.SM_Fire_Pit"), Location, FVector(1.1f), MetalColor, FRotator::ZeroRotator, false);
		SpawnPointLight(FString::Printf(TEXT("BrazierLight_%d"), Index), Location + FVector(0.0f, 0.0f, 120.0f), EmberColor, 2200.0f, 700.0f);
	}

	const TArray<FVector> CrystalLocations = {
		FVector(-1280.0f, -380.0f, 30.0f), FVector(-1320.0f, 420.0f, 42.0f),
		FVector(1260.0f, -420.0f, 36.0f), FVector(1300.0f, 360.0f, 28.0f)
	};
	for (int32 Index = 0; Index < CrystalLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(FString::Printf(TEXT("BiomassCrystal_%d"), Index), TEXT("/Engine/BasicShapes/Cone.Cone"), CrystalLocations[Index], FVector(0.34f, 0.34f, 1.25f), BiomassColor, FRotator(0.0f, Index * 47.0f, 0.0f));
	}

	const TArray<FVector> WallDecorLocations = {
		FVector(-980.0f, -1690.0f, 130.0f), FVector(0.0f, -1690.0f, 130.0f), FVector(980.0f, -1690.0f, 130.0f),
		FVector(-980.0f, 1690.0f, 130.0f), FVector(0.0f, 1690.0f, 130.0f), FVector(980.0f, 1690.0f, 130.0f)
	};
	for (int32 Index = 0; Index < WallDecorLocations.Num(); ++Index)
	{
		const bool bNorthSide = WallDecorLocations[Index].Y > 0.0f;
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonWallDecor_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall_Decorative_A.SM_Dungeon_Wall_Decorative_A"), WallDecorLocations[Index], FVector(1.8f), StoneColor, FRotator(0.0f, bNorthSide ? 180.0f : 0.0f, 0.0f), false);
	}

	const TArray<FVector> CryptPropLocations = {
		FVector(-1120.0f, -650.0f, 20.0f), FVector(1120.0f, 640.0f, 20.0f),
		FVector(-760.0f, 980.0f, 20.0f), FVector(820.0f, -960.0f, 20.0f)
	};
	SpawnDecorativeMesh(TEXT("DungeonCoffin_0"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Stone_Coffin.SM_Stone_Coffin"), CryptPropLocations[0], FVector(1.1f), StoneColor, FRotator(0.0f, 28.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonCoffin_1"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Coffin_No_Lid.SM_Coffin_No_Lid"), CryptPropLocations[1], FVector(1.1f), StoneColor, FRotator(0.0f, -28.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonGargoyle_0"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Gargoyle_Statue_On_Stand.SM_Gargoyle_Statue_On_Stand"), CryptPropLocations[2], FVector(0.9f), StoneColor, FRotator(0.0f, 145.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonSkeleton_0"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Skeleton_01.SM_Skeleton_01"), CryptPropLocations[3], FVector(1.0f), StoneColor, FRotator(0.0f, -35.0f, 0.0f), false);

	for (int32 Index = 0; Index < 12; ++Index)
	{
		const float Side = Index % 2 == 0 ? -1.0f : 1.0f;
		const FVector Location(-1320.0f + (Index % 4) * 850.0f, Side * (760.0f + (Index % 3) * 120.0f), 18.0f);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonRubble_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Dirt_Mound_A.SM_Dirt_Mound_A"), Location, FVector(0.75f + (Index % 3) * 0.08f), DarkStoneColor, FRotator(0.0f, Index * 29.0f, 0.0f), false);
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_IMPORTED_WORLD_ASSETS_READY: Medieval Dungeon meshes linked."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_WORLD_DRESSING_READY: dungeon floor, pillars, braziers, crystals and rubble spawned."));
}

void ABroodGameMode::BuildDungeonLevel5MoodLighting()
{
	APostProcessVolume* MoodVolume = GetWorld()->SpawnActor<APostProcessVolume>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (MoodVolume)
	{
		MoodVolume->Rename(TEXT("DungeonLevel5MoodPostProcess"));
		MoodVolume->bUnbound = true;
		MoodVolume->Settings.bOverride_AutoExposureBias = true;
		MoodVolume->Settings.AutoExposureBias = -0.45f;
		MoodVolume->Settings.bOverride_ColorSaturation = true;
		MoodVolume->Settings.ColorSaturation = FVector4(0.86f, 0.90f, 0.94f, 1.0f);
		MoodVolume->Settings.bOverride_ColorContrast = true;
		MoodVolume->Settings.ColorContrast = FVector4(1.08f, 1.06f, 1.04f, 1.0f);
		MoodVolume->Settings.bOverride_VignetteIntensity = true;
		MoodVolume->Settings.VignetteIntensity = 0.18f;
		MoodVolume->Settings.bOverride_MotionBlurAmount = true;
		MoodVolume->Settings.MotionBlurAmount = 0.0f;
	}

	const FLinearColor FloorReadColor(0.30f, 0.42f, 0.50f);
	const FLinearColor EnemyReadColor(0.45f, 0.36f, 0.24f);
	SpawnPointLight(TEXT("DungeonLevel5ReadableCenterLight"), FVector(80.0f, 0.0f, 260.0f), FloorReadColor, 1350.0f, 900.0f);
	SpawnPointLight(TEXT("DungeonLevel5ReadableEnemyLight"), FVector(620.0f, 100.0f, 240.0f), EnemyReadColor, 1800.0f, 720.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_MOOD_LIGHTING_READY: exposure, contrast and vignette tuned for darker Broken Threshold review screenshots."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_READABLE_DARKNESS_READY: Level 5 stays dark while center floor, larva and first enemies remain readable."));
}

void ABroodGameMode::BuildDungeonLevel5ModularFloor()
{
	const FLinearColor FloorColor(0.075f, 0.085f, 0.095f);
	const FLinearColor WetFloorColor(0.045f, 0.075f, 0.085f);

	for (int32 X = -4; X <= 4; ++X)
	{
		for (int32 Y = -4; Y <= 4; ++Y)
		{
			const bool bWetPatch = (X + Y) % 4 == 0;
			SpawnDecorativeMesh(
				FString::Printf(TEXT("DungeonLevel5ModularFloor_%d_%d"), X, Y),
				TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"),
				FVector(X * 500.0f, Y * 500.0f, 24.0f),
				FVector(2.15f),
				bWetPatch ? WetFloorColor : FloorColor,
				FRotator(0.0f, ((X * 17 + Y * 29) % 4) * 90.0f, 0.0f),
				true);
		}
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_MODULAR_FLOOR_READY: expanded MedievalDungeon SM_Floor modular floor layer spawned for La Soglia Spezzata."));
}

void ABroodGameMode::BuildDungeonLevel5RouteLayout()
{
	const FLinearColor WallColor(0.10f, 0.115f, 0.125f);
	const FLinearColor DarkWallColor(0.055f, 0.065f, 0.075f);
	const FLinearColor RustColor(0.24f, 0.105f, 0.055f);

	const TArray<FVector> SpineWallLocations = {
		FVector(-820.0f, 520.0f, 110.0f),
		FVector(-320.0f, 520.0f, 110.0f),
		FVector(320.0f, -520.0f, 110.0f),
		FVector(820.0f, -520.0f, 110.0f)
	};
	const TArray<FRotator> SpineWallRotations = {
		FRotator(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, 180.0f, 0.0f),
		FRotator(0.0f, 180.0f, 0.0f)
	};
	for (int32 Index = 0; Index < SpineWallLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5RouteWall_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Wall_Floor_Trim.SM_Wall_Floor_Trim"),
			SpineWallLocations[Index] + FVector(0.0f, 0.0f, -28.0f),
			FVector(1.45f),
			WallColor,
			SpineWallRotations[Index],
			false);
	}

	const TArray<FVector> ShortWallLocations = {
		FVector(-1120.0f, 160.0f, 82.0f),
		FVector(-720.0f, -160.0f, 82.0f),
		FVector(720.0f, 160.0f, 82.0f),
		FVector(1120.0f, -160.0f, 82.0f)
	};
	for (int32 Index = 0; Index < ShortWallLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5RouteWall_Short_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"),
			ShortWallLocations[Index] + FVector(0.0f, 0.0f, -34.0f),
			FVector(1.25f),
			WallColor,
			FRotator(0.0f, Index < 2 ? 90.0f : -90.0f, 0.0f),
			false);
	}

	const TArray<FVector> CellLocations = {
		FVector(-1470.0f, 480.0f, 132.0f),
		FVector(-1470.0f, 900.0f, 132.0f),
		FVector(1470.0f, -480.0f, 132.0f),
		FVector(1470.0f, -900.0f, 132.0f)
	};
	for (int32 Index = 0; Index < CellLocations.Num(); ++Index)
	{
		const bool bLeftSide = CellLocations[Index].X < 0.0f;
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5CellBlock_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front"),
			CellLocations[Index],
			FVector(1.1f),
			DarkWallColor,
			FRotator(0.0f, bLeftSide ? 90.0f : -90.0f, 0.0f),
			false);
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5CellBlock_Door_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door"),
			CellLocations[Index] + FVector(bLeftSide ? 18.0f : -18.0f, 0.0f, 0.0f),
			FVector(1.0f),
			RustColor,
			FRotator(0.0f, bLeftSide ? 82.0f : -82.0f, 0.0f),
			true);
	}

	const TArray<FVector> JailerChamberLocations = {
		FVector(610.0f, -1470.0f, 120.0f),
		FVector(1180.0f, -760.0f, 120.0f),
		FVector(1460.0f, -1180.0f, 120.0f)
	};
	const TArray<FRotator> JailerChamberRotations = {
		FRotator(0.0f, 138.0f, 0.0f),
		FRotator(0.0f, 48.0f, 0.0f),
		FRotator(0.0f, -42.0f, 0.0f)
	};
	for (int32 Index = 0; Index < JailerChamberLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5JailerChamber_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Wall_Floor_Trim.SM_Wall_Floor_Trim"),
			JailerChamberLocations[Index] + FVector(0.0f, 0.0f, -44.0f),
			FVector(1.48f),
			DarkWallColor,
			JailerChamberRotations[Index],
			false);
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_LOW_PROFILE_ROUTE_READY: central route uses MedievalDungeon floor trim and edge-stone assets to reduce camera occlusion."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ROUTE_LAYOUT_READY: low-profile route trim, side cells and Jailer Husk chamber dressing spawned."));
}

void ABroodGameMode::BuildDungeonLevel5RouteCollision()
{
	const TArray<FVector> SpineBlockerLocations = {
		FVector(-820.0f, 520.0f, 125.0f),
		FVector(-320.0f, 520.0f, 125.0f),
		FVector(320.0f, -520.0f, 125.0f),
		FVector(820.0f, -520.0f, 125.0f)
	};
	for (int32 Index = 0; Index < SpineBlockerLocations.Num(); ++Index)
	{
		SpawnRouteCollisionBlock(
			FString::Printf(TEXT("DungeonLevel5RouteCollision_Spine_%d"), Index),
			SpineBlockerLocations[Index],
			FVector(2.35f, 0.16f, 1.45f),
			FRotator(0.0f, Index < 2 ? 0.0f : 180.0f, 0.0f));
	}

	const TArray<FVector> ShortBlockerLocations = {
		FVector(-1120.0f, 160.0f, 100.0f),
		FVector(-720.0f, -160.0f, 100.0f),
		FVector(720.0f, 160.0f, 100.0f),
		FVector(1120.0f, -160.0f, 100.0f)
	};
	for (int32 Index = 0; Index < ShortBlockerLocations.Num(); ++Index)
	{
		SpawnRouteCollisionBlock(
			FString::Printf(TEXT("DungeonLevel5RouteCollision_Short_%d"), Index),
			ShortBlockerLocations[Index],
			FVector(0.18f, 1.18f, 1.0f),
			FRotator(0.0f, Index < 2 ? 90.0f : -90.0f, 0.0f));
	}

	const TArray<FVector> CellBlockerLocations = {
		FVector(-1500.0f, 480.0f, 132.0f),
		FVector(-1500.0f, 900.0f, 132.0f),
		FVector(1500.0f, -480.0f, 132.0f),
		FVector(1500.0f, -900.0f, 132.0f),
		FVector(-1560.0f, -620.0f, 132.0f),
		FVector(-1560.0f, -1040.0f, 132.0f)
	};
	for (int32 Index = 0; Index < CellBlockerLocations.Num(); ++Index)
	{
		const bool bLeftSide = CellBlockerLocations[Index].X < 0.0f;
		SpawnRouteCollisionBlock(
			FString::Printf(TEXT("DungeonLevel5RouteCollision_Cell_%d"), Index),
			CellBlockerLocations[Index],
			FVector(0.22f, 1.05f, 1.55f),
			FRotator(0.0f, bLeftSide ? 90.0f : -90.0f, 0.0f));
	}

	const TArray<FVector> JailerBlockerLocations = {
		FVector(760.0f, -1180.0f, 120.0f),
		FVector(1180.0f, -760.0f, 120.0f),
		FVector(1460.0f, -1180.0f, 120.0f)
	};
	const TArray<FRotator> JailerBlockerRotations = {
		FRotator(0.0f, 138.0f, 0.0f),
		FRotator(0.0f, 48.0f, 0.0f),
		FRotator(0.0f, -42.0f, 0.0f)
	};
	for (int32 Index = 0; Index < JailerBlockerLocations.Num(); ++Index)
	{
		SpawnRouteCollisionBlock(
			FString::Printf(TEXT("DungeonLevel5RouteCollision_Jailer_%d"), Index),
			JailerBlockerLocations[Index],
			FVector(1.85f, 0.16f, 1.35f),
			JailerBlockerRotations[Index]);
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ROUTE_COLLISION_READY: selective invisible collision follows Level 5 walls, cells and Jailer chamber."));
}

void ABroodGameMode::BuildDungeonLevel5FirstViewArchitecture()
{
	const FLinearColor WetStoneColor(0.085f, 0.095f, 0.105f);
	const FLinearColor DarkStoneColor(0.045f, 0.052f, 0.060f);
	const FLinearColor RustColor(0.25f, 0.12f, 0.055f);
	const FLinearColor ChainColor(0.18f, 0.17f, 0.15f);
	const FLinearColor ColdLight(0.05f, 0.55f, 0.32f);
	const FLinearColor WarmLeak(0.95f, 0.68f, 0.32f);

	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewLeftPier"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"), FVector(-485.0f, 365.0f, 108.0f), FVector(0.56f), WetStoneColor, FRotator::ZeroRotator, false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewRightPier"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"), FVector(485.0f, -365.0f, 108.0f), FVector(0.56f), WetStoneColor, FRotator::ZeroRotator, false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewBackWallCap"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Wall_Floor_Trim.SM_Wall_Floor_Trim"), FVector(0.0f, 760.0f, 66.0f), FVector(0.86f), WetStoneColor, FRotator(0.0f, 180.0f, 0.0f), false);

	const TArray<FVector> ThresholdPillars = {
		FVector(-350.0f, 660.0f, 112.0f),
		FVector(-70.0f, 820.0f, 112.0f),
		FVector(350.0f, -660.0f, 112.0f),
		FVector(70.0f, -820.0f, 112.0f)
	};
	for (int32 Index = 0; Index < ThresholdPillars.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5FirstViewThresholdPillar_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"),
			ThresholdPillars[Index],
			FVector(0.48f),
			DarkStoneColor,
			FRotator::ZeroRotator,
			false);
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewSurfaceThresholdDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay"), FVector(-500.0f, 845.0f, 118.0f), FVector(0.38f), DarkStoneColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewDescentThresholdDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay"), FVector(500.0f, -845.0f, 118.0f), FVector(0.38f), DarkStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewSurfaceWallRibA"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Wall_Floor_Trim.SM_Wall_Floor_Trim"), FVector(-165.0f, 705.0f, 64.0f), FVector(0.54f), DarkStoneColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewSurfaceWallRibB"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Wall_Floor_Trim.SM_Wall_Floor_Trim"), FVector(-610.0f, 475.0f, 64.0f), FVector(0.54f), DarkStoneColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewDescentWallRibA"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Wall_Floor_Trim.SM_Wall_Floor_Trim"), FVector(165.0f, -705.0f, 64.0f), FVector(0.54f), DarkStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewDescentWallRibB"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Wall_Floor_Trim.SM_Wall_Floor_Trim"), FVector(610.0f, -475.0f, 64.0f), FVector(0.54f), DarkStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);

	const TArray<FVector> SideButtresses = {
		FVector(-760.0f, -420.0f, 78.0f),
		FVector(-460.0f, -680.0f, 78.0f),
		FVector(460.0f, 680.0f, 78.0f),
		FVector(760.0f, 420.0f, 78.0f)
	};
	const TArray<FRotator> SideButtressRotations = {
		FRotator(0.0f, 28.0f, 0.0f),
		FRotator(0.0f, -18.0f, 0.0f),
		FRotator(0.0f, 162.0f, 0.0f),
		FRotator(0.0f, 208.0f, 0.0f)
	};
	for (int32 Index = 0; Index < SideButtresses.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5FirstViewButtress_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"),
			SideButtresses[Index],
			FVector(0.66f),
			DarkStoneColor,
			SideButtressRotations[Index],
			false);
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewSurfaceBrokenGate"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door"), FVector(-450.0f, 620.0f, 118.0f), FVector(0.78f), RustColor, FRotator(0.0f, -28.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewDescentBrokenGate"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door"), FVector(450.0f, -620.0f, 118.0f), FVector(0.78f), RustColor, FRotator(0.0f, 152.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewHangingChainA"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"), FVector(-350.0f, 500.0f, 220.0f), FVector(0.58f), ChainColor, FRotator(0.0f, -22.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5FirstViewHangingChainB"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"), FVector(350.0f, -500.0f, 220.0f), FVector(0.58f), ChainColor, FRotator(0.0f, 158.0f, 0.0f), false);
	SpawnPointLight(TEXT("DungeonLevel5FirstViewSurfaceLeak"), FVector(-520.0f, 560.0f, 230.0f), WarmLeak, 950.0f, 360.0f);
	SpawnPointLight(TEXT("DungeonLevel5FirstViewDescentGuide"), FVector(520.0f, -560.0f, 210.0f), ColdLight, 1100.0f, 380.0f);
	SpawnPointLight(TEXT("DungeonLevel5FirstViewThresholdSpineLight"), FVector(-65.0f, 670.0f, 205.0f), WarmLeak, 760.0f, 300.0f);
	SpawnPointLight(TEXT("DungeonLevel5FirstViewDepthSpineLight"), FVector(65.0f, -670.0f, 190.0f), ColdLight, 860.0f, 320.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_FIRST_VIEW_ARCHITECTURE_READY: spawn view has pillars, doorway silhouettes, broken gates, low trim and hanging chains without blocking gameplay camera."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_FIRST_VIEW_THRESHOLD_SPINE_READY: spawn view has paired threshold pillars, wall ribs and small doorway silhouettes to break up the floor field."));
}

void ABroodGameMode::BuildDungeonLevel5ThresholdCorridorArchitecture()
{
	const FLinearColor WetStoneColor(0.082f, 0.092f, 0.102f);
	const FLinearColor DarkStoneColor(0.036f, 0.043f, 0.050f);
	const FLinearColor RustColor(0.22f, 0.105f, 0.052f);
	const FLinearColor BoneColor(0.66f, 0.61f, 0.49f);
	const FLinearColor WarmTorch(0.95f, 0.52f, 0.24f);
	const FLinearColor ColdDepth(0.05f, 0.44f, 0.30f);

	const TArray<FVector> CorridorFloorLocations = {
		FVector(0.0f, -420.0f, 18.0f),
		FVector(0.0f, 0.0f, 18.0f),
		FVector(0.0f, 420.0f, 18.0f)
	};
	for (int32 Index = 0; Index < CorridorFloorLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5ThresholdCorridorFloor_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"),
			CorridorFloorLocations[Index],
			FVector(1.18f),
			WetStoneColor,
			FRotator::ZeroRotator,
			false);
	}

	const TArray<FVector> SideWallLocations = {
		FVector(-520.0f, -320.0f, 112.0f),
		FVector(-520.0f, 180.0f, 112.0f),
		FVector(520.0f, -320.0f, 112.0f),
		FVector(520.0f, 180.0f, 112.0f)
	};
	const TArray<FRotator> SideWallRotations = {
		FRotator(0.0f, 90.0f, 0.0f),
		FRotator(0.0f, 90.0f, 0.0f),
		FRotator(0.0f, -90.0f, 0.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	};
	for (int32 Index = 0; Index < SideWallLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5ThresholdCorridorWall_%d"), Index),
			TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"),
			SideWallLocations[Index],
			FVector(0.78f),
			WetStoneColor,
			SideWallRotations[Index],
			false);
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay"), FVector(0.0f, 1030.0f, 112.0f), FVector(0.50f), DarkStoneColor, FRotator(0.0f, 180.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorStairs"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_10.SM_Stairs_10"), FVector(0.0f, 1240.0f, 34.0f), FVector(0.70f), DarkStoneColor, FRotator(0.0f, 180.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorHalfArch_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Half_Arch.SM_Half_Arch"), FVector(-445.0f, 820.0f, 128.0f), FVector(0.42f), WetStoneColor, FRotator(0.0f, 180.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorHalfArch_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Half_Arch.SM_Half_Arch"), FVector(445.0f, 820.0f, 128.0f), FVector(0.42f), WetStoneColor, FRotator::ZeroRotator, false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorPillar_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"), FVector(-420.0f, 715.0f, 112.0f), FVector(0.50f), WetStoneColor, FRotator::ZeroRotator, false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorPillar_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"), FVector(420.0f, 715.0f, 112.0f), FVector(0.50f), WetStoneColor, FRotator::ZeroRotator, false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorBackWallSeal"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), FVector(0.0f, 1430.0f, 118.0f), FVector(0.90f), DarkStoneColor, FRotator(0.0f, 180.0f, 0.0f), false);

	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorBrokenCellDoor"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door"), FVector(-325.0f, -300.0f, 105.0f), FVector(0.62f), RustColor, FRotator(0.0f, 24.0f, -7.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorCellFront_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front"), FVector(-520.0f, 120.0f, 118.0f), FVector(0.62f), RustColor, FRotator(0.0f, 90.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorCellFront_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front"), FVector(520.0f, 120.0f, 118.0f), FVector(0.62f), RustColor, FRotator(0.0f, -90.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorStoneCoffin"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Stone_Coffin.SM_Stone_Coffin"), FVector(-330.0f, -565.0f, 40.0f), FVector(0.68f), WetStoneColor, FRotator(0.0f, 20.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorGibbetCage"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Open.SM_Gibbet_Cage_Open"), FVector(335.0f, -545.0f, 120.0f), FVector(0.58f), RustColor, FRotator(0.0f, -24.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorSkull"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull"), FVector(240.0f, -280.0f, 34.0f), FVector(0.46f), BoneColor, FRotator(0.0f, -35.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorRibs"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"), FVector(315.0f, -205.0f, 28.0f), FVector(0.52f), BoneColor, FRotator(0.0f, 16.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorChain_Left"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"), FVector(-365.0f, 255.0f, 205.0f), FVector(0.50f), RustColor, FRotator(6.0f, 90.0f, 84.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorChain_Right"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"), FVector(365.0f, 255.0f, 205.0f), FVector(0.50f), RustColor, FRotator(-6.0f, -90.0f, 84.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorTorch_Left"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch"), FVector(-505.0f, 360.0f, 145.0f), FVector(0.56f), RustColor, FRotator(0.0f, 90.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ThresholdCorridorTorch_Right"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch"), FVector(505.0f, 360.0f, 145.0f), FVector(0.56f), RustColor, FRotator(0.0f, -90.0f, 0.0f), false);

	SpawnPointLight(TEXT("DungeonLevel5ThresholdCorridorTorchLight_Left"), FVector(-430.0f, 330.0f, 190.0f), WarmTorch, 900.0f, 300.0f);
	SpawnPointLight(TEXT("DungeonLevel5ThresholdCorridorTorchLight_Right"), FVector(430.0f, 330.0f, 190.0f), WarmTorch, 900.0f, 300.0f);
	SpawnPointLight(TEXT("DungeonLevel5ThresholdCorridorDepthLight"), FVector(0.0f, 1030.0f, 180.0f), ColdDepth, 760.0f, 420.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_THRESHOLD_CORRIDOR_READY: Level 5 opening has an asset-based corridor spine with side walls, doorway, stairs, half arches, pillars and crypt dressing."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_THRESHOLD_CORRIDOR_CAMERA_CLEAR_READY: Level 5 threshold doorway, stairs and arch hints are pushed deeper and to the sides so the ARPG camera keeps a clear combat read."));
}

void ABroodGameMode::BuildDungeonLevel5Landmarks()
{
	const FLinearColor WarmSurfaceLight(1.0f, 0.62f, 0.28f);
	const FLinearColor ColdDepthLight(0.08f, 0.55f, 0.30f);
	const FLinearColor BoneColor(0.72f, 0.66f, 0.52f);
	const FLinearColor DarkStoneColor(0.08f, 0.09f, 0.10f);

	SpawnDecorativeMesh(TEXT("UpperSurfaceStair"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_10.SM_Stairs_10"), FVector(-1180.0f, 1320.0f, 40.0f), FVector(1.35f), DarkStoneColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("UpperSurfaceDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Doorway.SM_Crypt_Doorway"), FVector(-1420.0f, 1480.0f, 140.0f), FVector(1.15f), DarkStoneColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnPointLight(TEXT("UpperSurfaceStairLight"), FVector(-1180.0f, 1320.0f, 260.0f), WarmSurfaceLight, 1800.0f, 620.0f);

	SpawnDecorativeMesh(TEXT("LowerDungeonStair"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_10.SM_Stairs_10"), FVector(1180.0f, -1320.0f, 36.0f), FVector(1.35f), DarkStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("LowerDungeonDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Doorway.SM_Crypt_Doorway"), FVector(1520.0f, -1620.0f, 118.0f), FVector(0.68f), DarkStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnPointLight(TEXT("LowerDungeonStairLight"), FVector(1180.0f, -1320.0f, 230.0f), ColdDepthLight, 1600.0f, 640.0f);

	const TArray<FVector> BonePileLocations = {
		FVector(-420.0f, 1260.0f, 36.0f),
		FVector(460.0f, -1260.0f, 36.0f),
		FVector(-1260.0f, 520.0f, 36.0f),
		FVector(1260.0f, -520.0f, 36.0f)
	};
	for (int32 Index = 0; Index < BonePileLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5BonePile_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"), BonePileLocations[Index], FVector(0.85f), BoneColor, FRotator(0.0f, Index * 37.0f, 0.0f), false);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5Skull_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull"), BonePileLocations[Index] + FVector(48.0f, -28.0f, 10.0f), FVector(0.7f), BoneColor, FRotator(0.0f, Index * 51.0f, 0.0f), false);
	}

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ART_READY: upper and lower stairs, crypt doorways and bone piles spawned."));
}

void ABroodGameMode::BuildDungeonLevel5SurfaceTemptation()
{
	const FVector TriggerLocation(-1180.0f, 1320.0f, 110.0f);
	ATriggerBox* SurfaceTrialTrigger = GetWorld()->SpawnActor<ATriggerBox>(TriggerLocation, FRotator::ZeroRotator);
	if (SurfaceTrialTrigger)
	{
		SurfaceTrialTrigger->Rename(TEXT("DungeonLevel5SurfaceTrialTrigger"));
		SurfaceTrialTrigger->SetActorHiddenInGame(true);
		SurfaceTrialTrigger->SetActorScale3D(FVector(2.2f, 2.2f, 1.2f));
		if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(SurfaceTrialTrigger->GetCollisionComponent()))
		{
			BoxComponent->SetBoxExtent(FVector(135.0f, 135.0f, 120.0f));
			BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			BoxComponent->SetHiddenInGame(true);
			BoxComponent->SetVisibility(false);
		}
		SurfaceTrialTrigger->OnActorBeginOverlap.AddDynamic(this, &ABroodGameMode::OnSurfaceTrialTriggerOverlap);
	}

	SpawnLevelText(
		TEXT("DungeonLevel5SurfaceTrialPrompt"),
		TEXT("THE SURFACE IS NEAR"),
		FVector(-1290.0f, 1380.0f, 210.0f),
		FRotator(0.0f, 135.0f, 0.0f),
		FColor(255, 210, 120),
		26.0f);
	SpawnPointLight(TEXT("DungeonLevel5SurfaceTemptationLight"), FVector(-1260.0f, 1400.0f, 300.0f), FLinearColor(1.0f, 0.78f, 0.38f), 3600.0f, 720.0f);
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_SURFACE_TEMPTATION_READY: surface stair trigger and warning prompt spawned."));
}

void ABroodGameMode::BuildDungeonLevel5PrologueDressing()
{
	const FLinearColor DryBloodColor(0.18f, 0.015f, 0.008f);
	const FLinearColor OldBoneColor(0.60f, 0.55f, 0.44f);
	const FLinearColor DeadFleshColor(0.20f, 0.10f, 0.075f);
	const FLinearColor BiomassColor(0.08f, 0.95f, 0.38f);
	const FLinearColor ShadowColor(0.025f, 0.018f, 0.017f);
	const FLinearColor SurfaceDustColor(0.72f, 0.62f, 0.44f);

	const TArray<FVector> BloodPoolLocations = {
		FVector(-250.0f, -160.0f, 31.0f),
		FVector(150.0f, -220.0f, 31.0f),
		FVector(-90.0f, 220.0f, 31.0f),
		FVector(300.0f, 120.0f, 31.0f)
	};
	for (int32 Index = 0; Index < BloodPoolLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5PrologueDryBlood_%d"), Index),
			TEXT("/Engine/BasicShapes/Cylinder.Cylinder"),
			BloodPoolLocations[Index],
			FVector(0.72f + Index * 0.08f, 0.34f + Index * 0.05f, 0.014f),
			DryBloodColor,
			FRotator(0.0f, Index * 31.0f, 0.0f));
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueFallScar"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-80.0f, 20.0f, 33.0f), FVector(1.8f, 0.12f, 0.028f), ShadowColor, FRotator(0.0f, -24.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueFallScarCross"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-100.0f, 18.0f, 34.0f), FVector(0.85f, 0.08f, 0.024f), DryBloodColor, FRotator(0.0f, 48.0f, 0.0f));

	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueRibCage"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"), FVector(330.0f, -130.0f, 42.0f), FVector(0.78f), OldBoneColor, FRotator(0.0f, 62.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueSkull"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull"), FVector(245.0f, -300.0f, 38.0f), FVector(0.58f), OldBoneColor, FRotator(0.0f, -18.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueArmBone"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm.SM_Bones_Arm"), FVector(-330.0f, 170.0f, 39.0f), FVector(0.62f), OldBoneColor, FRotator(0.0f, 118.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueLegBone"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Leg.SM_Bones_Leg"), FVector(60.0f, 350.0f, 39.0f), FVector(0.62f), OldBoneColor, FRotator(0.0f, -76.0f, 0.0f), false);

	const TArray<FVector> SmallCarcassLocations = {
		FVector(-420.0f, -110.0f, 44.0f),
		FVector(420.0f, 230.0f, 44.0f),
		FVector(-30.0f, -430.0f, 44.0f)
	};
	for (int32 Index = 0; Index < SmallCarcassLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5PrologueSmallCarcass_%d"), Index),
			TEXT("/Engine/BasicShapes/Sphere.Sphere"),
			SmallCarcassLocations[Index],
			FVector(0.26f, 0.13f, 0.08f),
			DeadFleshColor,
			FRotator(0.0f, Index * 57.0f, 0.0f));
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5PrologueCarcassTail_%d"), Index),
			TEXT("/Engine/BasicShapes/Cube.Cube"),
			SmallCarcassLocations[Index] + FVector(-38.0f, 12.0f, -8.0f),
			FVector(0.30f, 0.035f, 0.018f),
			DeadFleshColor,
			FRotator(0.0f, Index * 41.0f, 0.0f));
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueAncientMeatCore"), TEXT("/Engine/BasicShapes/Sphere.Sphere"), FVector(510.0f, -360.0f, 54.0f), FVector(0.24f, 0.24f, 0.14f), BiomassColor, FRotator::ZeroRotator);
	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueAncientMeatShardA"), TEXT("/Engine/BasicShapes/Cone.Cone"), FVector(470.0f, -420.0f, 58.0f), FVector(0.12f, 0.12f, 0.42f), BiomassColor, FRotator(0.0f, 18.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5PrologueAncientMeatShardB"), TEXT("/Engine/BasicShapes/Cone.Cone"), FVector(560.0f, -325.0f, 56.0f), FVector(0.10f, 0.10f, 0.36f), BiomassColor, FRotator(0.0f, -36.0f, 0.0f));
	SpawnPointLight(TEXT("DungeonLevel5PrologueBiomassLight"), FVector(510.0f, -360.0f, 145.0f), BiomassColor, 950.0f, 320.0f);

	SpawnDecorativeMesh(TEXT("DungeonLevel5SurfaceDustBeam"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-760.0f, 720.0f, 140.0f), FVector(0.10f, 1.05f, 0.025f), SurfaceDustColor, FRotator(0.0f, -42.0f, 0.0f));
	SpawnPointLight(TEXT("DungeonLevel5PrologueSurfaceLeakLight"), FVector(-760.0f, 720.0f, 230.0f), SurfaceDustColor, 1200.0f, 420.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_PROLOGUE_DRESSING_READY: spawn chamber has fall scar, dry blood, small carcasses, bones, ancient biomass and surface dust light."));
}

void ABroodGameMode::BuildDungeonLevel5TraversalComposition()
{
	const FLinearColor WarmSurfaceStone(0.55f, 0.42f, 0.27f);
	const FLinearColor ColdDescentStone(0.035f, 0.095f, 0.070f);
	const FLinearColor BrokenEdgeStone(0.070f, 0.080f, 0.085f);
	const FLinearColor SurfaceLight(0.95f, 0.72f, 0.38f);
	const FLinearColor DescentLight(0.02f, 0.64f, 0.25f);
	const FLinearColor WarningRed(0.26f, 0.025f, 0.015f);

	const TArray<FVector> SurfaceTrail = {
		FVector(-260.0f, 320.0f, 36.0f),
		FVector(-520.0f, 650.0f, 36.0f),
		FVector(-820.0f, 960.0f, 36.0f),
		FVector(-1080.0f, 1240.0f, 36.0f)
	};
	for (int32 Index = 0; Index < SurfaceTrail.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5SurfaceTrailSlab_%d"), Index),
			TEXT("/Engine/BasicShapes/Cube.Cube"),
			SurfaceTrail[Index],
			FVector(0.92f, 0.22f, 0.022f),
			WarmSurfaceStone,
			FRotator(0.0f, -42.0f + Index * 5.0f, 0.0f),
			true);
	}

	const TArray<FVector> DescentTrail = {
		FVector(260.0f, -300.0f, 36.0f),
		FVector(530.0f, -620.0f, 36.0f),
		FVector(820.0f, -930.0f, 36.0f),
		FVector(1080.0f, -1210.0f, 36.0f)
	};
	for (int32 Index = 0; Index < DescentTrail.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5DescentTrailSlab_%d"), Index),
			TEXT("/Engine/BasicShapes/Cube.Cube"),
			DescentTrail[Index],
			FVector(1.05f, 0.24f, 0.022f),
			ColdDescentStone,
			FRotator(0.0f, 138.0f - Index * 4.0f, 0.0f),
			true);
	}

	const TArray<FVector> ThresholdEdges = {
		FVector(-520.0f, 120.0f, 36.0f),
		FVector(520.0f, -120.0f, 36.0f),
		FVector(-180.0f, 620.0f, 36.0f),
		FVector(180.0f, -620.0f, 36.0f)
	};
	const TArray<FRotator> ThresholdEdgeRotations = {
		FRotator(0.0f, 24.0f, 0.0f),
		FRotator(0.0f, 204.0f, 0.0f),
		FRotator(0.0f, -64.0f, 0.0f),
		FRotator(0.0f, 116.0f, 0.0f)
	};
	for (int32 Index = 0; Index < ThresholdEdges.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5BrokenThresholdEdge_%d"), Index),
			TEXT("/Engine/BasicShapes/Cube.Cube"),
			ThresholdEdges[Index],
			FVector(0.78f, 0.13f, 0.028f),
			BrokenEdgeStone,
			ThresholdEdgeRotations[Index],
			true);
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5SurfaceChoiceRune"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-565.0f, 700.0f, 48.0f), FVector(1.18f, 0.045f, 0.035f), SurfaceLight, FRotator(0.0f, -42.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5SurfaceChoiceRuneCross"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-565.0f, 700.0f, 50.0f), FVector(0.72f, 0.045f, 0.035f), WarningRed, FRotator(0.0f, 48.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5DescentChoiceRuneA"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(595.0f, -690.0f, 48.0f), FVector(1.18f, 0.045f, 0.035f), DescentLight, FRotator(0.0f, 138.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5DescentChoiceRuneB"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(595.0f, -690.0f, 50.0f), FVector(1.18f, 0.045f, 0.035f), DescentLight, FRotator(0.0f, 48.0f, 0.0f));
	SpawnPointLight(TEXT("DungeonLevel5SurfacePathReadLight"), FVector(-620.0f, 720.0f, 190.0f), SurfaceLight, 900.0f, 380.0f);
	SpawnPointLight(TEXT("DungeonLevel5DescentPathReadLight"), FVector(600.0f, -710.0f, 170.0f), DescentLight, 1100.0f, 420.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_TRAVERSAL_COMPOSITION_READY: spawn chamber has warm surface trail, cold descent trail, broken threshold edging and readable up/down choice runes."));
}

void ABroodGameMode::OnSurfaceTrialTriggerOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	TriggerDungeonLevel5SurfaceTrial(Cast<ABroodCharacter>(OtherActor));
}

void ABroodGameMode::BuildDungeonLevel5CreepyDressing()
{
	const FLinearColor DeadFleshColor(0.13f, 0.065f, 0.050f);
	const FLinearColor OldBoneColor(0.58f, 0.52f, 0.41f);
	const FLinearColor IronColor(0.055f, 0.060f, 0.065f);
	const FLinearColor BloodShadowColor(0.11f, 0.012f, 0.008f);
	const FLinearColor ColdCreepLight(0.06f, 0.24f, 0.18f);
	const FLinearColor DeepShadowColor(0.014f, 0.014f, 0.016f);

	const TArray<FVector> HangingChainLocations = {
		FVector(-690.0f, 1110.0f, 210.0f),
		FVector(380.0f, 980.0f, 215.0f),
		FVector(890.0f, -360.0f, 220.0f),
		FVector(-1120.0f, -540.0f, 215.0f)
	};
	for (int32 Index = 0; Index < HangingChainLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5CreepyHangingChain_%d"), Index),
			Index % 2 == 0 ? TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A") : TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"),
			HangingChainLocations[Index],
			FVector(0.72f),
			IronColor,
			FRotator(14.0f + Index * 3.0f, Index * 31.0f, 88.0f),
			false);
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5CreepyChainShadow_%d"), Index),
			TEXT("/Engine/BasicShapes/Cylinder.Cylinder"),
			HangingChainLocations[Index] + FVector(0.0f, 0.0f, -172.0f),
			FVector(0.34f, 0.12f, 0.010f),
			DeepShadowColor,
			FRotator(90.0f, Index * 37.0f, 0.0f));
	}

	const TArray<FVector> DryCorpseLocations = {
		FVector(-1040.0f, 430.0f, 48.0f),
		FVector(420.0f, 560.0f, 48.0f),
		FVector(-620.0f, -780.0f, 48.0f)
	};
	const TArray<FRotator> DryCorpseRotations = {
		FRotator(0.0f, -32.0f, 0.0f),
		FRotator(0.0f, 74.0f, 0.0f),
		FRotator(0.0f, 142.0f, 0.0f)
	};
	for (int32 Index = 0; Index < DryCorpseLocations.Num(); ++Index)
	{
		const FVector Base = DryCorpseLocations[Index];
		const FRotator Rotation = DryCorpseRotations[Index];
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5CreepyDryCorpseRibs_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"), Base + FVector(0.0f, 0.0f, 4.0f), FVector(0.52f), OldBoneColor, Rotation, false);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5CreepyDryCorpseSkull_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull"), Base + FVector(58.0f, -20.0f, 12.0f), FVector(0.42f), OldBoneColor, Rotation + FRotator(0.0f, 23.0f, 0.0f), false);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5CreepyDryCorpseCloth_%d"), Index), TEXT("/Engine/BasicShapes/Cube.Cube"), Base + FVector(-12.0f, 0.0f, -4.0f), FVector(0.72f, 0.18f, 0.018f), DeadFleshColor, Rotation);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5CreepyDryCorpseBlood_%d"), Index), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), Base + FVector(8.0f, 4.0f, -8.0f), FVector(0.46f, 0.22f, 0.010f), BloodShadowColor, FRotator(90.0f, Index * 41.0f, 0.0f));
	}

	const TArray<FVector> WarningBoneTrail = {
		FVector(-330.0f, 470.0f, 42.0f),
		FVector(240.0f, -510.0f, 42.0f),
		FVector(710.0f, -760.0f, 42.0f),
		FVector(-870.0f, 720.0f, 42.0f),
		FVector(1030.0f, -360.0f, 42.0f)
	};
	for (int32 Index = 0; Index < WarningBoneTrail.Num(); ++Index)
	{
		const bool bSkull = Index == 3;
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5CreepyWarningBone_%d"), Index),
			bSkull ? TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull") : TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm.SM_Bones_Arm"),
			WarningBoneTrail[Index],
			bSkull ? FVector(0.42f) : FVector(0.46f),
			OldBoneColor,
			FRotator(0.0f, 18.0f + Index * 53.0f, 0.0f),
			false);
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5CreepySurfaceNooseShadow"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-760.0f, 930.0f, 48.0f), FVector(0.92f, 0.055f, 0.026f), BloodShadowColor, FRotator(0.0f, -42.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5CreepyDescentScratchA"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(470.0f, -540.0f, 49.0f), FVector(0.82f, 0.030f, 0.022f), BloodShadowColor, FRotator(0.0f, 128.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5CreepyDescentScratchB"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(505.0f, -578.0f, 50.0f), FVector(0.58f, 0.026f, 0.020f), BloodShadowColor, FRotator(0.0f, 154.0f, 0.0f));
	SpawnPointLight(TEXT("DungeonLevel5CreepyColdPocketLight"), FVector(-940.0f, -620.0f, 155.0f), ColdCreepLight, 520.0f, 340.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_CREEPY_DRESSING_READY: Level 5 adds hanging chains, dry corpses, warning bones, drag scratches and cold shadow pockets without blocking the main play route."));
}

void ABroodGameMode::BuildDungeonLevel5JailerApproachDressing()
{
	const FLinearColor OldIronColor(0.07f, 0.07f, 0.075f);
	const FLinearColor RustColor(0.34f, 0.14f, 0.055f);
	const FLinearColor WarningBoneColor(0.64f, 0.58f, 0.46f);
	const FLinearColor DeadStoneColor(0.035f, 0.048f, 0.052f);
	const FLinearColor KeyGlowColor(0.58f, 0.48f, 0.22f);
	const FLinearColor BloodOldColor(0.19f, 0.02f, 0.015f);

	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerApproachGate_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front"), FVector(710.0f, -860.0f, 128.0f), FVector(1.08f), OldIronColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerApproachGate_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front"), FVector(1110.0f, -560.0f, 128.0f), FVector(1.08f), OldIronColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerApproachBrokenDoor"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door"), FVector(910.0f, -720.0f, 92.0f), FVector(1.02f), RustColor, FRotator(18.0f, -18.0f, -64.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerApproachDeadLanding"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1020.0f, -1035.0f, 38.0f), FVector(2.8f, 1.15f, 0.035f), DeadStoneColor, FRotator(0.0f, -42.0f, 0.0f));

	const TArray<FVector> ChainLocations = {
		FVector(540.0f, -720.0f, 46.0f),
		FVector(720.0f, -890.0f, 48.0f),
		FVector(940.0f, -1050.0f, 48.0f),
		FVector(1220.0f, -960.0f, 50.0f)
	};
	const TArray<FRotator> ChainRotations = {
		FRotator(0.0f, -38.0f, 0.0f),
		FRotator(0.0f, -58.0f, 0.0f),
		FRotator(0.0f, -28.0f, 0.0f),
		FRotator(0.0f, 16.0f, 0.0f)
	};
	for (int32 Index = 0; Index < ChainLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5JailerApproachFloorChain_%d"), Index),
			Index % 2 == 0 ? TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A") : TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"),
			ChainLocations[Index],
			FVector(0.62f),
			OldIronColor,
			ChainRotations[Index],
			false);
	}

	const TArray<FVector> WarningBoneLocations = {
		FVector(650.0f, -610.0f, 44.0f),
		FVector(760.0f, -1110.0f, 44.0f),
		FVector(1160.0f, -760.0f, 44.0f),
		FVector(1310.0f, -1160.0f, 44.0f)
	};
	for (int32 Index = 0; Index < WarningBoneLocations.Num(); ++Index)
	{
		const bool bSkull = Index % 2 == 0;
		SpawnDecorativeMesh(
			FString::Printf(TEXT("DungeonLevel5JailerApproachWarningBone_%d"), Index),
			bSkull ? TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull") : TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"),
			WarningBoneLocations[Index],
			bSkull ? FVector(0.72f) : FVector(0.58f),
			WarningBoneColor,
			FRotator(0.0f, Index * 47.0f, 0.0f),
			false);
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerApproachOldBlood_0"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(820.0f, -815.0f, 34.0f), FVector(0.72f, 0.38f, 0.014f), BloodOldColor, FRotator(90.0f, 18.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerApproachOldBlood_1"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(1170.0f, -1010.0f, 34.0f), FVector(0.54f, 0.26f, 0.014f), BloodOldColor, FRotator(90.0f, -35.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerApproachKeyRune_A"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1040.0f, -940.0f, 55.0f), FVector(0.72f, 0.045f, 0.04f), KeyGlowColor, FRotator(0.0f, -42.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerApproachKeyRune_B"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1040.0f, -940.0f, 57.0f), FVector(0.28f, 0.045f, 0.04f), KeyGlowColor, FRotator(0.0f, 48.0f, 0.0f));
	SpawnPointLight(TEXT("DungeonLevel5JailerApproachKeyLight"), FVector(1040.0f, -940.0f, 170.0f), KeyGlowColor, 820.0f, 390.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_APPROACH_READY: lower route now has a broken jail gate, floor chains, warning bones, old blood, and key-rune lead-in to Jailer Husk."));
}

void ABroodGameMode::BuildDungeonLevel5JailerBossRoomArchitecture()
{
	const FLinearColor DeadStoneColor(0.045f, 0.052f, 0.058f);
	const FLinearColor DarkVoidColor(0.012f, 0.015f, 0.018f);
	const FLinearColor RustColor(0.28f, 0.14f, 0.07f);
	const FLinearColor BoneColor(0.52f, 0.48f, 0.38f);
	const FLinearColor TorchColor(0.82f, 0.28f, 0.08f);
	const FLinearColor ColdKeyColor(0.04f, 0.44f, 0.32f);
	const FVector RoomCenter(1120.0f, -930.0f, 36.0f);

	for (int32 X = -1; X <= 1; ++X)
	{
		for (int32 Y = -1; Y <= 1; ++Y)
		{
			SpawnDecorativeMesh(
				FString::Printf(TEXT("DungeonLevel5JailerRoomFloor_%d_%d"), X, Y),
				TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"),
				RoomCenter + FVector(X * 430.0f, Y * 430.0f, -18.0f),
				FVector(0.96f),
				DeadStoneColor,
				FRotator(0.0f, (X - Y) * 90.0f, 0.0f),
				false);
		}
	}

	for (int32 Index = -1; Index <= 1; ++Index)
	{
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5JailerRoomBackWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), RoomCenter + FVector(Index * 430.0f, -640.0f, 118.0f), FVector(0.82f), DeadStoneColor, FRotator(0.0f, 0.0f, 0.0f), false);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5JailerRoomLeftWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), RoomCenter + FVector(-640.0f, Index * 430.0f, 118.0f), FVector(0.82f), DeadStoneColor, FRotator(0.0f, 90.0f, 0.0f), false);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5JailerRoomRightWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), RoomCenter + FVector(640.0f, Index * 430.0f, 118.0f), FVector(0.82f), DeadStoneColor, FRotator(0.0f, -90.0f, 0.0f), false);
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomCeiling_Back"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat"), RoomCenter + FVector(0.0f, -420.0f, 345.0f), FVector(0.72f), DeadStoneColor, FRotator::ZeroRotator, false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomCeiling_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat"), RoomCenter + FVector(-420.0f, 0.0f, 345.0f), FVector(0.72f), DeadStoneColor, FRotator(0.0f, 90.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomCeiling_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat"), RoomCenter + FVector(420.0f, 0.0f, 345.0f), FVector(0.72f), DeadStoneColor, FRotator(0.0f, 180.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomDarkRear"), TEXT("/Engine/BasicShapes/Cube.Cube"), RoomCenter + FVector(0.0f, -780.0f, 160.0f), FVector(8.2f, 0.16f, 2.0f), DarkVoidColor, FRotator::ZeroRotator);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomThresholdEdge"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"), RoomCenter + FVector(0.0f, 515.0f, 38.0f), FVector(0.92f), DeadStoneColor, FRotator::ZeroRotator, false);

	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomStocks"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Stocks_Game_Ready.SM_Stocks_Game_Ready"), RoomCenter + FVector(-330.0f, -360.0f, 45.0f), FVector(0.88f), RustColor, FRotator(0.0f, 22.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomGibbet"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Wall_Mount.SM_Gibbet_Cage_Wall_Mount"), RoomCenter + FVector(430.0f, -520.0f, 160.0f), FVector(0.88f), RustColor, FRotator(0.0f, 0.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomBallChain"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Ball_And_Chain.SM_Ball_And_Chain"), RoomCenter + FVector(310.0f, 190.0f, -10.0f), FVector(0.92f), RustColor, FRotator(0.0f, -35.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomHangingChain_Left"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"), RoomCenter + FVector(-250.0f, -475.0f, 190.0f), FVector(0.72f), RustColor, FRotator(12.0f, 0.0f, 86.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomHangingChain_Right"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"), RoomCenter + FVector(245.0f, -455.0f, 180.0f), FVector(0.72f), RustColor, FRotator(-8.0f, 0.0f, 84.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomKeyPile"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Key.SM_Lock_Key"), RoomCenter + FVector(-95.0f, -95.0f, 44.0f), FVector(0.58f), ColdKeyColor, FRotator(0.0f, 54.0f, 86.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomRibWarning"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"), RoomCenter + FVector(190.0f, -250.0f, 28.0f), FVector(0.64f), BoneColor, FRotator(0.0f, -22.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomSkullWarning"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull"), RoomCenter + FVector(120.0f, -330.0f, 38.0f), FVector(0.48f), BoneColor, FRotator(0.0f, 34.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomTorch_Left"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch"), RoomCenter + FVector(-480.0f, -515.0f, 140.0f), FVector(0.62f), TorchColor, FRotator(0.0f, 0.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerRoomTorch_Right"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch"), RoomCenter + FVector(480.0f, -515.0f, 140.0f), FVector(0.62f), TorchColor, FRotator(0.0f, 0.0f, 0.0f), false);
	SpawnPointLight(TEXT("DungeonLevel5JailerRoomTorchLight_Left"), RoomCenter + FVector(-480.0f, -515.0f, 195.0f), TorchColor, 900.0f, 360.0f);
	SpawnPointLight(TEXT("DungeonLevel5JailerRoomTorchLight_Right"), RoomCenter + FVector(480.0f, -515.0f, 195.0f), TorchColor, 900.0f, 360.0f);
	SpawnPointLight(TEXT("DungeonLevel5JailerRoomKeyColdLight"), RoomCenter + FVector(0.0f, -40.0f, 135.0f), ColdKeyColor, 760.0f, 350.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_BOSS_ROOM_AUTHORED_READY: in-level Jailer room has pack floors, walls, ceiling, stocks, gibbet, chains, bones, torches and key-light staging."));
}

void ABroodGameMode::BuildDungeonLevel5ExitArchitecture()
{
	const FLinearColor ColdDepthLight(0.04f, 0.50f, 0.28f);
	const FLinearColor TorchColor(0.86f, 0.32f, 0.08f);
	const FLinearColor OpenLockColor(0.46f, 0.36f, 0.16f);
	const FLinearColor OldIronColor(0.06f, 0.065f, 0.07f);
	const FLinearColor WetStoneColor(0.05f, 0.07f, 0.075f);

	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitArchDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay"), FVector(1450.0f, -1480.0f, 118.0f), FVector(0.48f), WetStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitArchHalfArch_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Half_Arch.SM_Half_Arch"), FVector(840.0f, -1010.0f, 120.0f), FVector(0.46f), WetStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitArchHalfArch_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Half_Arch.SM_Half_Arch"), FVector(1335.0f, -1455.0f, 120.0f), FVector(0.46f), WetStoneColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitArchPillar_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"), FVector(925.0f, -1135.0f, 116.0f), FVector(0.62f), WetStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitArchPillar_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar"), FVector(1310.0f, -980.0f, 116.0f), FVector(0.62f), WetStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitVisibleStairs"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_5.SM_Stairs_5"), FVector(1080.0f, -1160.0f, 40.0f), FVector(0.94f), WetStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitOpenedLock"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Open.SM_Lock_Open"), FVector(970.0f, -1040.0f, 80.0f), FVector(0.72f), OpenLockColor, FRotator(0.0f, 20.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitOpenChain_Left"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"), FVector(1000.0f, -960.0f, 108.0f), FVector(0.56f), OldIronColor, FRotator(0.0f, 28.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitOpenChain_Right"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"), FVector(1235.0f, -1225.0f, 108.0f), FVector(0.56f), OldIronColor, FRotator(0.0f, -36.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitTorch_Left"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch"), FVector(870.0f, -1185.0f, 132.0f), FVector(0.74f), TorchColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitTorch_Right"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch"), FVector(1275.0f, -855.0f, 132.0f), FVector(0.74f), TorchColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnPointLight(TEXT("DungeonLevel5ExitTorchLight_Left"), FVector(870.0f, -1185.0f, 180.0f), TorchColor, 780.0f, 320.0f);
	SpawnPointLight(TEXT("DungeonLevel5ExitTorchLight_Right"), FVector(1275.0f, -855.0f, 180.0f), TorchColor, 780.0f, 320.0f);
	SpawnPointLight(TEXT("DungeonLevel5ExitDepthArchLight"), FVector(1120.0f, -1210.0f, 175.0f), ColdDepthLight, 1200.0f, 460.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_ARCHITECTURE_READY: lower descent uses MedievalDungeon doorway, half arches, pillars, visible steps, open lock, chains and torches."));
}

void ABroodGameMode::BuildDungeonLevel5ExitDescentRoomArchitecture()
{
	const FLinearColor WetStoneColor(0.045f, 0.062f, 0.066f);
	const FLinearColor DarkDepthColor(0.006f, 0.010f, 0.012f);
	const FLinearColor ColdDepthLight(0.02f, 0.36f, 0.24f);
	const FLinearColor RustColor(0.22f, 0.11f, 0.06f);
	const FLinearColor BoneColor(0.50f, 0.46f, 0.36f);
	const FVector ExitCenter(1120.0f, -1210.0f, 36.0f);

	for (int32 X = -1; X <= 1; ++X)
	{
		for (int32 Y = -1; Y <= 1; ++Y)
		{
			SpawnDecorativeMesh(
				FString::Printf(TEXT("DungeonLevel5ExitRoomFloor_%d_%d"), X, Y),
				TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"),
				ExitCenter + FVector(X * 430.0f, Y * 430.0f, -18.0f),
				FVector(0.94f),
				WetStoneColor,
				FRotator(0.0f, (X + Y) * 90.0f, 0.0f),
				false);
		}
	}

	for (int32 Index = -1; Index <= 1; ++Index)
	{
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5ExitRoomBackWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), ExitCenter + FVector(Index * 430.0f, 640.0f, 118.0f), FVector(0.80f), WetStoneColor, FRotator(0.0f, 180.0f, 0.0f), false);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5ExitRoomLeftWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), ExitCenter + FVector(-640.0f, Index * 430.0f, 118.0f), FVector(0.80f), WetStoneColor, FRotator(0.0f, 90.0f, 0.0f), false);
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5ExitRoomRightWall_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall"), ExitCenter + FVector(640.0f, Index * 430.0f, 118.0f), FVector(0.80f), WetStoneColor, FRotator(0.0f, -90.0f, 0.0f), false);
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomCeiling_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat"), ExitCenter + FVector(-360.0f, 150.0f, 330.0f), FVector(0.72f), WetStoneColor, FRotator(0.0f, 90.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomCeiling_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat"), ExitCenter + FVector(360.0f, 150.0f, 330.0f), FVector(0.72f), WetStoneColor, FRotator(0.0f, 180.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomDarkBack"), TEXT("/Engine/BasicShapes/Cube.Cube"), ExitCenter + FVector(0.0f, -560.0f, 135.0f), FVector(8.0f, 0.22f, 1.8f), DarkDepthColor, FRotator(0.0f, 0.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomDepthHole"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), ExitCenter + FVector(0.0f, -95.0f, 10.0f), FVector(1.05f, 1.05f, 0.055f), DarkDepthColor, FRotator::ZeroRotator);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomDepthRim"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"), ExitCenter + FVector(0.0f, -95.0f, 35.0f), FVector(0.84f), WetStoneColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomLowerStairs"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_10.SM_Stairs_10"), ExitCenter + FVector(120.0f, -220.0f, 4.0f), FVector(0.72f), WetStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomCryptDoorway"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Doorway.SM_Crypt_Doorway"), ExitCenter + FVector(260.0f, -395.0f, 105.0f), FVector(0.58f), WetStoneColor, FRotator(0.0f, 138.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomOpenLock"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Open.SM_Lock_Open"), ExitCenter + FVector(-210.0f, -210.0f, 62.0f), FVector(0.62f), RustColor, FRotator(0.0f, 25.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomBrokenChainA"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"), ExitCenter + FVector(-145.0f, -280.0f, 82.0f), FVector(0.52f), RustColor, FRotator(0.0f, 28.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomBrokenChainB"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"), ExitCenter + FVector(180.0f, -30.0f, 76.0f), FVector(0.52f), RustColor, FRotator(0.0f, -34.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomWarningSkull"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull"), ExitCenter + FVector(-350.0f, 80.0f, 35.0f), FVector(0.46f), BoneColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitRoomWarningRibs"), TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"), ExitCenter + FVector(-275.0f, 160.0f, 28.0f), FVector(0.54f), BoneColor, FRotator(0.0f, 16.0f, 0.0f), false);
	SpawnPointLight(TEXT("DungeonLevel5ExitRoomDepthGreenLight"), ExitCenter + FVector(0.0f, -120.0f, 118.0f), ColdDepthLight, 1500.0f, 520.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_DESCENT_ROOM_AUTHORED_READY: in-level descent room has pack floor, side walls, ceiling, doorway, lower stairs, broken lock, chains, bones and a dark depth well."));
}

void ABroodGameMode::BuildDungeonLevel5BrokenThresholdDressing()
{
	const FLinearColor RustColor(0.34f, 0.16f, 0.08f);
	const FLinearColor IronColor(0.08f, 0.08f, 0.09f);
	const FLinearColor RootColor(0.18f, 0.09f, 0.04f);
	const FLinearColor WetStoneColor(0.035f, 0.06f, 0.065f);
	const FLinearColor WaterColor(0.02f, 0.22f, 0.24f);
	const FLinearColor SurfaceLightColor(0.95f, 0.82f, 0.52f);
	const FLinearColor ShadowColor(0.025f, 0.03f, 0.035f);

	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitShadowMask"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1120.0f, -1210.0f, 33.0f), FVector(0.72f, 0.46f, 0.022f), ShadowColor, FRotator(0.0f, -42.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5WetStoneLanding"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"), FVector(950.0f, -1080.0f, 18.0f), FVector(1.18f), WetStoneColor, FRotator(0.0f, -42.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitDarkShelf_Left"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"), FVector(585.0f, -1025.0f, 43.0f), FVector(0.72f), ShadowColor, FRotator(0.0f, -42.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitDarkShelf_Right"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"), FVector(1320.0f, -760.0f, 43.0f), FVector(0.64f), ShadowColor, FRotator(0.0f, 138.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitWetShelf_Bottom"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"), FVector(840.0f, -1390.0f, 43.0f), FVector(0.68f), WetStoneColor, FRotator(0.0f, -42.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitBrokenSlab_0"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(570.0f, -900.0f, 38.0f), FVector(0.75f, 0.28f, 0.035f), WetStoneColor, FRotator(0.0f, 16.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitBrokenSlab_1"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(320.0f, -1210.0f, 39.0f), FVector(0.55f, 0.24f, 0.035f), WetStoneColor, FRotator(0.0f, -31.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitBrokenSlab_2"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1420.0f, -840.0f, 39.0f), FVector(0.52f, 0.20f, 0.035f), WetStoneColor, FRotator(0.0f, 43.0f, 0.0f));

	SpawnDecorativeMesh(TEXT("DungeonLevel5AbandonedCell_Front"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front"), FVector(-1560.0f, -620.0f, 132.0f), FVector(1.35f), IronColor, FRotator(0.0f, 90.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5AbandonedCell_Side"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Side.SM_Cell_Side"), FVector(-1560.0f, -1040.0f, 132.0f), FVector(1.35f), IronColor, FRotator(0.0f, 90.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5RustDoor"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door"), FVector(-1528.0f, -810.0f, 132.0f), FVector(1.25f), RustColor, FRotator(0.0f, 78.0f, 0.0f), true);

	SpawnDecorativeMesh(TEXT("DungeonLevel5HangingChain_0"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"), FVector(-980.0f, 1480.0f, 240.0f), FVector(1.1f), IronColor, FRotator(0.0f, 180.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5HangingChain_1"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"), FVector(980.0f, 1480.0f, 240.0f), FVector(1.1f), IronColor, FRotator(0.0f, 180.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerCage"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Open.SM_Gibbet_Cage_Open"), FVector(1320.0f, -920.0f, 128.0f), FVector(0.95f), IronColor, FRotator(0.0f, -30.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5JailerBallChain"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Ball_And_Chain.SM_Ball_And_Chain"), FVector(930.0f, -760.0f, 32.0f), FVector(0.95f), IronColor, FRotator(0.0f, 22.0f, 0.0f), false);

	const TArray<FVector> WaterPoolLocations = {
		FVector(-760.0f, 260.0f, 22.0f),
		FVector(360.0f, -680.0f, 22.0f),
		FVector(1320.0f, 720.0f, 22.0f)
	};
	for (int32 Index = 0; Index < WaterPoolLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5WaterPool_%d"), Index), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), WaterPoolLocations[Index], FVector(0.85f + Index * 0.16f, 0.42f + Index * 0.08f, 0.018f), WaterColor, FRotator(90.0f, Index * 34.0f, 0.0f));
	}

	const TArray<FVector> RootLocations = {
		FVector(-1220.0f, 1380.0f, 230.0f),
		FVector(-980.0f, 1240.0f, 212.0f),
		FVector(1180.0f, -1480.0f, 210.0f),
		FVector(1360.0f, -1320.0f, 195.0f)
	};
	for (int32 Index = 0; Index < RootLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(FString::Printf(TEXT("DungeonLevel5CeilingRoot_%d"), Index), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), RootLocations[Index], FVector(0.045f, 0.045f, 1.15f), RootColor, FRotator(54.0f + Index * 8.0f, Index * 28.0f, 0.0f));
	}

	SpawnDecorativeMesh(TEXT("DungeonLevel5SurfaceLightCrack"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-1180.0f, 1260.0f, 246.0f), FVector(0.12f, 1.7f, 0.045f), SurfaceLightColor, FRotator(0.0f, -42.0f, 0.0f));
	SpawnPointLight(TEXT("DungeonLevel5SurfaceCrackLight"), FVector(-1180.0f, 1260.0f, 210.0f), SurfaceLightColor, 2100.0f, 520.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_COMPOSITION_READY: dark broken slabs frame the lower stair completion view."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_BROKEN_THRESHOLD_READY: abandoned cells, rusty doors, hanging chains, water pools, roots and darker lower descent dressing spawned."));
}

void ABroodGameMode::BuildDungeonLevel5ExitSeal()
{
	const FLinearColor SealStoneColor(0.05f, 0.08f, 0.07f);
	const FLinearColor SealGlowColor(0.02f, 0.55f, 0.22f);

	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitSealBase"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(1120.0f, -1210.0f, 18.0f), FVector(1.45f, 1.45f, 0.08f), SealStoneColor, FRotator::ZeroRotator);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitSealRuneA"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1120.0f, -1210.0f, 54.0f), FVector(1.12f, 0.06f, 0.045f), SealGlowColor, FRotator(0.0f, 45.0f, 0.0f));
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitSealRuneB"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(1120.0f, -1210.0f, 56.0f), FVector(1.12f, 0.06f, 0.045f), SealGlowColor, FRotator(0.0f, -45.0f, 0.0f));
	SpawnPointLight(TEXT("DungeonLevel5ExitSealLockedLight"), FVector(1120.0f, -1210.0f, 135.0f), SealGlowColor, 700.0f, 360.0f);
	SpawnLevelText(TEXT("DungeonLevel5ExitSealText"), TEXT("SEALED DESCENT"), FVector(1120.0f, -1210.0f, 155.0f), FRotator(0.0f, 135.0f, 0.0f), FColor(80, 210, 130), 26.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_SEALED: sealed descent marker spawned near the lower stair."));
}

void ABroodGameMode::BuildVillageCryptDressing()
{
	const FLinearColor WoodColor(0.34f, 0.20f, 0.10f);
	const FLinearColor ClothColor(0.42f, 0.04f, 0.04f);
	const FLinearColor IronColor(0.16f, 0.16f, 0.18f);
	const FLinearColor TorchColor(1.0f, 0.32f, 0.06f);

	const TArray<FVector> BarricadeLocations = {
		FVector(-360.0f, -1575.0f, 75.0f),
		FVector(360.0f, -1575.0f, 75.0f),
		FVector(-1480.0f, 0.0f, 75.0f),
		FVector(1480.0f, 0.0f, 75.0f)
	};
	const TArray<FRotator> BarricadeRotations = {
		FRotator(0.0f, 8.0f, 0.0f),
		FRotator(0.0f, -8.0f, 0.0f),
		FRotator(0.0f, 92.0f, 0.0f),
		FRotator(0.0f, -92.0f, 0.0f)
	};
	for (int32 Index = 0; Index < BarricadeLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("VillageBarricade_%d"), Index),
			TEXT("/Engine/BasicShapes/Cube.Cube"),
			BarricadeLocations[Index],
			FVector(1.85f, 0.12f, 0.18f),
			WoodColor,
			BarricadeRotations[Index]);
		SpawnDecorativeMesh(
			FString::Printf(TEXT("VillageBarricadeCrossbeam_%d"), Index),
			TEXT("/Engine/BasicShapes/Cube.Cube"),
			BarricadeLocations[Index] + FVector(0.0f, 0.0f, 42.0f),
			FVector(1.35f, 0.10f, 0.10f),
			WoodColor,
			BarricadeRotations[Index] + FRotator(0.0f, 0.0f, Index % 2 == 0 ? 18.0f : -18.0f));
	}

	const TArray<FVector> CrateLocations = {
		FVector(-1160.0f, -1120.0f, 42.0f),
		FVector(-1030.0f, -1010.0f, 42.0f),
		FVector(1140.0f, -1120.0f, 42.0f),
		FVector(1080.0f, 1110.0f, 42.0f)
	};
	for (int32 Index = 0; Index < CrateLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("VillageSupplyCrate_%d"), Index),
			TEXT("/Engine/BasicShapes/Cube.Cube"),
			CrateLocations[Index],
			FVector(0.42f, 0.42f, 0.36f),
			WoodColor,
			FRotator(0.0f, Index * 23.0f, 0.0f));
	}

	const TArray<FVector> BannerLocations = {
		FVector(-520.0f, 1685.0f, 210.0f),
		FVector(520.0f, 1685.0f, 210.0f)
	};
	for (int32 Index = 0; Index < BannerLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("VillageBanner_%d"), Index),
			TEXT("/Engine/BasicShapes/Cube.Cube"),
			BannerLocations[Index],
			FVector(0.18f, 0.03f, 1.0f),
			ClothColor,
			FRotator(0.0f, 180.0f, 0.0f));
	}

	const TArray<FVector> GearLocations = {
		FVector(-820.0f, -1320.0f, 24.0f),
		FVector(820.0f, -1320.0f, 24.0f),
		FVector(1280.0f, 760.0f, 24.0f)
	};
	for (int32 Index = 0; Index < GearLocations.Num(); ++Index)
	{
		SpawnDecorativeMesh(
			FString::Printf(TEXT("VillageMilitiaGear_%d"), Index),
			TEXT("/Engine/BasicShapes/Cylinder.Cylinder"),
			GearLocations[Index],
			FVector(0.22f, 0.22f, 0.06f),
			IronColor,
			FRotator(90.0f, Index * 41.0f, 0.0f));
	}

	SpawnPointLight(TEXT("VillageTorchLight_West"), FVector(-1560.0f, -220.0f, 190.0f), TorchColor, 1300.0f, 460.0f);
	SpawnPointLight(TEXT("VillageTorchLight_East"), FVector(1560.0f, 220.0f, 190.0f), TorchColor, 1300.0f, 460.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_VILLAGE_CRYPT_DRESSING_READY: barricades, supply crates, banners and militia gear spawned."));
}

void ABroodGameMode::SpawnStaticCube(const FString& Name, const FVector& Location, const FVector& Scale, const FLinearColor& Color, bool bVisible)
{
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!CubeMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load engine cube mesh for arena geometry."));
		return;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
	if (!MeshActor)
	{
		return;
	}

	MeshActor->Rename(*Name);
	MeshActor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
	MeshActor->SetActorScale3D(Scale);
	MeshActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshActor->GetStaticMeshComponent()->SetVisibility(bVisible);
	MeshActor->GetStaticMeshComponent()->SetHiddenInGame(!bVisible);
	if (bVisible)
	{
		ApplyPlaceholderColor(MeshActor->GetStaticMeshComponent(), Color);
	}
}

void ABroodGameMode::SpawnRouteCollisionBlock(const FString& Name, const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!CubeMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load engine cube mesh for Dungeon Level 5 route collision."));
		return;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(Location, Rotation);
	if (!MeshActor)
	{
		return;
	}

	MeshActor->Rename(*Name);
	UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent();
	MeshComponent->SetStaticMesh(CubeMesh);
	MeshActor->SetActorScale3D(Scale);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetVisibility(false);
	MeshComponent->SetHiddenInGame(true);
}

void ABroodGameMode::SpawnDecorativeMesh(const FString& Name, const TCHAR* MeshPath, const FVector& Location, const FVector& Scale, const FLinearColor& Color, const FRotator& Rotation, bool bApplyTint)
{
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, MeshPath);
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load decorative engine mesh %s."), MeshPath);
		return;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(Location, Rotation);
	if (!MeshActor)
	{
		return;
	}

	MeshActor->Rename(*Name);
	MeshActor->Tags.Add(FName(*Name));
	MeshActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
	MeshActor->SetActorScale3D(Scale);
	MeshActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (bApplyTint)
	{
		ApplyPlaceholderColor(MeshActor->GetStaticMeshComponent(), Color);
	}
}

void ABroodGameMode::SpawnPointLight(const FString& Name, const FVector& Location, const FLinearColor& Color, float Intensity, float Radius)
{
	APointLight* LightActor = GetWorld()->SpawnActor<APointLight>(Location, FRotator::ZeroRotator);
	if (!LightActor || !LightActor->PointLightComponent)
	{
		return;
	}

	LightActor->Rename(*Name);
	LightActor->PointLightComponent->SetLightColor(Color);
	LightActor->PointLightComponent->SetIntensity(Intensity);
	LightActor->PointLightComponent->SetAttenuationRadius(Radius);
	LightActor->PointLightComponent->SetCastShadows(true);
}

void ABroodGameMode::SpawnLevelText(const FString& Name, const FString& Text, const FVector& Location, const FRotator& Rotation, const FColor& Color, float WorldSize)
{
	ATextRenderActor* TextActor = GetWorld()->SpawnActor<ATextRenderActor>(Location, Rotation);
	if (!TextActor || !TextActor->GetTextRender())
	{
		return;
	}

	TextActor->Rename(*Name);
	TextActor->GetTextRender()->SetText(FText::FromString(Text));
	TextActor->GetTextRender()->SetTextRenderColor(Color);
	TextActor->GetTextRender()->SetHorizontalAlignment(EHTA_Center);
	TextActor->GetTextRender()->SetWorldSize(WorldSize);
	TextActor->GetTextRender()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABroodGameMode::CompleteDungeonLevel5()
{
	if (bDungeonLevel5Complete)
	{
		return;
	}

	bDungeonLevel5Complete = true;
	bRunComplete = true;

	const FLinearColor PortalGlowColor(0.00f, 0.78f, 0.34f);
	const FLinearColor PortalCoreColor(0.04f, 0.18f, 0.13f);
	const FLinearColor OpenStoneColor(0.025f, 0.045f, 0.044f);
	const FLinearColor KeyRuneColor(0.62f, 0.52f, 0.26f);
	const FVector ExitLocation(1120.0f, -1210.0f, 82.0f);

	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitOpenedShadowWell"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), ExitLocation + FVector(0.0f, 0.0f, -16.0f), FVector(1.42f, 1.42f, 0.035f), OpenStoneColor, FRotator::ZeroRotator);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitUnlockedDepthRim"), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones"), ExitLocation + FVector(0.0f, 0.0f, 32.0f), FVector(0.78f), PortalCoreColor, FRotator(0.0f, -42.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitUnlockedOpenLock"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Open.SM_Lock_Open"), ExitLocation + FVector(90.0f, 36.0f, 58.0f), FVector(0.62f), KeyRuneColor, FRotator(0.0f, 18.0f, 0.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitUnlockedFallenKey"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Key.SM_Lock_Key"), ExitLocation + FVector(-92.0f, -72.0f, 52.0f), FVector(0.54f), KeyRuneColor, FRotator(0.0f, 52.0f, 86.0f), true);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitUnlockedBrokenChainA"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"), ExitLocation + FVector(-135.0f, -108.0f, 78.0f), FVector(0.50f), OpenStoneColor, FRotator(0.0f, 28.0f, 0.0f), false);
	SpawnDecorativeMesh(TEXT("DungeonLevel5ExitUnlockedBrokenChainB"), TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"), ExitLocation + FVector(148.0f, 54.0f, 80.0f), FVector(0.50f), OpenStoneColor, FRotator(0.0f, -36.0f, 0.0f), false);
	SpawnPointLight(TEXT("DungeonLevel5ExitUnlockedLight"), ExitLocation + FVector(0.0f, 0.0f, 108.0f), PortalGlowColor, 2600.0f, 660.0f);

	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED: descent to Dungeon Level 4 opened."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED_ASSET_FEEDBACK_READY: completion feedback uses MedievalDungeon rim, open lock, fallen key and broken chain assets instead of primitive portal cones."));
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_COMPLETE: Jailer Husk defeated and Level 5 completion feedback spawned."));
	if (GEngine)
	{
		GEngine->ClearOnScreenDebugMessages();
	}
}

void ABroodGameMode::StartNextEncounter()
{
	if (bRunComplete)
	{
		return;
	}

	EncounterIndex++;
	AliveEnemies.Reset();

	if (GEngine)
	{
		FString Message;
		if (EncounterIndex == 0)
		{
			Message = TEXT("Dungeon Level 5 - La Soglia Spezzata: broken skeletons stir");
		}
		else if (EncounterIndex == 1)
		{
			Message = TEXT("Dungeon Level 5 - La Soglia Spezzata: rats, slimes and broken bones gather");
		}
		else if (EncounterIndex == 2)
		{
			Message = TEXT("Dungeon Level 5 - La Soglia Spezzata: animated remains rise");
		}
		else
		{
			Message = TEXT("Dungeon Level 5 - La Soglia Spezzata: Jailer Husk guards the lower stair");
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, Message);
	}

	if (EncounterIndex == 0)
	{
		SpawnEnemy(EBroodEnemyType::MeleeGrub, FVector(540.0f, -80.0f, 120.0f));
		SpawnEnemy(EBroodEnemyType::MeleeGrub, FVector(680.0f, 210.0f, 120.0f));
	}
	else if (EncounterIndex == 1)
	{
		SpawnEnemy(EBroodEnemyType::MeleeGrub, FVector(700.0f, -260.0f, 120.0f));
		SpawnEnemy(EBroodEnemyType::Spitter, FVector(950.0f, 220.0f, 120.0f));
		SpawnEnemy(EBroodEnemyType::Spitter, FVector(950.0f, -220.0f, 120.0f));
	}
	else if (EncounterIndex == 2)
	{
		SpawnEnemy(EBroodEnemyType::Brute, FVector(900.0f, 0.0f, 120.0f));
		SpawnEnemy(EBroodEnemyType::MeleeGrub, FVector(700.0f, 340.0f, 120.0f));
		SpawnEnemy(EBroodEnemyType::MeleeGrub, FVector(700.0f, -340.0f, 120.0f));
	}
	else
	{
		// On the editor-authored map the Jailer Husk guards its own Arena del Carceriere
		// (see .agents/unreal_scripts/build_level5_editor_map.py ArenaCarceriere center).
		// The runtime blockout keeps the original central spawn.
		const FVector JailerSpawn = IsEditorAuthoredDungeonLevel5Map()
			? FVector(3900.0f, 0.0f, 120.0f)
			: FVector(1000.0f, 0.0f, 120.0f);
		SpawnEnemy(EBroodEnemyType::BossBrute, JailerSpawn);
	}

	PrintHud();
}

void ABroodGameMode::SpawnEnemy(EBroodEnemyType EnemyType, const FVector& Location)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ABroodEnemyCharacter* Enemy = GetWorld()->SpawnActor<ABroodEnemyCharacter>(Location, FRotator::ZeroRotator, SpawnParams);
	if (Enemy)
	{
		Enemy->InitializeEnemy(EnemyType);
		AliveEnemies.Add(Enemy);
	}
}

void ABroodGameMode::SpawnBiomassPickup(const FVector& Location, float Amount)
{
	const FTransform SpawnTransform(FRotator::ZeroRotator, Location);
	ABiomassPickup* BiomassPickup = GetWorld()->SpawnActorDeferred<ABiomassPickup>(
		ABiomassPickup::StaticClass(),
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (BiomassPickup)
	{
		BiomassPickup->InitializeBiomass(Amount);
		BiomassPickup->FinishSpawning(SpawnTransform);
		UE_LOG(LogTemp, Display, TEXT("BROOD_BIOMASS_DROPPED: %.1f biomass at %s."), Amount, *Location.ToCompactString());
	}
}

void ABroodGameMode::OfferEvolutionChoices()
{
	bAwaitingEvolutionChoice = true;
	CurrentChoices.Reset();

	const TArray<FBroodEvolutionChoice> Catalog = BuildEvolutionCatalog();
	const int32 StartIndex = (EncounterIndex * 3) % Catalog.Num();
	for (int32 Index = 0; Index < 3; ++Index)
	{
		CurrentChoices.Add(Catalog[(StartIndex + Index) % Catalog.Num()]);
	}

	if (GEngine)
	{
		FString Message = TEXT("Choose evolution:\n");
		for (int32 Index = 0; Index < CurrentChoices.Num(); ++Index)
		{
			Message += FString::Printf(TEXT("%d: %s - %s\n"), Index + 1, *CurrentChoices[Index].DisplayName.ToString(), *CurrentChoices[Index].Description.ToString());
		}

		GEngine->AddOnScreenDebugMessage(2001, 20.0f, FColor::Cyan, Message);
	}
}

void ABroodGameMode::PrintHud() const
{
	if (!GEngine || !PlayerCharacter)
	{
		return;
	}

	const FString Message = FString::Printf(TEXT("Wave %d/4 | Enemies %d | HP %.0f | STA %.0f | Biomass %.0f"),
		FMath::Clamp(EncounterIndex + 1, 1, 4),
		AliveEnemies.Num(),
		PlayerCharacter->GetHealth(),
		PlayerCharacter->GetStamina(),
		PlayerCharacter->GetBiomass());
	GEngine->AddOnScreenDebugMessage(2000, 2.0f, FColor::White, Message);
}

TArray<FBroodEvolutionChoice> ABroodGameMode::BuildEvolutionCatalog() const
{
	TArray<FBroodEvolutionChoice> Catalog;

	auto AddChoice = [&Catalog](FName Id, const TCHAR* Name, const TCHAR* Description)
	{
		FBroodEvolutionChoice Choice;
		Choice.EvolutionId = Id;
		Choice.DisplayName = FText::FromString(FString(Name));
		Choice.Description = FText::FromString(FString(Description));
		Catalog.Add(Choice);
	};

	AddChoice(TEXT("HardenedCarapace"), TEXT("Hardened Carapace"), TEXT("+Armor, slight -MovementSpeed"));
	AddChoice(TEXT("RendingClaws"), TEXT("Rending Claws"), TEXT("+AttackPower"));
	AddChoice(TEXT("NeuralOverdrive"), TEXT("Neural Overdrive"), TEXT("+MovementSpeed, +MaxStamina"));
	AddChoice(TEXT("DigestEssence"), TEXT("Digest Essence"), TEXT("Heal after enemy kills"));
	AddChoice(TEXT("AcidBlood"), TEXT("Acid Blood"), TEXT("Damage nearby enemies when hit"));
	AddChoice(TEXT("BroodInstinct"), TEXT("Brood Instinct"), TEXT("Gain extra Biomass per kill"));
	AddChoice(TEXT("VenomStrike"), TEXT("Venom Strike"), TEXT("Basic attacks deal bonus venom damage"));
	AddChoice(TEXT("PredatorLeap"), TEXT("Predator Leap"), TEXT("Improve dodge distance and cooldown"));
	AddChoice(TEXT("ThickHide"), TEXT("Thick Hide"), TEXT("+MaxHealth"));

	return Catalog;
}
