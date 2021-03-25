//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

UCLASS()
class COOPTPS_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	

	AExplosiveBarrel();

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class URadialForceComponent* RadialForceComp;

protected:

	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	float ExplosionImpulse;
	
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UMaterialInterface* ExplodedMaterial;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigateBy, AActor* DamageCause);

	UFUNCTION()
	void OnRep_Exploded();
};
