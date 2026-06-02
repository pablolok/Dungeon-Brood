#include "Pickups/BiomassPickup.h"

#include "Characters/BroodCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ABiomassPickup::ABiomassPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	RootComponent = PickupSphere;
	PickupSphere->InitSphereRadius(72.0f);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABiomassPickup::HandlePickupOverlap);

	PickupVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupVisual"));
	PickupVisual->SetupAttachment(RootComponent);
	PickupVisual->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")));
	PickupVisual->SetRelativeScale3D(FVector(0.22f));
	PickupVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AmountVisual = CreateDefaultSubobject<UTextRenderComponent>(TEXT("AmountVisual"));
	AmountVisual->SetupAttachment(RootComponent);
	AmountVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 46.0f));
	AmountVisual->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	AmountVisual->SetHorizontalAlignment(EHTA_Center);
	AmountVisual->SetWorldSize(24.0f);
	AmountVisual->SetTextRenderColor(FColor(100, 255, 120));
	AmountVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UpdateVisuals();
}

void ABiomassPickup::InitializeBiomass(float NewBiomassAmount)
{
	BiomassAmount = FMath::Max(0.0f, NewBiomassAmount);
	UpdateVisuals();
}

float ABiomassPickup::GetBiomassAmount() const
{
	return BiomassAmount;
}

void ABiomassPickup::HandlePickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABroodCharacter* BroodCharacter = Cast<ABroodCharacter>(OtherActor);
	if (bCollected || !BroodCharacter || BiomassAmount <= 0.0f)
	{
		return;
	}

	bCollected = true;
	BroodCharacter->AddBiomass(BiomassAmount);
	UE_LOG(LogTemp, Display, TEXT("BROOD_BIOMASS_COLLECTED: %.1f biomass."), BiomassAmount);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT("+%.0f BIOMASS"), BiomassAmount));
	}

	Destroy();
}

void ABiomassPickup::UpdateVisuals()
{
	if (PickupVisual)
	{
		UMaterialInstanceDynamic* DynamicMaterial = PickupVisual->CreateAndSetMaterialInstanceDynamic(0);
		if (DynamicMaterial)
		{
			const FLinearColor BiomassColor(0.06f, 1.0f, 0.2f);
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), BiomassColor);
			DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), BiomassColor);
		}
	}

	if (AmountVisual)
	{
		AmountVisual->SetText(FText::FromString(FString::Printf(TEXT("+%.0f BIOMASS"), BiomassAmount)));
	}
}
