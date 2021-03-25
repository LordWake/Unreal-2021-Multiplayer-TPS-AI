//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+


#include "TPSGameMode.h"
#include "HealthComponent.h"

ATPSGameMode::ATPSGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATPSGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAnyPlayerAlive();
}

void ATPSGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UHealthComponent* HealthComponent = Cast<UHealthComponent>(MyPawn->GetComponentByClass(UHealthComponent::StaticClass()));
			
			if (ensure(HealthComponent) && HealthComponent->GetHealth() > 0.0f)
			{
				return;
			}
		}
	}
	
	RestartDeadPlayers();
}

void ATPSGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}
