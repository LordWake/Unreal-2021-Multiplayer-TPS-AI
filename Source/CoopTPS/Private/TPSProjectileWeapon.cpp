//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+


#include "TPSProjectileWeapon.h"
#include "GrenadeProjectile.h"

void ATPSProjectileWeapon::Fire()
{
	if (AActor* MyOwner = GetOwner())
	{
		FVector  EyesLocation;
		FRotator EyesRotation;
		MyOwner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (ProjectileClass)
		{
			GetWorld()->SpawnActor<AGrenadeProjectile>(ProjectileClass, MuzzleLocation, EyesRotation, SpawnParams);
		}
	}
}
