#pragma once
#include "Abilities/Crunch/GA_CrunchBase.h"
#include "GA_Crunch_PowerPunch.generated.h"

class UAnimMontage;

UCLASS()
class OVERCLOCK_API UGA_Crunch_PowerPunch : public UGA_CrunchBase
{
    GENERATED_BODY()
public:
    UGA_Crunch_PowerPunch() = default;

protected:
    UPROPERTY(EditDefaultsOnly, Category="Combo|Montage")
    TArray<TObjectPtr<UAnimMontage>> ComboMontages;

    UPROPERTY(EditDefaultsOnly, Category="Combo|Timing", meta=(ClampMin="0"))
    TArray<float> ComboWindowOpens;

    UPROPERTY(EditDefaultsOnly, Category="Combo|Timing", meta=(ClampMin="0"))
    TArray<float> ComboWindowCloses;
    
    UPROPERTY(EditDefaultsOnly, Category="Combo|Damage")
    float PunchDamage = 50.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Combo|Trace")
    float PunchRange = 200.f;

    UPROPERTY(EditDefaultsOnly, Category="Combo|Trace")
    FVector PunchHalfExtent = FVector(50.f, 50.f, 50.f);

    UPROPERTY(EditDefaultsOnly, Category="Combo")
    float ComboResetTime = 0.25f;

    int32 CurrentComboIndex = 0;
    bool bWindowOpen = false;
    bool bQueued     = false;
    bool bInputHold = false;

    FTimerHandle TH_Open;
    FTimerHandle TH_Close;
    FTimerHandle TH_Reset;
    FTimerHandle TH_Hit;
    FTimerHandle TH_End;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
    virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    void StartStep();
    void OpenWindow();
    void CloseWindow();
    void DoHit_Server();
    void OnStepEnd();
    void ClearAllTimers();
};
