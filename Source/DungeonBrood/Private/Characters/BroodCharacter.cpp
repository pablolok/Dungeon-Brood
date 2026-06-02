#include "Characters/BroodCharacter.h"

#include "AbilitySystem/BroodAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Evolution/EvolutionComponent.h"
#include "Game/BroodGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABroodCharacter::ABroodCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UBroodAttributeSet>(TEXT("AttributeSet"));
	EvolutionComponent = CreateDefaultSubobject<UEvolutionComponent>(TEXT("EvolutionComponent"));
}

UAbilitySystemComponent* ABroodCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABroodCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	AddDefaultInputMappingContext();
	ApplyInitialMovementSpeed();

	GetWorldTimerManager().SetTimer(StaminaRegenTimerHandle, this, &ABroodCharacter::RegenerateStamina, 0.25f, true);
	PrintStatus();
}

void ABroodCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABroodCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABroodCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABroodCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABroodCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ABroodCharacter::BasicAttack);
	PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &ABroodCharacter::Dodge);
	PlayerInputComponent->BindAction(TEXT("ChooseEvolution1"), IE_Pressed, this, &ABroodCharacter::ChooseEvolutionOne);
	PlayerInputComponent->BindAction(TEXT("ChooseEvolution2"), IE_Pressed, this, &ABroodCharacter::ChooseEvolutionTwo);
	PlayerInputComponent->BindAction(TEXT("ChooseEvolution3"), IE_Pressed, this, &ABroodCharacter::ChooseEvolutionThree);
	PlayerInputComponent->BindAction(TEXT("RestartRun"), IE_Pressed, this, &ABroodCharacter::RestartRun);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABroodCharacter::Move);
	}

	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABroodCharacter::Look);
	}

	if (BasicAttackAction)
	{
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &ABroodCharacter::BasicAttack);
	}

	if (DodgeAction)
	{
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ABroodCharacter::Dodge);
	}
}

void ABroodCharacter::BasicAttack()
{
	if (bIsDead)
	{
		return;
	}

	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 45.0f);
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	FCollisionShape Shape = FCollisionShape::MakeSphere(AttackRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(BroodBasicAttack), false, this);
	TArray<FHitResult> Hits;

	const bool bHit = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, Shape, Params);
	DrawDebugSphere(GetWorld(), End, AttackRadius, 16, bHit ? FColor::Red : FColor::Silver, false, 0.35f);

	float Damage = AttributeSet ? AttributeSet->GetAttackPower() : 10.0f;
	if (bVenomStrike)
	{
		Damage += VenomBonusDamage;
	}

	TSet<AActor*> DamagedActors;
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (IsValid(HitActor) && HitActor != this && !DamagedActors.Contains(HitActor))
		{
			DamagedActors.Add(HitActor);
			UGameplayStatics::ApplyDamage(HitActor, Damage, GetController(), this, UDamageType::StaticClass());
			UE_LOG(LogTemp, Log, TEXT("Brood attack hit %s for %.1f damage."), *HitActor->GetName(), Damage);
		}
	}

	if (DamagedActors.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Brood attack missed."));
	}
}

void ABroodCharacter::Dodge()
{
	if (bIsDead || !bCanDodge || !AttributeSet)
	{
		return;
	}

	if (AttributeSet->GetStamina() < DodgeStaminaCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dodge blocked: stamina too low."));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Yellow, TEXT("Not enough stamina to dodge"));
		}
		return;
	}

	SetStamina(AttributeSet->GetStamina() - DodgeStaminaCost);
	bCanDodge = false;

	FVector DodgeDirection = GetActorForwardVector();
	if (!LastMovementInput.IsNearlyZero())
	{
		const FRotator YawRotation(0.0f, Controller ? Controller->GetControlRotation().Yaw : GetActorRotation().Yaw, 0.0f);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		DodgeDirection = (ForwardDirection * LastMovementInput.Y + RightDirection * LastMovementInput.X).GetSafeNormal();
	}

	LaunchCharacter(DodgeDirection * DodgeImpulseStrength, true, false);
	GetWorldTimerManager().SetTimer(DodgeCooldownTimerHandle, this, &ABroodCharacter::ResetDodgeCooldown, DodgeCooldownSeconds, false);
	UE_LOG(LogTemp, Log, TEXT("Brood dodged. Stamina: %.1f"), AttributeSet->GetStamina());
	PrintStatus();
}

float ABroodCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead || !AttributeSet)
	{
		return 0.0f;
	}

	const float MitigatedDamage = FMath::Max(1.0f, DamageAmount - AttributeSet->GetArmor());
	SetHealth(AttributeSet->GetHealth() - MitigatedDamage);
	UE_LOG(LogTemp, Warning, TEXT("Brood took %.1f damage from %s. Health: %.1f"), MitigatedDamage, *GetNameSafe(DamageCauser), AttributeSet->GetHealth());

	if (bAcidBlood)
	{
		ApplyAcidBlood();
	}

	if (AttributeSet->GetHealth() <= 0.0f)
	{
		bIsDead = true;
		GetCharacterMovement()->DisableMovement();
		UE_LOG(LogTemp, Error, TEXT("Player dead."));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, TEXT("PLAYER DEAD - Press R to restart"));
		}
	}

	PrintStatus();
	return MitigatedDamage;
}

void ABroodCharacter::AddBiomass(float Amount)
{
	if (!AttributeSet || Amount <= 0.0f)
	{
		return;
	}

	AttributeSet->SetBiomass(AttributeSet->GetBiomass() + Amount);
	PrintStatus();
}

void ABroodCharacter::Heal(float Amount)
{
	if (!AttributeSet || Amount <= 0.0f || bIsDead)
	{
		return;
	}

	SetHealth(AttributeSet->GetHealth() + Amount);
	PrintStatus();
}

bool ABroodCharacter::ApplyEvolution(FName EvolutionId)
{
	if (!EvolutionComponent || !AttributeSet || EvolutionId.IsNone() || !EvolutionComponent->AcquireEvolution(EvolutionId))
	{
		return false;
	}

	if (EvolutionId == TEXT("HardenedCarapace"))
	{
		SetArmor(AttributeSet->GetArmor() + 3.0f);
		SetMovementSpeed(AttributeSet->GetMovementSpeed() - 35.0f);
	}
	else if (EvolutionId == TEXT("RendingClaws"))
	{
		SetAttackPower(AttributeSet->GetAttackPower() + 6.0f);
	}
	else if (EvolutionId == TEXT("NeuralOverdrive"))
	{
		SetMovementSpeed(AttributeSet->GetMovementSpeed() + 60.0f);
		SetMaxStamina(AttributeSet->GetMaxStamina() + 25.0f);
		SetStamina(AttributeSet->GetStamina() + 25.0f);
	}
	else if (EvolutionId == TEXT("DigestEssence"))
	{
		bDigestEssence = true;
	}
	else if (EvolutionId == TEXT("AcidBlood"))
	{
		bAcidBlood = true;
	}
	else if (EvolutionId == TEXT("BroodInstinct"))
	{
		BiomassRewardMultiplier += 0.5f;
	}
	else if (EvolutionId == TEXT("VenomStrike"))
	{
		bVenomStrike = true;
	}
	else if (EvolutionId == TEXT("PredatorLeap"))
	{
		DodgeImpulseStrength += 250.0f;
		DodgeCooldownSeconds = FMath::Max(0.35f, DodgeCooldownSeconds - 0.25f);
	}
	else if (EvolutionId == TEXT("ThickHide"))
	{
		SetMaxHealth(AttributeSet->GetMaxHealth() + 35.0f);
		SetHealth(AttributeSet->GetHealth() + 35.0f);
	}

	UE_LOG(LogTemp, Log, TEXT("Evolution acquired: %s"), *EvolutionId.ToString());
	PrintStatus();
	return true;
}

bool ABroodCharacter::IsDead() const
{
	return bIsDead;
}

float ABroodCharacter::GetHealth() const
{
	return AttributeSet ? AttributeSet->GetHealth() : 0.0f;
}

float ABroodCharacter::GetStamina() const
{
	return AttributeSet ? AttributeSet->GetStamina() : 0.0f;
}

float ABroodCharacter::GetBiomass() const
{
	return AttributeSet ? AttributeSet->GetBiomass() : 0.0f;
}

float ABroodCharacter::GetBiomassRewardMultiplier() const
{
	return BiomassRewardMultiplier;
}

void ABroodCharacter::OnEnemyKilled(float BiomassReward)
{
	AddBiomass(BiomassReward * BiomassRewardMultiplier);
	if (bDigestEssence)
	{
		Heal(8.0f);
	}
}

void ABroodCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (!Controller || MovementVector.IsNearlyZero())
	{
		return;
	}

	const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
	LastMovementInput = MovementVector;
}

void ABroodCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (LookAxisVector.IsNearlyZero())
	{
		return;
	}

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ABroodCharacter::MoveForward(float Value)
{
	if (Controller && !bIsDead && !FMath::IsNearlyZero(Value))
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
		LastMovementInput.Y = Value;
	}
}

void ABroodCharacter::MoveRight(float Value)
{
	if (Controller && !bIsDead && !FMath::IsNearlyZero(Value))
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
		LastMovementInput.X = Value;
	}
}

void ABroodCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABroodCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABroodCharacter::AddDefaultInputMappingContext() const
{
	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController || !DefaultMappingContext)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (Subsystem)
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ABroodCharacter::ApplyInitialMovementSpeed() const
{
	if (AttributeSet && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMovementSpeed();
	}
}

void ABroodCharacter::RegenerateStamina()
{
	if (AttributeSet && !bIsDead && AttributeSet->GetStamina() < AttributeSet->GetMaxStamina())
	{
		SetStamina(AttributeSet->GetStamina() + StaminaRegenPerSecond * 0.25f);
	}
}

void ABroodCharacter::ResetDodgeCooldown()
{
	bCanDodge = true;
}

void ABroodCharacter::ChooseEvolutionOne()
{
	if (ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr)
	{
		BroodGameMode->ChooseEvolution(0);
	}
}

void ABroodCharacter::ChooseEvolutionTwo()
{
	if (ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr)
	{
		BroodGameMode->ChooseEvolution(1);
	}
}

void ABroodCharacter::ChooseEvolutionThree()
{
	if (ABroodGameMode* BroodGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABroodGameMode>() : nullptr)
	{
		BroodGameMode->ChooseEvolution(2);
	}
}

void ABroodCharacter::RestartRun()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ABroodCharacter::ApplyAcidBlood()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Shape = FCollisionShape::MakeSphere(220.0f);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(BroodAcidBlood), false, this);

	if (GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, ECC_Pawn, Shape, Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* Actor = Overlap.GetActor();
			if (IsValid(Actor) && Actor != this)
			{
				UGameplayStatics::ApplyDamage(Actor, 5.0f, GetController(), this, UDamageType::StaticClass());
			}
		}
	}
}

void ABroodCharacter::PrintStatus() const
{
	if (!GEngine || !AttributeSet)
	{
		return;
	}

	const FString Status = FString::Printf(TEXT("HP %.0f/%.0f | STA %.0f/%.0f | Biomass %.0f"),
		AttributeSet->GetHealth(),
		AttributeSet->GetMaxHealth(),
		AttributeSet->GetStamina(),
		AttributeSet->GetMaxStamina(),
		AttributeSet->GetBiomass());
	GEngine->AddOnScreenDebugMessage(1001, 1.0f, FColor::Green, Status);
}

void ABroodCharacter::SetHealth(float NewHealth)
{
	if (AttributeSet)
	{
		AttributeSet->SetHealth(FMath::Clamp(NewHealth, 0.0f, AttributeSet->GetMaxHealth()));
	}
}

void ABroodCharacter::SetStamina(float NewStamina)
{
	if (AttributeSet)
	{
		AttributeSet->SetStamina(FMath::Clamp(NewStamina, 0.0f, AttributeSet->GetMaxStamina()));
	}
}

void ABroodCharacter::SetMaxHealth(float NewMaxHealth)
{
	if (AttributeSet)
	{
		AttributeSet->SetMaxHealth(FMath::Max(1.0f, NewMaxHealth));
		SetHealth(AttributeSet->GetHealth());
	}
}

void ABroodCharacter::SetMaxStamina(float NewMaxStamina)
{
	if (AttributeSet)
	{
		AttributeSet->SetMaxStamina(FMath::Max(1.0f, NewMaxStamina));
		SetStamina(AttributeSet->GetStamina());
	}
}

void ABroodCharacter::SetAttackPower(float NewAttackPower)
{
	if (AttributeSet)
	{
		AttributeSet->SetAttackPower(FMath::Max(0.0f, NewAttackPower));
	}
}

void ABroodCharacter::SetArmor(float NewArmor)
{
	if (AttributeSet)
	{
		AttributeSet->SetArmor(FMath::Max(0.0f, NewArmor));
	}
}

void ABroodCharacter::SetMovementSpeed(float NewMovementSpeed)
{
	if (AttributeSet)
	{
		AttributeSet->SetMovementSpeed(FMath::Max(150.0f, NewMovementSpeed));
		ApplyInitialMovementSpeed();
	}
}
