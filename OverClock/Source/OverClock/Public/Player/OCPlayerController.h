#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OCPlayerController.generated.h"

class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
class UDA_OCInputConfig;

UCLASS()
class OVERCLOCK_API AOCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOCPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="OC|Input")
	TObjectPtr<UInputMappingContext> FallbackMappingContext = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> AppliedMappingContext = nullptr;
	

private:
	UInputMappingContext* ResolvePawnMappingContext(APawn* InPawn) const;
	
	void ApplyMappingContext(UInputMappingContext* IMC, int32 Priority = 0);
	void RemoveMappingContext();
};
