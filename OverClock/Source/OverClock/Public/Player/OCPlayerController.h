#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OCPlayerController.generated.h"

class UDA_OCInputConfig;
class UOCInputComponent;
class UAbilitySystemComponent;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionValue;
struct FGameplayTag;

UCLASS()
class OVERCLOCK_API AOCPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AOCPlayerController();
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

#pragma region Inputs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDA_OCInputConfig> InputConfigDataAsset;
	
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Jump_Pressed(const FInputActionValue& Value);
	void Input_Jump_Released(const FInputActionValue& Value);

	void Input_Ability_Pressed(FGameplayTag InInputTag);
	void Input_Ability_Released(FGameplayTag InInputTag);
	
	UFUNCTION(Server, Reliable)
	void Server_TryActivateByTag(FGameplayTag InInputTag);

	UFUNCTION(Client, Reliable)
	void Server_InputReleased_ByTag(FGameplayTag InInputTag);

	
	UAbilitySystemComponent* GetOCASC() const;
#pragma endregion
	
};
