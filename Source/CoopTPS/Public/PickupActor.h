//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

class APowerUpActor;

UCLASS()
class COOPTPS_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	

	APickupActor();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<APowerUpActor> PowerUpClass;

	APowerUpActor* PowerUpInstance;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float CoolDownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

	virtual void BeginPlay() override;

	void Respawn();

public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
