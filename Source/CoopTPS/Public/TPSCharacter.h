//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

class ATPSWeapon;

UCLASS()
class COOPTPS_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ATPSCharacter();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraSpring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComp;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void MoveForward(float Val);
	void MoveRight(float Val);
	void BeginCrouch();
	void EndCrouch();

protected:

	bool bWantsToZoom;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	void CheckZoom(float DeltaTimer);
	void BeginZoom();
	void EndZoom();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	/*Replicated to let the client fire his weapon.*/
	UPROPERTY(Replicated)
	ATPSWeapon* CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ATPSWeapon> StarterWeaponClass;

public:

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

protected:

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:

	virtual FVector GetPawnViewLocation() const override;

};
