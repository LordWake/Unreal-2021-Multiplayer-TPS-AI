//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeProjectile.generated.h"

UCLASS()
class COOPTPS_API AGrenadeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AGrenadeProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* GrenadeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* ExplosionEffect;

	virtual void BeginPlay() override;
	void ExplodeGrenade();

private:

	FTimerHandle ExplosionTimerHandle;
};
