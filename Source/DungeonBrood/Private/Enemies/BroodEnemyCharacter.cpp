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

	bool ConfigureStaticVisual(UStaticMeshComponent* MeshComponent, const TCHAR* PrimaryMeshPath, const TCHAR* FallbackMeshPath)
	{
		if (!MeshComponent)
		{
			return false;
		}

		if (UStaticMesh* PrimaryMesh = LoadEngineMesh(PrimaryMeshPath))
		{
			MeshComponent->SetStaticMesh(PrimaryMesh);
			return true;
		}

		if (UStaticMesh* FallbackMesh = LoadEngineMesh(FallbackMeshPath))
		{
			MeshComponent->SetStaticMesh(FallbackMesh);
		}

		return false;
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

	BoneTorsoVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoneTorsoVisual"));
	BoneTorsoVisual->SetupAttachment(RootComponent);
	BoneTorsoVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoneTorsoVisual->SetVisibility(false);

	BoneHeadVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoneHeadVisual"));
	BoneHeadVisual->SetupAttachment(RootComponent);
	BoneHeadVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoneHeadVisual->SetVisibility(false);

	WeaponVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponVisual"));
	WeaponVisual->SetupAttachment(RootComponent);
	WeaponVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponVisual->SetVisibility(false);

	CreatureAccentVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CreatureAccentVisual"));
	CreatureAccentVisual->SetupAttachment(RootComponent);
	CreatureAccentVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CreatureAccentVisual->SetVisibility(false);

	CreatureSecondaryAccentVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CreatureSecondaryAccentVisual"));
	CreatureSecondaryAccentVisual->SetupAttachment(RootComponent);
	CreatureSecondaryAccentVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CreatureSecondaryAccentVisual->SetVisibility(false);

	JailerKeyRingVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerKeyRingVisual"));
	JailerKeyRingVisual->SetupAttachment(RootComponent);
	JailerKeyRingVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerKeyRingVisual->SetVisibility(false);

	JailerKeyTeethVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerKeyTeethVisual"));
	JailerKeyTeethVisual->SetupAttachment(RootComponent);
	JailerKeyTeethVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerKeyTeethVisual->SetVisibility(false);

	JailerChainVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerChainVisual"));
	JailerChainVisual->SetupAttachment(RootComponent);
	JailerChainVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerChainVisual->SetVisibility(false);

	JailerMantleVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerMantleVisual"));
	JailerMantleVisual->SetupAttachment(RootComponent);
	JailerMantleVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerMantleVisual->SetVisibility(false);

	JailerLeftArmVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerLeftArmVisual"));
	JailerLeftArmVisual->SetupAttachment(RootComponent);
	JailerLeftArmVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerLeftArmVisual->SetVisibility(false);

	JailerRightArmVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerRightArmVisual"));
	JailerRightArmVisual->SetupAttachment(RootComponent);
	JailerRightArmVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerRightArmVisual->SetVisibility(false);

	JailerManacleVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerManacleVisual"));
	JailerManacleVisual->SetupAttachment(RootComponent);
	JailerManacleVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerManacleVisual->SetVisibility(false);

	JailerHipsVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerHipsVisual"));
	JailerHipsVisual->SetupAttachment(RootComponent);
	JailerHipsVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerHipsVisual->SetVisibility(false);

	JailerRobeVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerRobeVisual"));
	JailerRobeVisual->SetupAttachment(RootComponent);
	JailerRobeVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerRobeVisual->SetVisibility(false);

	JailerSpineVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerSpineVisual"));
	JailerSpineVisual->SetupAttachment(RootComponent);
	JailerSpineVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerSpineVisual->SetVisibility(false);

	JailerLeftLegVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerLeftLegVisual"));
	JailerLeftLegVisual->SetupAttachment(RootComponent);
	JailerLeftLegVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerLeftLegVisual->SetVisibility(false);

	JailerRightLegVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerRightLegVisual"));
	JailerRightLegVisual->SetupAttachment(RootComponent);
	JailerRightLegVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerRightLegVisual->SetVisibility(false);

	JailerLeftHandKeyVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerLeftHandKeyVisual"));
	JailerLeftHandKeyVisual->SetupAttachment(RootComponent);
	JailerLeftHandKeyVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerLeftHandKeyVisual->SetVisibility(false);

	JailerRightHandKeyVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JailerRightHandKeyVisual"));
	JailerRightHandKeyVisual->SetupAttachment(RootComponent);
	JailerRightHandKeyVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	JailerRightHandKeyVisual->SetVisibility(false);

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
	AnimateDungeonLevel5Dressing(DeltaSeconds);

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
		BodyColor = FLinearColor(0.72f, 0.46f, 0.24f);
		MarkerColor = FLinearColor(1.0f, 0.68f, 0.22f);
		ApplyPlaceholderColor(BodyVisual, BodyColor);
		ApplyPlaceholderColor(MarkerVisual, MarkerColor);
		NameplateVisual->SetTextRenderColor(FColor(230, 170, 95));
		break;
	case EBroodEnemyType::Spitter:
		BodyVisual->SetStaticMesh(LoadEngineMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder")));
		BodyVisual->SetRelativeScale3D(FVector(0.42f, 0.42f, 0.75f));
		BodyColor = FLinearColor(0.30f, 0.33f, 0.40f);
		MarkerColor = FLinearColor(0.70f, 0.82f, 1.0f);
		ApplyPlaceholderColor(BodyVisual, BodyColor);
		ApplyPlaceholderColor(MarkerVisual, MarkerColor);
		NameplateVisual->SetTextRenderColor(FColor(150, 180, 255));
		break;
	case EBroodEnemyType::Brute:
		BodyVisual->SetStaticMesh(LoadEngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube")));
		BodyVisual->SetRelativeScale3D(FVector(0.78f, 0.78f, 1.05f));
		BodyColor = FLinearColor(0.42f, 0.38f, 0.30f);
		MarkerColor = FLinearColor(1.0f, 0.88f, 0.34f);
		ApplyPlaceholderColor(BodyVisual, BodyColor);
		ApplyPlaceholderColor(MarkerVisual, MarkerColor);
		NameplateVisual->SetTextRenderColor(FColor(230, 210, 130));
		break;
	case EBroodEnemyType::BossBrute:
		BodyVisual->SetStaticMesh(LoadEngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube")));
		BodyVisual->SetRelativeScale3D(FVector(1.18f, 1.18f, 1.45f));
		BodyColor = FLinearColor(0.55f, 0.08f, 0.05f);
		MarkerColor = FLinearColor(1.0f, 0.18f, 0.08f);
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

	ConfigureDungeonLevel5Dressing();
	CacheDungeonLevel5MotionPose();
	UpdateNameplate();
}

void ABroodEnemyCharacter::ConfigureDungeonLevel5Dressing()
{
	if (!BoneTorsoVisual || !BoneHeadVisual || !WeaponVisual)
	{
		return;
	}

	const FLinearColor BoneColor(0.82f, 0.76f, 0.62f);
	const FLinearColor OldBoneColor(0.58f, 0.52f, 0.42f);
	const FLinearColor GuardianColor(0.85f, 0.22f, 0.12f);
	const FLinearColor ArcherColor(0.55f, 0.70f, 1.0f);

	const bool bTorsoImported = ConfigureStaticVisual(
		BoneTorsoVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage"),
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	const bool bHeadImported = ConfigureStaticVisual(
		BoneHeadVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull"),
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	bool bWeaponImported = false;
	switch (EnemyType)
	{
	case EBroodEnemyType::Spitter:
		bWeaponImported = ConfigureStaticVisual(
			WeaponVisual,
			TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm.SM_Bones_Arm"),
			TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
		break;
	case EBroodEnemyType::Brute:
	case EBroodEnemyType::BossBrute:
		bWeaponImported = ConfigureStaticVisual(
			WeaponVisual,
			TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_FemurRight.SM_Bones_FemurRight"),
			TEXT("/Engine/BasicShapes/Cube.Cube"));
		break;
	default:
		bWeaponImported = ConfigureStaticVisual(
			WeaponVisual,
			TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm.SM_Bones_Arm"),
			TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
		break;
	}

	BoneTorsoVisual->SetVisibility(true);
	BoneTorsoVisual->SetHiddenInGame(false);
	BoneHeadVisual->SetVisibility(true);
	BoneHeadVisual->SetHiddenInGame(false);
	WeaponVisual->SetVisibility(true);
	WeaponVisual->SetHiddenInGame(false);

	switch (EnemyType)
	{
	case EBroodEnemyType::MeleeGrub:
		BoneTorsoVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 4.0f));
		BoneTorsoVisual->SetRelativeRotation(FRotator(8.0f, 20.0f, 0.0f));
		BoneTorsoVisual->SetRelativeScale3D(FVector(0.42f));
		BoneHeadVisual->SetRelativeLocation(FVector(38.0f, 0.0f, 38.0f));
		BoneHeadVisual->SetRelativeRotation(FRotator(0.0f, 35.0f, -8.0f));
		BoneHeadVisual->SetRelativeScale3D(FVector(0.34f));
		WeaponVisual->SetRelativeLocation(FVector(10.0f, -34.0f, 18.0f));
		WeaponVisual->SetRelativeRotation(FRotator(72.0f, 18.0f, 38.0f));
		WeaponVisual->SetRelativeScale3D(FVector(0.34f));
		ApplyPlaceholderColor(BoneTorsoVisual, BoneColor);
		ApplyPlaceholderColor(BoneHeadVisual, BoneColor);
		ApplyPlaceholderColor(WeaponVisual, OldBoneColor);
		break;
	case EBroodEnemyType::Spitter:
		BoneTorsoVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 14.0f));
		BoneTorsoVisual->SetRelativeRotation(FRotator(0.0f, -10.0f, 0.0f));
		BoneTorsoVisual->SetRelativeScale3D(FVector(0.46f));
		BoneHeadVisual->SetRelativeLocation(FVector(18.0f, 0.0f, 72.0f));
		BoneHeadVisual->SetRelativeRotation(FRotator(0.0f, 10.0f, 0.0f));
		BoneHeadVisual->SetRelativeScale3D(FVector(0.36f));
		WeaponVisual->SetRelativeLocation(FVector(54.0f, -30.0f, 46.0f));
		WeaponVisual->SetRelativeRotation(FRotator(82.0f, 0.0f, 96.0f));
		WeaponVisual->SetRelativeScale3D(FVector(0.44f));
		ApplyPlaceholderColor(BoneTorsoVisual, BoneColor);
		ApplyPlaceholderColor(BoneHeadVisual, BoneColor);
		ApplyPlaceholderColor(WeaponVisual, ArcherColor);
		break;
	case EBroodEnemyType::Brute:
		BoneTorsoVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 28.0f));
		BoneTorsoVisual->SetRelativeRotation(FRotator(-4.0f, 14.0f, 0.0f));
		BoneTorsoVisual->SetRelativeScale3D(FVector(0.68f));
		BoneHeadVisual->SetRelativeLocation(FVector(22.0f, 0.0f, 96.0f));
		BoneHeadVisual->SetRelativeRotation(FRotator(0.0f, -12.0f, 0.0f));
		BoneHeadVisual->SetRelativeScale3D(FVector(0.52f));
		WeaponVisual->SetRelativeLocation(FVector(58.0f, -40.0f, 42.0f));
		WeaponVisual->SetRelativeRotation(FRotator(55.0f, -20.0f, 118.0f));
		WeaponVisual->SetRelativeScale3D(FVector(0.62f));
		ApplyPlaceholderColor(BoneTorsoVisual, OldBoneColor);
		ApplyPlaceholderColor(BoneHeadVisual, BoneColor);
		ApplyPlaceholderColor(WeaponVisual, OldBoneColor);
		break;
	case EBroodEnemyType::BossBrute:
		BoneTorsoVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 66.0f));
		BoneTorsoVisual->SetRelativeRotation(FRotator(-6.0f, 0.0f, 0.0f));
		BoneTorsoVisual->SetRelativeScale3D(FVector(0.86f));
		BoneHeadVisual->SetRelativeLocation(FVector(26.0f, 0.0f, 104.0f));
		BoneHeadVisual->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		BoneHeadVisual->SetRelativeScale3D(FVector(0.68f));
		WeaponVisual->SetRelativeLocation(FVector(70.0f, -52.0f, 54.0f));
		WeaponVisual->SetRelativeRotation(FRotator(48.0f, -28.0f, 126.0f));
		WeaponVisual->SetRelativeScale3D(FVector(0.82f));
		ApplyPlaceholderColor(BoneTorsoVisual, GuardianColor);
		ApplyPlaceholderColor(BoneHeadVisual, BoneColor);
		ApplyPlaceholderColor(WeaponVisual, GuardianColor);
		break;
	default:
		break;
	}

	bDungeonLevel5DressingReady = bTorsoImported && bHeadImported && bWeaponImported;
	UE_LOG(
		LogTemp,
		Display,
		TEXT("%s: %s bone torso, skull and weapon dressing configured for %s."),
		bDungeonLevel5DressingReady ? TEXT("BROOD_DUNGEON_LEVEL5_ENEMY_DRESSING_READY") : TEXT("BROOD_DUNGEON_LEVEL5_ENEMY_DRESSING_FALLBACK_READY"),
		*GetName(),
		*StaticEnum<EBroodEnemyType>()->GetNameStringByValue(static_cast<int64>(EnemyType)));

	ConfigureJailerHuskDressing();
	ConfigureDungeonLevel5CreatureVariety();
}

