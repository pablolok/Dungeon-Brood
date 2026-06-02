#include "Enemies/BroodEnemyCharacter.h"

#include "Characters/BroodCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Game/BroodGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABroodEnemyCharacter::ABroodEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
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
	if (Distance > AttackRange * 0.85f)
	{
		AddMovementInput(ToPlayer.GetSafeNormal2D(), 1.0f);
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
		UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
		UE_LOG(LogTemp, Log, TEXT("%s attacked player for %.1f damage."), *GetName(), AttackDamage);
	}
}

void ABroodEnemyCharacter::Die(AActor* DamageCauser)
{
	bIsDead = true;
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(0.25f);
	UE_LOG(LogTemp, Log, TEXT("%s died."), *GetName());

	if (ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr)
	{
		BroodGameMode->NotifyEnemyKilled(this);
	}
}
