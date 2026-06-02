#include "Evolution/EvolutionComponent.h"

UEvolutionComponent::UEvolutionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEvolutionComponent::AcquireEvolution(FName EvolutionId)
{
	if (EvolutionId.IsNone() || AcquiredEvolutionIds.Contains(EvolutionId))
	{
		return false;
	}

	AcquiredEvolutionIds.Add(EvolutionId);
	OnEvolutionAcquired.Broadcast(EvolutionId);
	return true;
}

bool UEvolutionComponent::HasEvolution(FName EvolutionId) const
{
	return AcquiredEvolutionIds.Contains(EvolutionId);
}

TArray<FName> UEvolutionComponent::GetAcquiredEvolutionIds() const
{
	return AcquiredEvolutionIds;
}
