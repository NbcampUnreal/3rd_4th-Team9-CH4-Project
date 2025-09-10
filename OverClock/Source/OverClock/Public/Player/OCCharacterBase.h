#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OCCharacterBase.generated.h"

struct FInputActionValue;
class AOCPlayerController;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class OVERCLOCK_API AOCCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AOCCharacterBase();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> SpringArmComp;
protected:
	virtual void BeginPlay() override;

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
	UFUNCTION()
	void BeginCrouch();
	UFUNCTION()
	void EndCrouch();
	UFUNCTION()
	void NormalAttack();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float AirControlSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchSpeed;
#pragma endregion
};
