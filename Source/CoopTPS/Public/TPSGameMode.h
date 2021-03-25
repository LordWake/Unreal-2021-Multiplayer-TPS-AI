//+---------------------------------------------------------+
//| Project   : Co-op Third Person Shooter Online Game		|
//| UE Version: UE 4.25										|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"

UCLASS()
class COOPTPS_API ATPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ATPSGameMode();

private:

	virtual void Tick(float DeltaTime) override;

protected:

	void CheckAnyPlayerAlive();	
	void RestartDeadPlayers();
};
