#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

#include "OCCharacterBase.generated.h"

struct FInputActionValue;
class AOCPlayerController;
class UCameraComponent;

class UAbilitySystemComponent;

UCLASS()
class OVERCLOCK_API AOCCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOCCharacterBase();
	
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComp;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	//class UOCAttributeSet* OCAttributeSet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> DashAbilityClass;

public:	
	virtual void Tick(float DeltaTime) override;
	
#pragma region CharacterMovement
	virtual void SetupPlayerInputComponent(class UInputComponent* EnhancedInputComp) override;
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint();
	UFUNCTION()
	void StopSprint();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float JumpVelocity;

	UFUNCTION()
	virtual void GANormalAttack();
	
#pragma endregion
};
