//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+


#include "GrenadeProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"

AGrenadeProjectile::AGrenadeProjectile()
{	
	PrimaryActorTick.bCanEverTick = true;

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetSimulatePhysics(true);
	RootComponent = GrenadeMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bShouldBounce = true;
}

void AGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AGrenadeProjectile::ExplodeGrenade, 1.0f, false);
}

void AGrenadeProjectile::ExplodeGrenade()
{
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect,
			GetActorLocation(), FRotator(0.f, 0.f, 0.f));
	}

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(GetWorld(), 100.f, GetActorLocation(), 200.f, 
	nullptr, IgnoredActors, this, GetInstigatorController(), true, ECC_Visibility);

	Destroy();
}


