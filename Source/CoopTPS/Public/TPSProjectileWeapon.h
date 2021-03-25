//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "TPSWeapon.h"
#include "TPSProjectileWeapon.generated.h"

class AGrenadeProjectile;

UCLASS()
class COOPTPS_API ATPSProjectileWeapon : public ATPSWeapon
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
	TSubclassOf<AGrenadeProjectile> ProjectileClass;

	virtual void Fire() override;

};
