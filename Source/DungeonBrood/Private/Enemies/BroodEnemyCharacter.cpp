#include "Enemies/BroodEnemyCharacter.h"

#include "AIController.h"
#include "Animation/AnimationAsset.h"
#include "Characters/BroodCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Game/BroodGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

namespace
{
	UStaticMesh* LoadEngineMesh(const TCHAR* MeshPath)
	{
		return LoadObject<UStaticMesh>(nullptr, MeshPath);
	}

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

	bool ConfigureSkeletalVisual(USkeletalMeshComponent* MeshComponent, const TCHAR* MeshPath, const TCHAR* IdleAnimationPath, const FVector& RelativeLocation, const FRotator& RelativeRotation, const FVector& RelativeScale)
	{
		if (!MeshComponent)
		{
			return false;
		}

		USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, MeshPath);
		if (!SkeletalMesh)
		{
			return false;
		}

		MeshComponent->SetSkeletalMesh(SkeletalMesh);
		MeshComponent->SetRelativeLocation(RelativeLocation);
		MeshComponent->SetRelativeRotation(RelativeRotation);
		MeshComponent->SetRelativeScale3D(RelativeScale);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (UAnimationAsset* IdleAnimation = LoadObject<UAnimationAsset>(nullptr, IdleAnimationPath))
		{
			MeshComponent->PlayAnimation(IdleAnimation, true);
		}

		return true;
	}

	void PlayImportedAnimation(USkeletalMeshComponent* MeshComponent, const TCHAR* AnimationPath, bool bLooping)
	{
		if (!MeshComponent || !MeshComponent->GetSkeletalMeshAsset())
		{
			return;
		}

		if (UAnimationAsset* Animation = LoadObject<UAnimationAsset>(nullptr, AnimationPath))
		{
			MeshComponent->PlayAnimation(Animation, bLooping);
		}
	}
}

ABroodEnemyCharacter::ABroodEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	BodyVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyVisual"));
	BodyVisual->SetupAttachment(RootComponent);
	BodyVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	BodyVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MarkerVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerVisual"));
	MarkerVisual->SetupAttachment(RootComponent);
	MarkerVisual->SetRelativeLocation(FVector(45.0f, 0.0f, 4.0f));
	MarkerVisual->SetRelativeScale3D(FVector(0.28f, 0.1f, 0.12f));
	MarkerVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkerVisual->SetStaticMesh(LoadEngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube")));

	NameplateVisual = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameplateVisual"));
	NameplateVisual->SetupAttachment(RootComponent);
	NameplateVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 115.0f));
	NameplateVisual->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	NameplateVisual->SetHorizontalAlignment(EHTA_Center);
	NameplateVisual->SetWorldSize(26.0f);
	NameplateVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConfigureStats();
}

void ABroodEnemyCharacter::InitializeEnemy(EBroodEnemyType NewEnemyType)
{
	EnemyType = NewEnemyType;
	ConfigureStats();
}

void ABroodEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsDead)
	{
		return;
	}

	ABroodCharacter* Player = Cast<ABroodCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Player || Player->IsDead())
	{
		return;
	}

	const FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
	const float Distance = ToPlayer.Size2D();

	if (EnemyType == EBroodEnemyType::Spitter && Distance < AttackRange * 0.65f)
	{
		AddMovementInput(-ToPlayer.GetSafeNormal2D(), 0.8f, true);
	}
	else if (Distance > AttackRange * 0.85f)
	{
		AddMovementInput(ToPlayer.GetSafeNormal2D(), 1.0f, true);
	}

	TryAttackPlayer(DeltaSeconds);
}

float ABroodEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	ApplyPlaceholderColor(BodyVisual, FLinearColor::White);
	GetWorldTimerManager().SetTimer(DamageFeedbackTimerHandle, this, &ABroodEnemyCharacter::ResetDamageFeedback, 0.16f, false);
	UpdateNameplate();
	UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage. Health: %.1f"), *GetName(), DamageAmount, Health);

	if (Health <= 0.0f)
	{
		Die(DamageCauser);
	}

	return DamageAmount;
}

bool ABroodEnemyCharacter::IsDead() const
{
	return bIsDead;
}

float ABroodEnemyCharacter::GetBiomassReward() const
{
	return BiomassReward;
}

void ABroodEnemyCharacter::ConfigureStats()
{
	switch (EnemyType)
	{
	case EBroodEnemyType::MeleeGrub:
		MaxHealth = 28.0f;
		AttackDamage = 7.0f;
		AttackRange = 125.0f;
		AttackCooldown = 1.15f;
		BiomassReward = 8.0f;
		GetCharacterMovement()->MaxWalkSpeed = 360.0f;
		break;
	case EBroodEnemyType::Spitter:
		MaxHealth = 38.0f;
		AttackDamage = 10.0f;
		AttackRange = 260.0f;
		AttackCooldown = 1.8f;
		BiomassReward = 11.0f;
		GetCharacterMovement()->MaxWalkSpeed = 260.0f;
		break;
	case EBroodEnemyType::Brute:
		MaxHealth = 70.0f;
		AttackDamage = 16.0f;
		AttackRange = 145.0f;
		AttackCooldown = 1.6f;
		BiomassReward = 18.0f;
		GetCharacterMovement()->MaxWalkSpeed = 220.0f;
		break;
	case EBroodEnemyType::BossBrute:
		MaxHealth = 180.0f;
		AttackDamage = 22.0f;
		AttackRange = 165.0f;
		AttackCooldown = 1.35f;
		BiomassReward = 60.0f;
		GetCharacterMovement()->MaxWalkSpeed = 250.0f;
		break;
	default:
		break;
	}

	Health = MaxHealth;
	ConfigureVisuals();
}

