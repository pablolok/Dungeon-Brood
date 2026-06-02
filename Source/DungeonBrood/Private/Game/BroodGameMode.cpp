#include "Game/BroodGameMode.h"

#include "Characters/BroodCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/PointLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Pickups/BiomassPickup.h"
#include "Player/BroodPlayerController.h"

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

	BuildTestArena();
	BuildDungeonDressing();
	EnsurePlayer();
	StartNextEncounter();

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
			bRunComplete = true;
			UE_LOG(LogTemp, Log, TEXT("Run Complete"));
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 12.0f, FColor::Green, TEXT("RUN COMPLETE"));
			}
			return;
		}

		OfferEvolutionChoices();
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
	SpawnStaticCube(TEXT("ArenaFloor"), FVector(0.0f, 0.0f, -55.0f), FVector(18.0f, 18.0f, 0.5f), FLinearColor(0.08f, 0.08f, 0.09f));
	SpawnStaticCube(TEXT("NorthWall"), FVector(0.0f, 1800.0f, 120.0f), FVector(18.0f, 0.4f, 2.8f), FLinearColor(0.18f, 0.18f, 0.22f));
	SpawnStaticCube(TEXT("SouthWall"), FVector(0.0f, -1800.0f, 120.0f), FVector(18.0f, 0.4f, 2.8f), FLinearColor(0.18f, 0.18f, 0.22f));
	SpawnStaticCube(TEXT("EastWall"), FVector(1800.0f, 0.0f, 120.0f), FVector(0.4f, 18.0f, 2.8f), FLinearColor(0.18f, 0.18f, 0.22f));
	SpawnStaticCube(TEXT("WestWall"), FVector(-1800.0f, 0.0f, 120.0f), FVector(0.4f, 18.0f, 2.8f), FLinearColor(0.18f, 0.18f, 0.22f));
	SpawnStaticCube(TEXT("PlayerStartMarker"), FVector(0.0f, 0.0f, -5.0f), FVector(1.2f, 1.2f, 0.06f), FLinearColor(0.0f, 0.8f, 0.2f));
	SpawnStaticCube(TEXT("EnemyLaneMarker"), FVector(760.0f, 0.0f, -4.0f), FVector(3.2f, 0.16f, 0.06f), FLinearColor(0.9f, 0.08f, 0.04f));
}

void ABroodGameMode::BuildDungeonDressing()
{
	const FLinearColor StoneColor(0.12f, 0.14f, 0.16f);
	const FLinearColor DarkStoneColor(0.06f, 0.07f, 0.08f);
	const FLinearColor MetalColor(0.20f, 0.16f, 0.10f);
	const FLinearColor EmberColor(1.0f, 0.18f, 0.03f);
	const FLinearColor BiomassColor(0.05f, 0.85f, 0.30f);

	for (int32 Index = -3; Index <= 3; ++Index)
	{
		const float Offset = Index * 420.0f;
		SpawnDecorativeMesh(FString::Printf(TEXT("FloorBand_%d"), Index), TEXT("/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor"), FVector(Offset, 0.0f, -24.0f), FVector(3.0f, 3.0f, 1.0f), StoneColor, FRotator::ZeroRotator, false);
	}

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

void ABroodGameMode::SpawnStaticCube(const FString& Name, const FVector& Location, const FVector& Scale, const FLinearColor& Color)
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
	ApplyPlaceholderColor(MeshActor->GetStaticMeshComponent(), Color);
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
		const FString Message = EncounterIndex >= 3
			? TEXT("Final encounter: Brood Brute")
			: FString::Printf(TEXT("Wave %d begins"), EncounterIndex + 1);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, Message);
	}

	if (EncounterIndex == 0)
	{
		SpawnEnemy(EBroodEnemyType::MeleeGrub, FVector(650.0f, 0.0f, 120.0f));
		SpawnEnemy(EBroodEnemyType::MeleeGrub, FVector(850.0f, 260.0f, 120.0f));
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
		SpawnEnemy(EBroodEnemyType::BossBrute, FVector(1000.0f, 0.0f, 120.0f));
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
