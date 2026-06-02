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

	UFUNCTION(BlueprintCallable, Category = "Brood|Abilities")
	virtual void BasicAttack();

	UFUNCTION(BlueprintCallable, Category = "Brood|Abilities")
	virtual void Dodge();

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
	void AddDefaultInputMappingContext() const;
	void ApplyInitialMovementSpeed() const;
};