void ABroodEnemyCharacter::ConfigureVisuals()
{
	if (!BodyVisual || !MarkerVisual || !NameplateVisual)
	{
		return;
	}

	switch (EnemyType)
	{
	case EBroodEnemyType::MeleeGrub:
		BodyVisual->SetStaticMesh(LoadEngineMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere")));
		BodyVisual->SetRelativeScale3D(FVector(0.48f, 0.38f, 0.55f));
		BodyColor = FLinearColor(0.95f, 0.12f, 0.08f);
		MarkerColor = FLinearColor(1.0f, 0.85f, 0.08f);
		ApplyPlaceholderColor(BodyVisual, BodyColor);
		ApplyPlaceholderColor(MarkerVisual, MarkerColor);
		NameplateVisual->SetTextRenderColor(FColor(255, 80, 60));
		break;
	case EBroodEnemyType::Spitter:
		BodyVisual->SetStaticMesh(LoadEngineMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder")));
		BodyVisual->SetRelativeScale3D(FVector(0.42f, 0.42f, 0.75f));
		BodyColor = FLinearColor(0.55f, 0.15f, 0.95f);
		MarkerColor = FLinearColor(0.1f, 0.95f, 0.9f);
		ApplyPlaceholderColor(BodyVisual, BodyColor);
		ApplyPlaceholderColor(MarkerVisual, MarkerColor);
		NameplateVisual->SetTextRenderColor(FColor(190, 90, 255));
		break;
	case EBroodEnemyType::Brute:
		BodyVisual->SetStaticMesh(LoadEngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube")));
		BodyVisual->SetRelativeScale3D(FVector(0.78f, 0.78f, 1.05f));
		BodyColor = FLinearColor(1.0f, 0.45f, 0.04f);
		MarkerColor = FLinearColor(1.0f, 0.95f, 0.12f);
		ApplyPlaceholderColor(BodyVisual, BodyColor);
		ApplyPlaceholderColor(MarkerVisual, MarkerColor);
		NameplateVisual->SetTextRenderColor(FColor(255, 150, 40));
		break;
	case EBroodEnemyType::BossBrute:
		BodyVisual->SetStaticMesh(LoadEngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube")));
		BodyVisual->SetRelativeScale3D(FVector(1.18f, 1.18f, 1.45f));
		BodyColor = FLinearColor(0.8f, 0.04f, 0.02f);
		MarkerColor = FLinearColor(1.0f, 0.1f, 0.02f);
		ApplyPlaceholderColor(BodyVisual, BodyColor);
		ApplyPlaceholderColor(MarkerVisual, MarkerColor);
		NameplateVisual->SetTextRenderColor(FColor(255, 30, 20));
		NameplateVisual->SetWorldSize(34.0f);
		break;
	default:
		break;
	}

	bool bUsingImportedMesh = false;
	switch (EnemyType)
	{
	case EBroodEnemyType::MeleeGrub:
		bUsingImportedMesh = ConfigureSkeletalVisual(
			GetMesh(),
			TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Melee_Dawn.Minion_Lane_Melee_Dawn"),
			TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Melee/NonCombat_Idle.NonCombat_Idle"),
			FVector(0.0f, 0.0f, -88.0f),
			FRotator(0.0f, -90.0f, 0.0f),
			FVector(0.62f));
		break;
	case EBroodEnemyType::Spitter:
		bUsingImportedMesh = ConfigureSkeletalVisual(
			GetMesh(),
			TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Ranged_Dawn.Minion_Lane_Ranged_Dawn"),
			TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Ranged/Idle_A.Idle_A"),
			FVector(0.0f, 0.0f, -88.0f),
			FRotator(0.0f, -90.0f, 0.0f),
			FVector(0.66f));
		break;
	case EBroodEnemyType::Brute:
		bUsingImportedMesh = ConfigureSkeletalVisual(
			GetMesh(),
			TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Siege_Dawn.Minion_Lane_Siege_Dawn"),
			TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Siege/Idle.Idle"),
			FVector(0.0f, 0.0f, -88.0f),
			FRotator(0.0f, -90.0f, 0.0f),
			FVector(0.82f));
		break;
	case EBroodEnemyType::BossBrute:
		bUsingImportedMesh = ConfigureSkeletalVisual(
			GetMesh(),
			TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/SK_Minion_Lane_Super.SK_Minion_Lane_Super"),
			TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Super/Idle.Idle"),
			FVector(0.0f, 0.0f, -88.0f),
			FRotator(0.0f, -90.0f, 0.0f),
			FVector(0.95f));
		break;
	default:
		break;
	}

	BodyVisual->SetVisibility(!bUsingImportedMesh);
	if (bUsingImportedMesh)
	{
		UE_LOG(LogTemp, Display, TEXT("BROOD_IMPORTED_ENEMY_ASSET_READY: %s mesh linked."), *GetName());
	}

	UpdateNameplate();
}

void ABroodEnemyCharacter::TryAttackPlayer(float DeltaSeconds)
{
	AttackTimer -= DeltaSeconds;
	if (AttackTimer > 0.0f)
	{
		return;
	}

	ABroodCharacter* Player = Cast<ABroodCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Player || Player->IsDead())
	{
		return;
	}

	const float Distance = FVector::Dist2D(GetActorLocation(), Player->GetActorLocation());
	if (Distance <= AttackRange)
	{
		AttackTimer = AttackCooldown;
		switch (EnemyType)
		{
		case EBroodEnemyType::MeleeGrub:
			PlayImportedAnimation(GetMesh(), TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Melee/Attack_A.Attack_A"), false);
			break;
		case EBroodEnemyType::Spitter:
			PlayImportedAnimation(GetMesh(), TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Ranged/Primary_Fire.Primary_Fire"), false);
			break;
		case EBroodEnemyType::Brute:
			PlayImportedAnimation(GetMesh(), TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Siege/MeleeAttack_A.MeleeAttack_A"), false);
			break;
		case EBroodEnemyType::BossBrute:
			PlayImportedAnimation(GetMesh(), TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Super/Attack_A.Attack_A"), false);
			break;
		default:
			break;
		}
		MarkerVisual->SetRelativeScale3D(FVector(0.58f, 0.2f, 0.2f));
		ApplyPlaceholderColor(MarkerVisual, FLinearColor(1.0f, 1.0f, 1.0f));
		GetWorldTimerManager().SetTimer(AttackFeedbackTimerHandle, this, &ABroodEnemyCharacter::ResetAttackFeedback, 0.2f, false);
		UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
		UE_LOG(LogTemp, Log, TEXT("%s attacked player for %.1f damage."), *GetName(), AttackDamage);
	}
}

void ABroodEnemyCharacter::ResetDamageFeedback()
{
	if (!bIsDead)
	{
		ApplyPlaceholderColor(BodyVisual, BodyColor);
	}
}

void ABroodEnemyCharacter::ResetAttackFeedback()
{
	MarkerVisual->SetRelativeScale3D(FVector(0.28f, 0.1f, 0.12f));
	ApplyPlaceholderColor(MarkerVisual, MarkerColor);
}

void ABroodEnemyCharacter::UpdateNameplate()
{
	const TCHAR* EnemyName = TEXT("GRUB");
	switch (EnemyType)
	{
	case EBroodEnemyType::Spitter:
		EnemyName = TEXT("SPITTER");
		break;
	case EBroodEnemyType::Brute:
		EnemyName = TEXT("BRUTE");
		break;
	case EBroodEnemyType::BossBrute:
		EnemyName = TEXT("BOSS");
		break;
	default:
		break;
	}

	NameplateVisual->SetText(FText::FromString(FString::Printf(TEXT("%s  %.0f/%.0f"), EnemyName, Health, MaxHealth)));
}

void ABroodEnemyCharacter::Die(AActor* DamageCauser)
{
	bIsDead = true;
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ApplyPlaceholderColor(BodyVisual, FLinearColor(0.05f, 0.05f, 0.05f));
	ApplyPlaceholderColor(MarkerVisual, FLinearColor(0.05f, 0.05f, 0.05f));
	NameplateVisual->SetText(FText::FromString(TEXT("DEFEATED")));
	NameplateVisual->SetTextRenderColor(FColor::Silver);
	BodyVisual->SetRelativeRotation(FRotator(0.0f, 0.0f, 70.0f));
	switch (EnemyType)
	{
	case EBroodEnemyType::MeleeGrub:
		PlayImportedAnimation(GetMesh(), TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Melee/Death_A.Death_A"), false);
		break;
	case EBroodEnemyType::Spitter:
		PlayImportedAnimation(GetMesh(), TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Ranged/Death_Front_A.Death_Front_A"), false);
		break;
	case EBroodEnemyType::Brute:
		PlayImportedAnimation(GetMesh(), TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Siege/Death_A.Death_A"), false);
		break;
	case EBroodEnemyType::BossBrute:
		PlayImportedAnimation(GetMesh(), TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Super/Death_Front.Death_Front"), false);
		break;
	default:
		break;
	}
	SetLifeSpan(1.0f);
	UE_LOG(LogTemp, Log, TEXT("%s died."), *GetName());

	if (ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr)
	{
		BroodGameMode->NotifyEnemyKilled(this);
	}
}
