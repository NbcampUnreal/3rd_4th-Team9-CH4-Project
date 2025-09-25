#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TeamInfoInterface.generated.h"

UINTERFACE(MinimalAPI)
class UTeamInfoInterface : public UInterface
{
	GENERATED_BODY()
};

class OVERCLOCK_API ITeamInfoInterface
{
	GENERATED_BODY()

public:
	virtual void SetInfo(int32 InTeamId) = 0;
	virtual class UUtilTeam* GetInfo() const = 0;

};
