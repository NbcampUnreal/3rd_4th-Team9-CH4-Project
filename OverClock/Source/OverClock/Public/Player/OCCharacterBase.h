#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

#include "OCCharacterBase.generated.h"

class UDA_OCInputConfig;
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
	UAbilitySystemComponent* AbilitySystemComponent; // 리스폰 가능성이 있으면 PlayerState에 보관
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	//class UOCAttributeSet* OCAttributeSet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> DashAbilityClass;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Tick(float DeltaTime) override;
	
#pragma region CharacterMovement
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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MovementInputData")
	UDA_OCInputConfig* InputConfigDataAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float JumpVelocity;

	UPROPERTY(Replicated)
	float AimPitch;
	
#pragma endregion
};
