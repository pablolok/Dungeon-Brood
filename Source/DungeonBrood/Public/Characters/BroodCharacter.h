#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BroodCharacter.generated.h"

class UAbilitySystemComponent;
class UBroodAttributeSet;
class UEvolutionComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class DUNGEONBROOD_API ABroodCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABroodCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Brood|Abilities")
	virtual void BasicAttack();

	UFUNCTION(BlueprintCallable, Category = "Brood|Abilities")
	virtual void Dodge();

	UFUNCTION(BlueprintCallable, Category = "Brood|Stats")
	void AddBiomass(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Brood|Stats")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Brood|Evolution")
	bool ApplyEvolution(FName EvolutionId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Brood|State")
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Brood|Stats")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Brood|Stats")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Brood|Stats")
	float GetBiomass() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Brood|Stats")
	float GetBiomassRewardMultiplier() const;

	void OnEnemyKilled(float BiomassReward);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brood|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brood|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brood|GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brood|GAS")
	TObjectPtr<UBroodAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brood|Evolution")
	TObjectPtr<UEvolutionComponent> EvolutionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Brood|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Brood|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Brood|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Brood|Input")
	TObjectPtr<UInputAction> BasicAttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Brood|Input")
	TObjectPtr<UInputAction> DodgeAction;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void AddDefaultInputMappingContext() const;
	void ApplyInitialMovementSpeed() const;
	void RegenerateStamina();
	void ResetDodgeCooldown();
	void ChooseEvolutionOne();
	void ChooseEvolutionTwo();
	void ChooseEvolutionThree();
	void RestartRun();
	void ApplyAcidBlood();
	void PrintStatus() const;
	void SetHealth(float NewHealth);
	void SetStamina(float NewStamina);
	void SetMaxHealth(float NewMaxHealth);
	void SetMaxStamina(float NewMaxStamina);
	void SetAttackPower(float NewAttackPower);
	void SetArmor(float NewArmor);
	void SetMovementSpeed(float NewMovementSpeed);

	FVector2D LastMovementInput = FVector2D::ZeroVector;
	bool bIsDead = false;
	bool bCanDodge = true;
	bool bDigestEssence = false;
	bool bAcidBlood = false;
	bool bVenomStrike = false;
	float BiomassRewardMultiplier = 1.0f;
	float DodgeStaminaCost = 25.0f;
	float DodgeImpulseStrength = 900.0f;
	float DodgeCooldownSeconds = 0.8f;
	float StaminaRegenPerSecond = 18.0f;
	float AttackRange = 220.0f;
	float AttackRadius = 75.0f;
	float VenomBonusDamage = 4.0f;
	FTimerHandle StaminaRegenTimerHandle;
	FTimerHandle DodgeCooldownTimerHandle;
};
