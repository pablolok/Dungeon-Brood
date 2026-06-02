#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BroodEnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EBroodEnemyType : uint8
{
	MeleeGrub,
	Spitter,
	Brute,
	BossBrute
};

UCLASS()
class DUNGEONBROOD_API ABroodEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABroodEnemyCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void InitializeEnemy(EBroodEnemyType NewEnemyType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy")
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy")
	float GetBiomassReward() const;

private:
	void ConfigureStats();
	void TryAttackPlayer(float DeltaSeconds);
	void Die(AActor* DamageCauser);

	EBroodEnemyType EnemyType = EBroodEnemyType::MeleeGrub;
	float Health = 30.0f;
	float MaxHealth = 30.0f;
	float AttackDamage = 8.0f;
	float AttackRange = 130.0f;
	float AttackCooldown = 1.25f;
	float AttackTimer = 0.0f;
	float BiomassReward = 8.0f;
	bool bIsDead = false;
};
