#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OCGameInstance.generated.h"

UCLASS()
class OVERCLOCK_API UOCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UOCGameInstance();

	virtual void Init() override;

};
