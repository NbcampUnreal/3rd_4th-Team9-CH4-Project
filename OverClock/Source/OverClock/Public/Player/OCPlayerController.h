#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/OCAbilityStruct.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "OCPlayerController.generated.h"

class UOCAbilityDataAsset;
struct FGameplayTagContainer;
class UAbilitySystemComponent;      // �������̽� ���漱��
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
	virtual void OnRep_PlayerState() override;

	FORCEINLINE UAbilitySystemComponent* GetASC() const;

private:

#pragma region Inputs
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDA_OCInputConfig> InputConfigDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOCAbilityDataAsset> AbilityDataAsset;

	UPROPERTY(Replicated)
	FOCAbilityStruct AbilityStruct;

	UFUNCTION(Server, Reliable)
	void Server_ActivateSkill(FGameplayTag AbilityTag);

	UFUNCTION(Server, Reliable)
	void Server_EnsureAbilityGivenByTag(FGameplayTag AbilityTag);

	UFUNCTION(Server, Reliable)
	void Server_TryActivateByTag(FGameplayTag AbilityTag);

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Jump_Pressed(const FInputActionValue& Value);
	void Input_Jump_Released(const FInputActionValue& Value);

	void Input_Attack_Pressed();
	void Input_Reload();
	void Input_Attack_Alt();
	void Input_Skill_Active();
	void Input_Ultimate();
	void Input_Interact();
	void Input_Skill1();
	void Input_Skill2();

	void TriggerAbilityByTag(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable) void Server_EnsureAbilityGivenByTag(FGameplayTag AbilityTag);
	UFUNCTION(Server, Reliable) void Server_TryActivateByTag(FGameplayTag AbilityTag);

public:
	UFUNCTION(Server, Reliable) void Server_ActivateSkill(TSubclassOf<UGameplayAbility> DeadlyBulletClass);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> DeadlyBulletGAClass;
};
