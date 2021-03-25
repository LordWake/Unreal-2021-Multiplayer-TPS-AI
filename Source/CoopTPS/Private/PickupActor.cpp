//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+


#include "PickupActor.h"
#include "PowerUpActor.h"

#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

APickupActor::APickupActor()
{ 	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateAbstractDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);

	CoolDownDuration = 10.0f;

	SetReplicates(true);
}

void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		Respawn();
	}
}

void APickupActor::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		#if UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("PowerUp Class is nullptr in %s. Please update your Blueprint"), *GetName());
		#endif
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerUpInstance = GetWorld()->SpawnActor<APowerUpActor>(PowerUpClass, GetTransform(), SpawnParams);
}

void APickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (GetLocalRole() == ROLE_Authority && PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerup(OtherActor);
		PowerUpInstance = nullptr;

		//Set timer to re-spawn
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &APickupActor::Respawn, CoolDownDuration);
	}
}

