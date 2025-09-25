#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"

#include "OCPlayerController.generated.h"

struct FGameplayTagContainer;
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

	// 서버로 능력을 활성화해달라고 요청하는 함수
	UFUNCTION(Server, Reliable)
	void Server_ActivateSkill(TSubclassOf<UGameplayAbility> DeadlyBulletClass);
private:

#pragma region Inputs
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDA_OCInputConfig> InputConfigDataAsset;

	UFUNCTION(Server, Reliable)
	void Server_EnsureAbilityGivenByTag(FGameplayTag AbilityTag);

	UFUNCTION(Server, Reliable)
	void Server_TryActivateByTag(FGameplayTag AbilityTag);

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Jump_Pressed(const FInputActionValue& Value);
	void Input_Jump_Released(const FInputActionValue& Value);

	void Input_Attack_Pressed(const FInputActionValue& Value);
	void Input_Reload(const FInputActionValue& Value);
	void Input_Attack_Alt();
	void Input_Skill_Active();
	void Input_Ultimate();
	void Input_Interact();
	
#pragma endregion

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> DeadlyBulletGAClass;
};
