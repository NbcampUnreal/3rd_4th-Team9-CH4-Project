#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "OCPlayerController.generated.h"

class UAbilitySystemComponent;      // 인터페이스 전방선언
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

	/** PlayerState에 있는 ASC를 바로 반환하는 헬퍼 (핫패스용, 조용히 nullptr 허용) */
	FORCEINLINE UAbilitySystemComponent* GetASC() const
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PlayerState))
		{
			return ASI->GetAbilitySystemComponent();
		}
		return nullptr;
	}

private:
	// ───── Input Bindings ─────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDA_OCInputConfig> InputConfigDataAsset;

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

	// ───── Ability Trigger (공용) ─────
	void TriggerAbilityByTag(const FGameplayTag& AbilityTag);

	// ───── Server fallbacks (기존 유지) ─────
	UFUNCTION(Server, Reliable) void Server_EnsureAbilityGivenByTag(FGameplayTag AbilityTag);
	UFUNCTION(Server, Reliable) void Server_TryActivateByTag(FGameplayTag AbilityTag);

public:
	UFUNCTION(Server, Reliable) void Server_ActivateSkill(TSubclassOf<UGameplayAbility> DeadlyBulletClass);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> DeadlyBulletGAClass;
};
