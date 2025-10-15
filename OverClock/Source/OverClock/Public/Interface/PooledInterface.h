#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PooledInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPooledInterface : public UInterface
{
	GENERATED_BODY()
};

class OVERCLOCK_API IPooledInterface
{
	GENERATED_BODY()

public:
	bool bInitial = true;

	virtual void Alloc() = 0;
	virtual void Init() = 0;
	virtual void UnInit() = 0;
	virtual void Release() = 0;
};
