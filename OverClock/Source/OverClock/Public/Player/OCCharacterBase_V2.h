#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "OCCharacterBase_V2.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAbilitySystemComponent;
class UOCAbilitySystemComponent;
class UOCInputComponent;
class UDA_OCInputConfig;
class UDA_OCHeroStartUpData;
class AOCPlayerState;
class UOCUIComponent;
class UOCAttributeSet_Health;
struct FInputActionValue;

UCLASS(meta=(DisplayName="OC Character Base V2"))
class OVERCLOCK_API AOCCharacterBase_V2 : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AOCCharacterBase_V2();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    UFUNCTION(BlueprintCallable, Category="OC|Input")
    const UDA_OCInputConfig* GetInputConfig() const;
    
protected:
#pragma region Components
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OC|UI")
    UOCUIComponent* UICompRef = nullptr;
    
#pragma endregion
    
#pragma region Inputs
    
    UPROPERTY(EditDefaultsOnly, Category="OC|Input")
    TSoftObjectPtr<UDA_OCInputConfig> InputConfigAsset;

    UPROPERTY(EditDefaultsOnly, Category="OC|Hero")
    TSoftObjectPtr<UDA_OCHeroStartUpData> HeroStartUpDataAsset;

    UPROPERTY(Transient)
    UDA_OCInputConfig* InputConfig = nullptr;
    
    UPROPERTY(Transient)
    UDA_OCHeroStartUpData* HeroData = nullptr;
    
    TWeakObjectPtr<UOCAbilitySystemComponent> ASCWeak;
    
    UFUNCTION()
    void Input_Move(const FInputActionValue& Value);

    UFUNCTION()
    void Input_Look(const FInputActionValue& Value);

    UFUNCTION()
    void Input_Jump_Pressed(const FInputActionValue& Value);

    UFUNCTION()
    void Input_Jump_Released(const FInputActionValue& Value);
    
    UFUNCTION()
    void Input_Ability_Pressed(FGameplayTag InInputTag);
    
    UFUNCTION()
    void Input_Ability_Released(FGameplayTag InInputTag);

    UFUNCTION(Server, Reliable)
    void Server_Ability_Pressed(FGameplayTag InInputTag);

    UFUNCTION(Server, Reliable)
    void Server_Ability_Released(FGameplayTag InInputTag);

#pragma endregion 
    
    void InitASCFromPlayerState();
    void ResolveData();
    
    UOCAbilitySystemComponent* GetOCASC() const;

    UPROPERTY(Transient)
    UOCAttributeSet_Health* HealthSet = nullptr;

    void SetupHealthBinding();
    void PushHealthToUI();
    void OnHealthChanged(const struct FOnAttributeChangeData& Data);
};
