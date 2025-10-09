#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OCPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class OVERCLOCK_API AOCPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext = nullptr;

private:
	void AddDefaultMapping() const;
	void RemoveDefaultMapping() const;

	
};
