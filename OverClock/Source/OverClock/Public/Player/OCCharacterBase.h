#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "OCCharacterBase.generated.h"

struct FInputActionValue;
struct FGameplayTag;
class UDA_OCInputConfig;
class UDA_OCHeroStartUpData;

UCLASS()
class OVERCLOCK_API AOCCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOCCharacterBase();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UDA_OCInputConfig> InputConfigAsset;

	UPROPERTY(Transient)
	TObjectPtr<UDA_OCInputConfig> InputConfig = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Hero")
	TSoftObjectPtr<UDA_OCHeroStartUpData> HeroStartUpDataAsset;

	UPROPERTY(Transient)
	TObjectPtr<UDA_OCHeroStartUpData> HeroStartUpData = nullptr;

	TWeakObjectPtr<UAbilitySystemComponent> ASCWeak;


#pragma region Input_Function
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Jump_Pressed(const FInputActionValue& Value);
	void Input_Jump_Released(const FInputActionValue& Value);
	
	UFUNCTION()
	void Input_Ability_Pressed(const FGameplayTag& InInputTag);

	UFUNCTION()
	void Input_Ability_Released(const FGameplayTag& InInputTag);

	UFUNCTION(Server, Reliable)
	void Server_Ability_Pressed(const FGameplayTag& InInputTag);
	
	UFUNCTION(Server, Reliable)
	void Server_Ability_Released(const FGameplayTag& InInputTag);
#pragma endregion

	void InitASCFromPalyerState();
	void ResolveData();
	void AutoBindAbilityInputs(UEnhancedInputComponent* EnhancedInputComponent);

	UAbilitySystemComponent* GetASC() const;
	
};
