#include "Game/BroodGameMode.h"

#include "Characters/BroodCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ABroodGameMode::ABroodGameMode()
{
	DefaultPawnClass = ABroodCharacter::StaticClass();
}

void ABroodGameMode::BeginPlay()
{
	Super::BeginPlay();

	BuildTestArena();
	EnsurePlayer();
	StartNextEncounter();
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
	if (PlayerCharacter)
	{
		PlayerCharacter->OnEnemyKilled(Enemy->GetBiomassReward());
	}

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
	SpawnStaticCube(TEXT("ArenaFloor"), FVector(0.0f, 0.0f, -55.0f), FVector(18.0f, 18.0f, 0.5f));
	SpawnStaticCube(TEXT("NorthWall"), FVector(0.0f, 1800.0f, 120.0f), FVector(18.0f, 0.4f, 2.8f));
	SpawnStaticCube(TEXT("SouthWall"), FVector(0.0f, -1800.0f, 120.0f), FVector(18.0f, 0.4f, 2.8f));
	SpawnStaticCube(TEXT("EastWall"), FVector(1800.0f, 0.0f, 120.0f), FVector(0.4f, 18.0f, 2.8f));
	SpawnStaticCube(TEXT("WestWall"), FVector(-1800.0f, 0.0f, 120.0f), FVector(0.4f, 18.0f, 2.8f));
}

void ABroodGameMode::SpawnStaticCube(const FString& Name, const FVector& Location, const FVector& Scale)
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
