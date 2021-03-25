//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+


#include "TPSCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "HealthComponent.h"
#include "CoopTPS.h"
#include "TPSWeapon.h"

ATPSCharacter::ATPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring"));
	CameraSpring->SetupAttachment(RootComponent);
	CameraSpring->bUsePawnControlRotation = true;
	CameraSpring->TargetArmLength = 160.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraSpring, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeLocation(FVector(0.f, 80.f, 0.f));

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	ZoomedFOV		= 65.0f;
	ZoomInterpSpeed = 20.0f;

	WeaponAttachSocketName = "WeaponSocket";
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = FollowCamera->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ATPSCharacter::OnHealthChanged);

	//Only run this code if we are on the server.
	if (GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ATPSWeapon>(StarterWeaponClass, FVector::ZeroVector,
			FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}

}

void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckZoom(DeltaTime);
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed,  this, &ATPSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATPSCharacter::EndCrouch);
	
	PlayerInputComponent->BindAction("Jump",   IE_Pressed,  this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Aim",    IE_Pressed,  this, &ATPSCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Aim",	   IE_Released, this, &ATPSCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire",   IE_Pressed,  this, &ATPSCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire",   IE_Released, this, &ATPSCharacter::StopFire);


	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",	  this, &ATPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp",	  this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn",		  this, &ACharacter::AddControllerYawInput);
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSCharacter, CurrentWeapon);
	DOREPLIFETIME(ATPSCharacter, bDied);
}

void ATPSCharacter::MoveForward(float Val)
{
	AddMovementInput(GetActorForwardVector() * Val);
}

void ATPSCharacter::MoveRight(float Val)
{
	AddMovementInput(GetActorRightVector() * Val);
}

void ATPSCharacter::BeginCrouch()
{
	Crouch();
}

void ATPSCharacter::EndCrouch()
{
	UnCrouch();
}

void ATPSCharacter::CheckZoom(float DeltaTimer)
{
	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	if (FollowCamera->FieldOfView != TargetFOV)
	{
		float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTimer, ZoomInterpSpeed);
		FollowCamera->SetFieldOfView(NewFOV);
	}
}

void ATPSCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ATPSCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ATPSCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ATPSCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ATPSCharacter::OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;
	
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
	}
}

FVector ATPSCharacter::GetPawnViewLocation() const
{
	if (FollowCamera)
	{
		//Custom implementation, it returns the camera view.
		return FollowCamera->GetComponentLocation();
	}

	//If for some reason this fails, return the original character view location.
	return Super::GetPawnViewLocation();
}

