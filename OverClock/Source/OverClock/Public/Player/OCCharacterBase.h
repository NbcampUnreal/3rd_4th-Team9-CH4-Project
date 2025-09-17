#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "OCCharacterBase.generated.h"

class UCameraComponent;
class UAbilitySystemComponent;
class AOCPlayerState;

UCLASS()
class OVERCLOCK_API AOCCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOCCharacterBase();
	
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	FORCEINLINE float GetAimRotation() const {return AimPitch;}
	
	UFUNCTION(Server, Reliable)
	void ServerSetAimRotation(float InAimPitch);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> CameraComp;

	// 아직 프로토타입 단계라 안전성을 위해 사용 X
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|State")
	TObjectPtr<AOCPlayerState> CachedPlayerState;*/
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float JumpVelocity;
	
	UPROPERTY(Replicated)
	float AimPitch;
};
