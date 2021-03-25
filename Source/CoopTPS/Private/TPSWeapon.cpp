//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+


#include "TPSWeapon.h"
#include "Net/UnrealNetwork.h"

#include "CoopTPS.h"

#include "PhysicalMaterials/PhysicalMaterial.h"

#include "Kismet/GameplayStatics.h"

#include "Components/SkeletalMeshComponent.h"

#include "DrawDebugHelpers.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

ATPSWeapon::ATPSWeapon()
{
	WeaponMesh	  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage				 = 20.0f;
	HeadShotDamageMultiplier = 4.0f;
	BulletSpread			 = 2.0f;
	TimeBetweenShots		 = 0.1f;
	NetUpdateFrequency		 = 66.0f;
	MinNetUpdateFrequency	 = 33.0f;

	SetReplicates(true); //Replicates this actor on the clients.
}

void ATPSWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}
	
	if (AActor* MyOwner = GetOwner())
	{
		FHitResult Hit;

		FVector  EyesLocation;
		FRotator EyesRotation;
		MyOwner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

		FVector ShotDirection = EyesRotation.Vector();

		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd	   = EyesLocation + (ShotDirection * 10000.f);
		FVector TracerEndPoint = TraceEnd; //Particle target parameter.

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;			
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}
			
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;

		}

		PlayFireEffects(TracerEndPoint);

		if(GetLocalRole() == ROLE_Authority)
		{
			HitScanTrace.TraceTo	 = TracerEndPoint; //It will trigger OnRep_HitScanTrace() here.
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFiredTime = GetWorld()->TimeSeconds;
	}
}

void ATPSWeapon::ServerFire_Implementation()
{
	Fire();
}

void ATPSWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ATPSWeapon::StartFire()
{
	//Take the max value, the result or zero. Is some kind of way of clamping.
	float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ATPSWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ATPSWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ATPSWeapon::PlayFireEffects(const FVector& TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			TracerEffect, MuzzleLocation);

		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}

	if (APawn* MyOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(MyOwner->GetController()))
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}

void ATPSWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
		case SURFACE_FLESHDEFAULT:
		case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;

		default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection  = ImpactPoint - MuzzleLocation;
		
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ATPSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*Replicate to everyone except who shots this weapon.*/
	DOREPLIFETIME_CONDITION(ATPSWeapon, HitScanTrace, COND_SkipOwner);
}
