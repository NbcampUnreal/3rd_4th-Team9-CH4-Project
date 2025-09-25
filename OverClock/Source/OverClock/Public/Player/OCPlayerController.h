#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OCPlayerController.generated.h"

class UDA_OCInputConfig;
class UOCInputComponent;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionValue;

UCLASS()
class OVERCLOCK_API AOCPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AOCPlayerController();

	virtual void PostNetInit() override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

private:

#pragma region Inputs
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDA_OCInputConfig> InputConfigDataAsset;

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Jump_Pressed(const FInputActionValue& Value);
	void Input_Jump_Released(const FInputActionValue& Value);
	
	
#pragma endregion 
};
