#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OCGameInterface.generated.h"

UINTERFACE(MinimalAPI)
class UOCGameInterface : public UInterface
{
	GENERATED_BODY()
};

class OVERCLOCK_API IOCGameInterface
{
	GENERATED_BODY()

public:
	virtual FTransform GetRespawnTransform() const = 0;
	virtual void OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn) = 0;

};
