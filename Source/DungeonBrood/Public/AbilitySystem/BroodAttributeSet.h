#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BroodAttributeSet.generated.h"

#define BROOD_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(BlueprintType)
class DUNGEONBROOD_API UBroodAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBroodAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	BROOD_ATTRIBUTE_ACCESSORS(UBroodAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	BROOD_ATTRIBUTE_ACCESSORS(UBroodAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stamina;
	BROOD_ATTRIBUTE_ACCESSORS(UBroodAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	BROOD_ATTRIBUTE_ACCESSORS(UBroodAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData AttackPower;
	BROOD_ATTRIBUTE_ACCESSORS(UBroodAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Armor;
	BROOD_ATTRIBUTE_ACCESSORS(UBroodAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MovementSpeed;
	BROOD_ATTRIBUTE_ACCESSORS(UBroodAttributeSet, MovementSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Biomass;
	BROOD_ATTRIBUTE_ACCESSORS(UBroodAttributeSet, Biomass)
};

#undef BROOD_ATTRIBUTE_ACCESSORS
