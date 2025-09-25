#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "OCCharacterBase.generated.h"

class UOCAnimDataAsset;
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
	FORCEINLINE FRotator GetAimRotation() const {return AimRotation;}
	
	UFUNCTION(Server, Unreliable)
	void ServerSetAimRotation(FRotator InAimRotation);
	
	UFUNCTION()
	FORCEINLINE FGameplayTag GetCurrentTag() const {return CharacterTag;};
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataAsset")
	TObjectPtr<UOCAnimDataAsset> OCAnimDataAsset;

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadOnly, Category = "Character")
	FGameplayTag CharacterTag;//network
	
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
	FRotator AimRotation;
};
