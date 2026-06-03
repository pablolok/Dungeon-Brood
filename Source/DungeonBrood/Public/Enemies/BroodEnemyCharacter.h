#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BroodEnemyCharacter.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

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
	void ConfigureVisuals();
	void ConfigureDungeonLevel5Dressing();
	void ConfigureDungeonLevel5CreatureVariety();
	void ConfigureJailerHuskDressing();
	void CacheDungeonLevel5MotionPose();
	void AnimateDungeonLevel5Dressing(float DeltaSeconds);
	void TryAttackPlayer(float DeltaSeconds);
	void ResetDamageFeedback();
	void ResetAttackFeedback();
	void UpdateNameplate();
	void Die(AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> BodyVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> MarkerVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> BoneTorsoVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> BoneHeadVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> WeaponVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> CreatureAccentVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> CreatureSecondaryAccentVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerKeyRingVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerKeyTeethVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerChainVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerMantleVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerLeftArmVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerRightArmVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerManacleVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerHipsVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerRobeVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerSpineVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerLeftLegVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerRightLegVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerLeftHandKeyVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UStaticMeshComponent> JailerRightHandKeyVisual;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Visual")
	TObjectPtr<UTextRenderComponent> NameplateVisual;

	EBroodEnemyType EnemyType = EBroodEnemyType::MeleeGrub;
	float Health = 30.0f;
	float MaxHealth = 30.0f;
	float AttackDamage = 8.0f;
	float AttackRange = 130.0f;
	float AttackCooldown = 1.25f;
	float AttackTimer = 0.0f;
	float BiomassReward = 8.0f;
	FLinearColor BodyColor = FLinearColor::White;
	FLinearColor MarkerColor = FLinearColor::White;
	FTimerHandle DamageFeedbackTimerHandle;
	FTimerHandle AttackFeedbackTimerHandle;
	float VisualMotionTime = 0.0f;
	FTransform BodyMotionBase;
	FTransform MarkerMotionBase;
	FTransform BoneHeadMotionBase;
	FTransform WeaponMotionBase;
	FTransform CreatureAccentMotionBase;
	FTransform CreatureSecondaryAccentMotionBase;
	FTransform JailerKeyRingMotionBase;
	FTransform JailerKeyTeethMotionBase;
	FTransform JailerChainMotionBase;
	FTransform JailerLeftArmMotionBase;
	FTransform JailerRightArmMotionBase;
	FTransform JailerManacleMotionBase;
	FTransform JailerHipsMotionBase;
	FTransform JailerRobeMotionBase;
	FTransform JailerSpineMotionBase;
	FTransform JailerLeftLegMotionBase;
	FTransform JailerRightLegMotionBase;
	FTransform JailerLeftHandKeyMotionBase;
	FTransform JailerRightHandKeyMotionBase;
	bool bDungeonLevel5DressingReady = false;
	bool bDungeonLevel5CreatureVarietyReady = false;
	bool bJailerHuskDressingReady = false;
	bool bDungeonLevel5MotionPoseCached = false;
	bool bDungeonLevel5EnemyMotionLogged = false;
	bool bJailerHuskChainMotionLogged = false;
	bool bIsDead = false;
};