void ABroodEnemyCharacter::ConfigureDungeonLevel5CreatureVariety()
{
	if (!CreatureAccentVisual || !CreatureSecondaryAccentVisual)
	{
		return;
	}

	bDungeonLevel5CreatureVarietyReady = false;

	const bool bAccentMeshReady = ConfigureStaticVisual(
		CreatureAccentVisual,
		TEXT("/Engine/BasicShapes/Sphere.Sphere"),
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	const bool bSecondaryMeshReady = ConfigureStaticVisual(
		CreatureSecondaryAccentVisual,
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	CreatureAccentVisual->SetVisibility(true);
	CreatureAccentVisual->SetHiddenInGame(false);
	CreatureSecondaryAccentVisual->SetVisibility(true);
	CreatureSecondaryAccentVisual->SetHiddenInGame(false);

	switch (EnemyType)
	{
	case EBroodEnemyType::MeleeGrub:
		MarkerVisual->SetRelativeLocation(FVector(24.0f, 0.0f, 46.0f));
		MarkerVisual->SetRelativeScale3D(FVector(0.42f, 0.16f, 0.18f));
		ApplyPlaceholderColor(MarkerVisual, FLinearColor(0.92f, 0.58f, 0.28f));
		CreatureAccentVisual->SetRelativeLocation(FVector(-38.0f, 0.0f, 22.0f));
		CreatureAccentVisual->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		CreatureAccentVisual->SetRelativeScale3D(FVector(0.34f, 0.18f, 0.13f));
		CreatureSecondaryAccentVisual->SetRelativeLocation(FVector(-82.0f, 0.0f, 16.0f));
		CreatureSecondaryAccentVisual->SetRelativeRotation(FRotator(0.0f, 90.0f, 92.0f));
		CreatureSecondaryAccentVisual->SetRelativeScale3D(FVector(0.08f, 0.08f, 0.68f));
		ApplyPlaceholderColor(CreatureAccentVisual, FLinearColor(0.62f, 0.40f, 0.25f));
		ApplyPlaceholderColor(CreatureSecondaryAccentVisual, FLinearColor(0.38f, 0.24f, 0.18f));
		break;
	case EBroodEnemyType::Spitter:
		MarkerVisual->SetRelativeLocation(FVector(28.0f, 0.0f, 58.0f));
		MarkerVisual->SetRelativeScale3D(FVector(0.5f, 0.18f, 0.22f));
		ApplyPlaceholderColor(MarkerVisual, FLinearColor(0.18f, 1.0f, 0.42f));
		CreatureAccentVisual->SetRelativeLocation(FVector(28.0f, 0.0f, 52.0f));
		CreatureAccentVisual->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		CreatureAccentVisual->SetRelativeScale3D(FVector(0.42f, 0.36f, 0.30f));
		CreatureSecondaryAccentVisual->SetRelativeLocation(FVector(58.0f, 0.0f, 42.0f));
		CreatureSecondaryAccentVisual->SetRelativeRotation(FRotator(88.0f, 0.0f, 0.0f));
		CreatureSecondaryAccentVisual->SetRelativeScale3D(FVector(0.11f, 0.11f, 0.42f));
		ApplyPlaceholderColor(CreatureAccentVisual, FLinearColor(0.10f, 1.0f, 0.36f));
		ApplyPlaceholderColor(CreatureSecondaryAccentVisual, FLinearColor(0.06f, 0.60f, 0.22f));
		break;
	case EBroodEnemyType::Brute:
		MarkerVisual->SetRelativeLocation(FVector(30.0f, 0.0f, 84.0f));
		MarkerVisual->SetRelativeScale3D(FVector(0.58f, 0.2f, 0.24f));
		ApplyPlaceholderColor(MarkerVisual, FLinearColor(0.88f, 0.75f, 0.42f));
		CreatureAccentVisual->SetRelativeLocation(FVector(-22.0f, -66.0f, 84.0f));
		CreatureAccentVisual->SetRelativeRotation(FRotator(0.0f, 0.0f, -24.0f));
		CreatureAccentVisual->SetRelativeScale3D(FVector(0.18f, 0.62f, 0.12f));
		CreatureSecondaryAccentVisual->SetRelativeLocation(FVector(-22.0f, 66.0f, 84.0f));
		CreatureSecondaryAccentVisual->SetRelativeRotation(FRotator(0.0f, 0.0f, 24.0f));
		CreatureSecondaryAccentVisual->SetRelativeScale3D(FVector(0.18f, 0.62f, 0.12f));
		ApplyPlaceholderColor(CreatureAccentVisual, FLinearColor(0.26f, 0.22f, 0.20f));
		ApplyPlaceholderColor(CreatureSecondaryAccentVisual, FLinearColor(0.26f, 0.22f, 0.20f));
		break;
	case EBroodEnemyType::BossBrute:
		CreatureAccentVisual->SetVisibility(false);
		CreatureAccentVisual->SetHiddenInGame(true);
		CreatureSecondaryAccentVisual->SetVisibility(false);
		CreatureSecondaryAccentVisual->SetHiddenInGame(true);
		break;
	default:
		break;
	}

	bDungeonLevel5CreatureVarietyReady = bAccentMeshReady && bSecondaryMeshReady;
	UE_LOG(
		LogTemp,
		Display,
		TEXT("%s: %s Level 5 creature accent dressing configured."),
		bDungeonLevel5CreatureVarietyReady ? TEXT("BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_READY") : TEXT("BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_FALLBACK_READY"),
		*GetName());
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ENEMY_SILHOUETTE_READY: %s high-contrast Level 5 enemy silhouette accents configured."), *GetName());
}

void ABroodEnemyCharacter::ConfigureJailerHuskDressing()
{
	if (EnemyType != EBroodEnemyType::BossBrute)
	{
		if (JailerKeyRingVisual)
		{
			JailerKeyRingVisual->SetVisibility(false);
			JailerKeyRingVisual->SetHiddenInGame(true);
		}
		if (JailerKeyTeethVisual)
		{
			JailerKeyTeethVisual->SetVisibility(false);
			JailerKeyTeethVisual->SetHiddenInGame(true);
		}
		if (JailerChainVisual)
		{
			JailerChainVisual->SetVisibility(false);
			JailerChainVisual->SetHiddenInGame(true);
		}
		if (JailerMantleVisual)
		{
			JailerMantleVisual->SetVisibility(false);
			JailerMantleVisual->SetHiddenInGame(true);
		}
		if (JailerLeftArmVisual)
		{
			JailerLeftArmVisual->SetVisibility(false);
			JailerLeftArmVisual->SetHiddenInGame(true);
		}
		if (JailerRightArmVisual)
		{
			JailerRightArmVisual->SetVisibility(false);
			JailerRightArmVisual->SetHiddenInGame(true);
		}
		if (JailerManacleVisual)
		{
			JailerManacleVisual->SetVisibility(false);
			JailerManacleVisual->SetHiddenInGame(true);
		}
		if (JailerHipsVisual)
		{
			JailerHipsVisual->SetVisibility(false);
			JailerHipsVisual->SetHiddenInGame(true);
		}
		if (JailerRobeVisual)
		{
			JailerRobeVisual->SetVisibility(false);
			JailerRobeVisual->SetHiddenInGame(true);
		}
		if (JailerSpineVisual)
		{
			JailerSpineVisual->SetVisibility(false);
			JailerSpineVisual->SetHiddenInGame(true);
		}
		if (JailerLeftLegVisual)
		{
			JailerLeftLegVisual->SetVisibility(false);
			JailerLeftLegVisual->SetHiddenInGame(true);
		}
		if (JailerRightLegVisual)
		{
			JailerRightLegVisual->SetVisibility(false);
			JailerRightLegVisual->SetHiddenInGame(true);
		}
		if (JailerLeftHandKeyVisual)
		{
			JailerLeftHandKeyVisual->SetVisibility(false);
			JailerLeftHandKeyVisual->SetHiddenInGame(true);
		}
		if (JailerRightHandKeyVisual)
		{
			JailerRightHandKeyVisual->SetVisibility(false);
			JailerRightHandKeyVisual->SetHiddenInGame(true);
		}
		bJailerHuskDressingReady = false;
		return;
	}

	if (!JailerKeyRingVisual || !JailerKeyTeethVisual || !JailerChainVisual || !JailerMantleVisual || !JailerLeftArmVisual || !JailerRightArmVisual || !JailerManacleVisual || !JailerHipsVisual || !JailerRobeVisual || !JailerSpineVisual || !JailerLeftLegVisual || !JailerRightLegVisual || !JailerLeftHandKeyVisual || !JailerRightHandKeyVisual)
	{
		return;
	}

	if (BodyVisual)
	{
		BodyVisual->SetVisibility(false);
		BodyVisual->SetHiddenInGame(true);
	}
	if (MarkerVisual)
	{
		MarkerVisual->SetVisibility(false);
		MarkerVisual->SetHiddenInGame(true);
	}
	if (GetMesh())
	{
		GetMesh()->SetVisibility(false);
		GetMesh()->SetHiddenInGame(true);
	}
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_PLACEHOLDER_BODY_HIDDEN_READY: Paragon placeholder body hidden for skeletal Jailer Husk silhouette."));

	const FLinearColor BrassColor(1.0f, 0.68f, 0.22f);
	const FLinearColor RustedIronColor(0.34f, 0.31f, 0.28f);
	const FLinearColor DeadLeatherColor(0.13f, 0.085f, 0.055f);
	const FLinearColor DryArmColor(0.50f, 0.42f, 0.32f);
	const FLinearColor OldBoneColor(0.64f, 0.58f, 0.46f);
	const bool bChainImported = ConfigureStaticVisual(
		JailerChainVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	const bool bKeyRingImported = ConfigureStaticVisual(
		JailerKeyRingVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Ring.SM_Decor_Ring"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	const bool bKeyTeethImported = ConfigureStaticVisual(
		JailerKeyTeethVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Key.SM_Lock_Key"),
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	const bool bMantleReady = ConfigureStaticVisual(
		JailerMantleVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"),
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A"));
	const bool bLeftArmReady = ConfigureStaticVisual(
		JailerLeftArmVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm.SM_Bones_Arm"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	const bool bRightArmReady = ConfigureStaticVisual(
		JailerRightArmVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm.SM_Bones_Arm"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	const bool bManacleReady = ConfigureStaticVisual(
		JailerManacleVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Ring.SM_Decor_Ring"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	const bool bHipsReady = ConfigureStaticVisual(
		JailerHipsVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Hips.SM_Bones_Hips"),
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	const bool bRobeReady = ConfigureStaticVisual(
		JailerRobeVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B"),
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	const bool bSpineReady = ConfigureStaticVisual(
		JailerSpineVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Neck.SM_Bones_Neck"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	const bool bLeftLegReady = ConfigureStaticVisual(
		JailerLeftLegVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_FemurLeft.SM_Bones_FemurLeft"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	const bool bRightLegReady = ConfigureStaticVisual(
		JailerRightLegVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_FemurRight.SM_Bones_FemurRight"),
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	const bool bLeftHandKeyReady = ConfigureStaticVisual(
		JailerLeftHandKeyVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Key.SM_Lock_Key"),
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	const bool bRightHandKeyReady = ConfigureStaticVisual(
		JailerRightHandKeyVisual,
		TEXT("/Game/MedievalDungeon/Meshes/Props/SM_Lock_Key.SM_Lock_Key"),
		TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (BoneTorsoVisual)
	{
		BoneTorsoVisual->SetVisibility(true);
		BoneTorsoVisual->SetHiddenInGame(false);
		BoneTorsoVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 62.0f));
		BoneTorsoVisual->SetRelativeRotation(FRotator(-4.0f, 0.0f, 0.0f));
		BoneTorsoVisual->SetRelativeScale3D(FVector(0.92f));
		ApplyPlaceholderColor(BoneTorsoVisual, OldBoneColor);
	}
	if (BoneHeadVisual)
	{
		BoneHeadVisual->SetVisibility(true);
		BoneHeadVisual->SetHiddenInGame(false);
		BoneHeadVisual->SetRelativeLocation(FVector(18.0f, 0.0f, 112.0f));
		BoneHeadVisual->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		BoneHeadVisual->SetRelativeScale3D(FVector(0.62f));
		ApplyPlaceholderColor(BoneHeadVisual, OldBoneColor);
	}
	if (WeaponVisual)
	{
		WeaponVisual->SetVisibility(true);
		WeaponVisual->SetHiddenInGame(false);
		WeaponVisual->SetRelativeLocation(FVector(58.0f, -34.0f, 34.0f));
		WeaponVisual->SetRelativeRotation(FRotator(58.0f, -18.0f, 116.0f));
		WeaponVisual->SetRelativeScale3D(FVector(0.56f));
		ApplyPlaceholderColor(WeaponVisual, DryArmColor);
	}

	JailerMantleVisual->SetVisibility(true);
	JailerMantleVisual->SetHiddenInGame(false);
	JailerMantleVisual->SetRelativeLocation(FVector(12.0f, 0.0f, 88.0f));
	JailerMantleVisual->SetRelativeRotation(FRotator(88.0f, 0.0f, 0.0f));
	JailerMantleVisual->SetRelativeScale3D(FVector(0.34f));
	ApplyPlaceholderColor(JailerMantleVisual, DeadLeatherColor);

	JailerLeftArmVisual->SetVisibility(true);
	JailerLeftArmVisual->SetHiddenInGame(false);
	JailerLeftArmVisual->SetRelativeLocation(FVector(42.0f, -46.0f, 54.0f));
	JailerLeftArmVisual->SetRelativeRotation(FRotator(22.0f, 0.0f, 58.0f));
	JailerLeftArmVisual->SetRelativeScale3D(FVector(0.42f));
	ApplyPlaceholderColor(JailerLeftArmVisual, DryArmColor);

	JailerRightArmVisual->SetVisibility(true);
	JailerRightArmVisual->SetHiddenInGame(false);
	JailerRightArmVisual->SetRelativeLocation(FVector(42.0f, 46.0f, 54.0f));
	JailerRightArmVisual->SetRelativeRotation(FRotator(-22.0f, 0.0f, -58.0f));
	JailerRightArmVisual->SetRelativeScale3D(FVector(0.42f));
	ApplyPlaceholderColor(JailerRightArmVisual, DryArmColor);

	JailerHipsVisual->SetVisibility(true);
	JailerHipsVisual->SetHiddenInGame(false);
	JailerHipsVisual->SetRelativeLocation(FVector(2.0f, 0.0f, 36.0f));
	JailerHipsVisual->SetRelativeRotation(FRotator(0.0f, -8.0f, 0.0f));
	JailerHipsVisual->SetRelativeScale3D(bHipsReady ? FVector(0.52f) : FVector(0.42f, 0.24f, 0.14f));
	ApplyPlaceholderColor(JailerHipsVisual, DryArmColor);

	JailerRobeVisual->SetVisibility(true);
	JailerRobeVisual->SetHiddenInGame(false);
	JailerRobeVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 48.0f));
	JailerRobeVisual->SetRelativeRotation(FRotator(88.0f, 0.0f, 0.0f));
	JailerRobeVisual->SetRelativeScale3D(bRobeReady ? FVector(0.32f) : FVector(0.24f, 0.18f, 0.50f));
	ApplyPlaceholderColor(JailerRobeVisual, DeadLeatherColor);

	JailerSpineVisual->SetVisibility(true);
	JailerSpineVisual->SetHiddenInGame(false);
	JailerSpineVisual->SetRelativeLocation(FVector(12.0f, 0.0f, 82.0f));
	JailerSpineVisual->SetRelativeRotation(FRotator(8.0f, -8.0f, 0.0f));
	JailerSpineVisual->SetRelativeScale3D(bSpineReady ? FVector(0.46f) : FVector(0.10f, 0.10f, 0.54f));
	ApplyPlaceholderColor(JailerSpineVisual, DryArmColor);

	JailerLeftLegVisual->SetVisibility(true);
	JailerLeftLegVisual->SetHiddenInGame(false);
	JailerLeftLegVisual->SetRelativeLocation(FVector(-8.0f, -28.0f, 12.0f));
	JailerLeftLegVisual->SetRelativeRotation(FRotator(58.0f, -12.0f, 18.0f));
	JailerLeftLegVisual->SetRelativeScale3D(bLeftLegReady ? FVector(0.48f) : FVector(0.08f, 0.08f, 0.62f));
	ApplyPlaceholderColor(JailerLeftLegVisual, DryArmColor);

	JailerRightLegVisual->SetVisibility(true);
	JailerRightLegVisual->SetHiddenInGame(false);
	JailerRightLegVisual->SetRelativeLocation(FVector(-8.0f, 28.0f, 12.0f));
	JailerRightLegVisual->SetRelativeRotation(FRotator(-58.0f, 12.0f, -18.0f));
	JailerRightLegVisual->SetRelativeScale3D(bRightLegReady ? FVector(0.48f) : FVector(0.08f, 0.08f, 0.62f));
	ApplyPlaceholderColor(JailerRightLegVisual, DryArmColor);

	JailerManacleVisual->SetVisibility(true);
	JailerManacleVisual->SetHiddenInGame(false);
	JailerManacleVisual->SetRelativeLocation(FVector(62.0f, 0.0f, 46.0f));
	JailerManacleVisual->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	JailerManacleVisual->SetRelativeScale3D(bManacleReady ? FVector(0.32f) : FVector(0.34f, 0.34f, 0.08f));
	ApplyPlaceholderColor(JailerManacleVisual, BrassColor);

	JailerChainVisual->SetVisibility(true);
	JailerChainVisual->SetHiddenInGame(false);
	JailerChainVisual->SetRelativeLocation(FVector(26.0f, -20.0f, 66.0f));
	JailerChainVisual->SetRelativeRotation(FRotator(12.0f, -18.0f, 62.0f));
	JailerChainVisual->SetRelativeScale3D(bChainImported ? FVector(0.26f) : FVector(0.08f, 0.08f, 0.58f));
	ApplyPlaceholderColor(JailerChainVisual, RustedIronColor);

	JailerKeyRingVisual->SetVisibility(true);
	JailerKeyRingVisual->SetHiddenInGame(false);
	JailerKeyRingVisual->SetRelativeLocation(FVector(70.0f, 34.0f, 40.0f));
	JailerKeyRingVisual->SetRelativeRotation(FRotator(88.0f, 12.0f, -18.0f));
	JailerKeyRingVisual->SetRelativeScale3D(bKeyRingImported ? FVector(0.24f) : FVector(0.24f, 0.24f, 0.045f));
	ApplyPlaceholderColor(JailerKeyRingVisual, BrassColor);

	JailerKeyTeethVisual->SetVisibility(true);
	JailerKeyTeethVisual->SetHiddenInGame(false);
	JailerKeyTeethVisual->SetRelativeLocation(FVector(86.0f, 44.0f, 28.0f));
	JailerKeyTeethVisual->SetRelativeRotation(FRotator(8.0f, 18.0f, -14.0f));
	JailerKeyTeethVisual->SetRelativeScale3D(bKeyTeethImported ? FVector(0.28f) : FVector(0.12f, 0.05f, 0.38f));
	ApplyPlaceholderColor(JailerKeyTeethVisual, BrassColor);

	JailerLeftHandKeyVisual->SetVisibility(true);
	JailerLeftHandKeyVisual->SetHiddenInGame(false);
	JailerLeftHandKeyVisual->SetRelativeLocation(FVector(66.0f, -52.0f, 34.0f));
	JailerLeftHandKeyVisual->SetRelativeRotation(FRotator(18.0f, 16.0f, 84.0f));
	JailerLeftHandKeyVisual->SetRelativeScale3D(bLeftHandKeyReady ? FVector(0.24f) : FVector(0.10f, 0.04f, 0.36f));
	ApplyPlaceholderColor(JailerLeftHandKeyVisual, BrassColor);

	JailerRightHandKeyVisual->SetVisibility(true);
	JailerRightHandKeyVisual->SetHiddenInGame(false);
	JailerRightHandKeyVisual->SetRelativeLocation(FVector(66.0f, 52.0f, 34.0f));
	JailerRightHandKeyVisual->SetRelativeRotation(FRotator(-18.0f, -16.0f, -84.0f));
	JailerRightHandKeyVisual->SetRelativeScale3D(bRightHandKeyReady ? FVector(0.24f) : FVector(0.10f, 0.04f, 0.36f));
	ApplyPlaceholderColor(JailerRightHandKeyVisual, BrassColor);

	bJailerHuskDressingReady = bChainImported && bKeyRingImported && bKeyTeethImported && bMantleReady && bLeftArmReady && bRightArmReady && bManacleReady && bHipsReady && bRobeReady && bSpineReady && bLeftLegReady && bRightLegReady && bLeftHandKeyReady && bRightHandKeyReady;
	UE_LOG(
		LogTemp,
		Display,
		TEXT("%s: %s fused key ring, key teeth and jailer chain dressing configured."),
		bJailerHuskDressingReady ? TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_DRESSING_READY") : TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_FALLBACK_READY"),
		*GetName());
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_BESPOKE_SILHOUETTE_READY: %s dry jailer mantle, bound arms, manacle and fused key silhouette configured."), *GetName());
	UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VERTICAL_BODY_READY: %s hips, broken legs and fused hand keys complete the dead jailer body silhouette."), *GetName());
}

void ABroodEnemyCharacter::CacheDungeonLevel5MotionPose()
{
	if (BodyVisual)
	{
		BodyMotionBase = BodyVisual->GetRelativeTransform();
	}
	if (MarkerVisual)
	{
		MarkerMotionBase = MarkerVisual->GetRelativeTransform();
	}
	if (BoneHeadVisual)
	{
		BoneHeadMotionBase = BoneHeadVisual->GetRelativeTransform();
	}
	if (WeaponVisual)
	{
		WeaponMotionBase = WeaponVisual->GetRelativeTransform();
	}
	if (CreatureAccentVisual)
	{
		CreatureAccentMotionBase = CreatureAccentVisual->GetRelativeTransform();
	}
	if (CreatureSecondaryAccentVisual)
	{
		CreatureSecondaryAccentMotionBase = CreatureSecondaryAccentVisual->GetRelativeTransform();
	}
	if (JailerKeyRingVisual)
	{
		JailerKeyRingMotionBase = JailerKeyRingVisual->GetRelativeTransform();
	}
	if (JailerKeyTeethVisual)
	{
		JailerKeyTeethMotionBase = JailerKeyTeethVisual->GetRelativeTransform();
	}
	if (JailerChainVisual)
	{
		JailerChainMotionBase = JailerChainVisual->GetRelativeTransform();
	}
	if (JailerLeftArmVisual)
	{
		JailerLeftArmMotionBase = JailerLeftArmVisual->GetRelativeTransform();
	}
	if (JailerRightArmVisual)
	{
		JailerRightArmMotionBase = JailerRightArmVisual->GetRelativeTransform();
	}
	if (JailerManacleVisual)
	{
		JailerManacleMotionBase = JailerManacleVisual->GetRelativeTransform();
	}
	if (JailerHipsVisual)
	{
		JailerHipsMotionBase = JailerHipsVisual->GetRelativeTransform();
	}
	if (JailerRobeVisual)
	{
		JailerRobeMotionBase = JailerRobeVisual->GetRelativeTransform();
	}
	if (JailerSpineVisual)
	{
		JailerSpineMotionBase = JailerSpineVisual->GetRelativeTransform();
	}
	if (JailerLeftLegVisual)
	{
		JailerLeftLegMotionBase = JailerLeftLegVisual->GetRelativeTransform();
	}
	if (JailerRightLegVisual)
	{
		JailerRightLegMotionBase = JailerRightLegVisual->GetRelativeTransform();
	}
	if (JailerLeftHandKeyVisual)
	{
		JailerLeftHandKeyMotionBase = JailerLeftHandKeyVisual->GetRelativeTransform();
	}
	if (JailerRightHandKeyVisual)
	{
		JailerRightHandKeyMotionBase = JailerRightHandKeyVisual->GetRelativeTransform();
	}

	VisualMotionTime = 0.0f;
	bDungeonLevel5MotionPoseCached = true;
	bDungeonLevel5EnemyMotionLogged = false;
	bJailerHuskChainMotionLogged = false;
}

void ABroodEnemyCharacter::AnimateDungeonLevel5Dressing(float DeltaSeconds)
{
	if (!BoneHeadVisual || !WeaponVisual || !BoneHeadVisual->IsVisible() || !WeaponVisual->IsVisible())
	{
		return;
	}
	if (!bDungeonLevel5MotionPoseCached)
	{
		CacheDungeonLevel5MotionPose();
	}

	VisualMotionTime += DeltaSeconds;
	const float HeadYaw = FMath::Sin(VisualMotionTime * 3.3f) * 5.0f;
	const float WeaponRoll = FMath::Sin(VisualMotionTime * 4.7f) * 8.0f;
	const float BodyBob = FMath::Sin(VisualMotionTime * 5.2f) * 4.5f;
	const float CrawlLurch = FMath::Sin(VisualMotionTime * 7.4f) * 7.0f;
	const float SlimePulse = 1.0f + (FMath::Sin(VisualMotionTime * 4.8f) * 0.12f);
	const float WingFlap = FMath::Sin(VisualMotionTime * 9.0f) * 18.0f;
	const float KeySway = FMath::Sin(VisualMotionTime * 5.1f) * 10.0f;
	const float ChainSway = FMath::Sin(VisualMotionTime * 2.6f) * 7.0f;
	const float BoundArmSway = FMath::Sin(VisualMotionTime * 3.8f) * 5.0f;

	BoneHeadVisual->SetRelativeLocation(BoneHeadMotionBase.GetLocation() + FVector(0.0f, 0.0f, BodyBob * 0.35f));
	BoneHeadVisual->SetRelativeRotation((BoneHeadMotionBase.GetRotation() * FRotator(0.0f, HeadYaw, 0.0f).Quaternion()).Rotator());
	WeaponVisual->SetRelativeRotation((WeaponMotionBase.GetRotation() * FRotator(0.0f, 0.0f, WeaponRoll).Quaternion()).Rotator());

	if (BodyVisual && BodyVisual->IsVisible())
	{
		BodyVisual->SetRelativeLocation(BodyMotionBase.GetLocation() + FVector(CrawlLurch * 0.5f, 0.0f, BodyBob));
	}
	if (MarkerVisual && MarkerVisual->IsVisible())
	{
		MarkerVisual->SetRelativeLocation(MarkerMotionBase.GetLocation() + FVector(0.0f, 0.0f, BodyBob * 0.25f));
	}

	if (CreatureAccentVisual && CreatureAccentVisual->IsVisible())
	{
		switch (EnemyType)
		{
		case EBroodEnemyType::MeleeGrub:
			CreatureAccentVisual->SetRelativeLocation(CreatureAccentMotionBase.GetLocation() + FVector(-CrawlLurch, 0.0f, BodyBob * 0.35f));
			CreatureAccentVisual->SetRelativeScale3D(CreatureAccentMotionBase.GetScale3D() * FVector(1.0f, 1.0f + FMath::Abs(FMath::Sin(VisualMotionTime * 6.5f)) * 0.10f, 1.0f));
			break;
		case EBroodEnemyType::Spitter:
			CreatureAccentVisual->SetRelativeScale3D(CreatureAccentMotionBase.GetScale3D() * FVector(SlimePulse, SlimePulse, 1.0f + ((SlimePulse - 1.0f) * 1.8f)));
			break;
		case EBroodEnemyType::Brute:
			CreatureAccentVisual->SetRelativeRotation((CreatureAccentMotionBase.GetRotation() * FRotator(0.0f, 0.0f, -WingFlap).Quaternion()).Rotator());
			break;
		default:
			break;
		}
	}

	if (CreatureSecondaryAccentVisual && CreatureSecondaryAccentVisual->IsVisible())
	{
		switch (EnemyType)
		{
		case EBroodEnemyType::MeleeGrub:
			CreatureSecondaryAccentVisual->SetRelativeRotation((CreatureSecondaryAccentMotionBase.GetRotation() * FRotator(0.0f, CrawlLurch * 2.0f, 0.0f).Quaternion()).Rotator());
			break;
		case EBroodEnemyType::Spitter:
			CreatureSecondaryAccentVisual->SetRelativeLocation(CreatureSecondaryAccentMotionBase.GetLocation() + FVector(BodyBob * 0.35f, 0.0f, BodyBob * 0.45f));
			CreatureSecondaryAccentVisual->SetRelativeScale3D(CreatureSecondaryAccentMotionBase.GetScale3D() * FVector(1.0f, 1.0f, SlimePulse));
			break;
		case EBroodEnemyType::Brute:
			CreatureSecondaryAccentVisual->SetRelativeRotation((CreatureSecondaryAccentMotionBase.GetRotation() * FRotator(0.0f, 0.0f, WingFlap).Quaternion()).Rotator());
			break;
		default:
			break;
		}
	}

	if (EnemyType == EBroodEnemyType::BossBrute)
	{
		if (JailerKeyRingVisual && JailerKeyRingVisual->IsVisible())
		{
			JailerKeyRingVisual->SetRelativeRotation((JailerKeyRingMotionBase.GetRotation() * FRotator(0.0f, 0.0f, KeySway).Quaternion()).Rotator());
		}
		if (JailerKeyTeethVisual && JailerKeyTeethVisual->IsVisible())
		{
			JailerKeyTeethVisual->SetRelativeRotation((JailerKeyTeethMotionBase.GetRotation() * FRotator(0.0f, KeySway * 0.4f, 0.0f).Quaternion()).Rotator());
		}
		if (JailerChainVisual && JailerChainVisual->IsVisible())
		{
			JailerChainVisual->SetRelativeRotation((JailerChainMotionBase.GetRotation() * FRotator(0.0f, 0.0f, ChainSway).Quaternion()).Rotator());
		}
		if (JailerLeftArmVisual && JailerLeftArmVisual->IsVisible())
		{
			JailerLeftArmVisual->SetRelativeRotation((JailerLeftArmMotionBase.GetRotation() * FRotator(0.0f, 0.0f, BoundArmSway).Quaternion()).Rotator());
		}
		if (JailerRightArmVisual && JailerRightArmVisual->IsVisible())
		{
			JailerRightArmVisual->SetRelativeRotation((JailerRightArmMotionBase.GetRotation() * FRotator(0.0f, 0.0f, -BoundArmSway).Quaternion()).Rotator());
		}
		if (JailerManacleVisual && JailerManacleVisual->IsVisible())
		{
			JailerManacleVisual->SetRelativeRotation((JailerManacleMotionBase.GetRotation() * FRotator(0.0f, 0.0f, KeySway * 0.35f).Quaternion()).Rotator());
		}
		if (JailerHipsVisual && JailerHipsVisual->IsVisible())
		{
			JailerHipsVisual->SetRelativeRotation((JailerHipsMotionBase.GetRotation() * FRotator(0.0f, ChainSway * 0.18f, 0.0f).Quaternion()).Rotator());
		}
		if (JailerRobeVisual && JailerRobeVisual->IsVisible())
		{
			JailerRobeVisual->SetRelativeRotation((JailerRobeMotionBase.GetRotation() * FRotator(0.0f, ChainSway * 0.12f, 0.0f).Quaternion()).Rotator());
		}
		if (JailerSpineVisual && JailerSpineVisual->IsVisible())
		{
			JailerSpineVisual->SetRelativeRotation((JailerSpineMotionBase.GetRotation() * FRotator(0.0f, ChainSway * 0.22f, 0.0f).Quaternion()).Rotator());
		}
		if (JailerLeftLegVisual && JailerLeftLegVisual->IsVisible())
		{
			JailerLeftLegVisual->SetRelativeRotation((JailerLeftLegMotionBase.GetRotation() * FRotator(0.0f, 0.0f, BoundArmSway * 0.55f).Quaternion()).Rotator());
		}
		if (JailerRightLegVisual && JailerRightLegVisual->IsVisible())
		{
			JailerRightLegVisual->SetRelativeRotation((JailerRightLegMotionBase.GetRotation() * FRotator(0.0f, 0.0f, -BoundArmSway * 0.55f).Quaternion()).Rotator());
		}
		if (JailerLeftHandKeyVisual && JailerLeftHandKeyVisual->IsVisible())
		{
			JailerLeftHandKeyVisual->SetRelativeRotation((JailerLeftHandKeyMotionBase.GetRotation() * FRotator(0.0f, 0.0f, KeySway * 0.7f).Quaternion()).Rotator());
		}
		if (JailerRightHandKeyVisual && JailerRightHandKeyVisual->IsVisible())
		{
			JailerRightHandKeyVisual->SetRelativeRotation((JailerRightHandKeyMotionBase.GetRotation() * FRotator(0.0f, 0.0f, -KeySway * 0.7f).Quaternion()).Rotator());
		}
		if (!bJailerHuskChainMotionLogged && VisualMotionTime > 0.25f)
		{
			bJailerHuskChainMotionLogged = true;
			UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_JAILER_HUSK_CHAIN_MOTION_READY: %s key ring, manacle and chain use tick-driven fused-key sway."), *GetName());
		}
	}

	if (!bDungeonLevel5EnemyMotionLogged && VisualMotionTime > 0.25f)
	{
		bDungeonLevel5EnemyMotionLogged = true;
		UE_LOG(LogTemp, Display, TEXT("BROOD_DUNGEON_LEVEL5_ENEMY_MOTION_READY: %s Level 5 enemy uses tick-driven bone rattle, crawl, slime pulse, wing flap, or Jailer chain motion."), *GetName());
	}
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
	const TCHAR* EnemyName = TEXT("SKELETON");
	switch (EnemyType)
	{
	case EBroodEnemyType::Spitter:
		EnemyName = TEXT("SKELETON ARCHER");
		break;
	case EBroodEnemyType::Brute:
		EnemyName = TEXT("ANIMATED REMAINS");
		break;
	case EBroodEnemyType::BossBrute:
		EnemyName = TEXT("JAILER HUSK");
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
