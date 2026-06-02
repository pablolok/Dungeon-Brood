#include "Encounters/RoomEncounterManager.h"

ARoomEncounterManager::ARoomEncounterManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARoomEncounterManager::BeginPlay()
{
	Super::BeginPlay();

	TrackedEnemies.RemoveAll([](const TObjectPtr<AActor>& Enemy)
	{
		return !IsValid(Enemy);
	});

	for (AActor* Enemy : TrackedEnemies)
	{
		BindEnemyDestroyed(Enemy);
	}

	CheckEncounterCleared();
}

void ARoomEncounterManager::RegisterEnemy(AActor* EnemyActor)
{
	const bool bAlreadyTracked = TrackedEnemies.ContainsByPredicate([EnemyActor](const TObjectPtr<AActor>& Enemy)
	{
		return Enemy == EnemyActor;
	});

	if (!IsValid(EnemyActor) || bAlreadyTracked)
	{
		return;
	}

	TrackedEnemies.Add(EnemyActor);
	BindEnemyDestroyed(EnemyActor);
	bEncounterCleared = false;
}

void ARoomEncounterManager::NotifyEnemyDefeated(AActor* EnemyActor)
{
	if (!EnemyActor)
	{
		return;
	}

	TrackedEnemies.RemoveAll([EnemyActor](const TObjectPtr<AActor>& Enemy)
	{
		return Enemy == EnemyActor;
	});
	CheckEncounterCleared();
}

bool ARoomEncounterManager::IsEncounterCleared() const
{
	return bEncounterCleared;
}

void ARoomEncounterManager::HandleTrackedEnemyDestroyed(AActor* DestroyedActor)
{
	NotifyEnemyDefeated(DestroyedActor);
}

void ARoomEncounterManager::BindEnemyDestroyed(AActor* EnemyActor)
{
	if (IsValid(EnemyActor))
	{
		EnemyActor->OnDestroyed.AddUniqueDynamic(this, &ARoomEncounterManager::HandleTrackedEnemyDestroyed);
	}
}

void ARoomEncounterManager::CheckEncounterCleared()
{
	TrackedEnemies.RemoveAll([](const TObjectPtr<AActor>& Enemy)
	{
		return !IsValid(Enemy);
	});

	if (!bEncounterCleared && TrackedEnemies.IsEmpty())
	{
		bEncounterCleared = true;
		OnEncounterCleared.Broadcast();
	}
}
