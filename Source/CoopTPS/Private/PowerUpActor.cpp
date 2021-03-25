//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#include "PowerUpActor.h"
#include "Net/UnrealNetwork.h"

APowerUpActor::APowerUpActor()
{
	PowerupInterval		= 0.0f;
	TotalNumberOfTicks  = 0;

	bIsPowerUpActive = false;

	SetReplicates(true);
}

void APowerUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APowerUpActor, bIsPowerUpActive);
}

void APowerUpActor::OnTickPowerUp()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNumberOfTicks)
	{
		OnExpired();
		bIsPowerUpActive = false;
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void APowerUpActor::OnRep_PowerupActive()
{
	OnPowerupStateChange(bIsPowerUpActive);
}

void APowerUpActor::ActivatePowerup(AActor* ActivateFor)
{
	OnActivated(ActivateFor);

	bIsPowerUpActive = true;
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &APowerUpActor::OnTickPowerUp, PowerupInterval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}

