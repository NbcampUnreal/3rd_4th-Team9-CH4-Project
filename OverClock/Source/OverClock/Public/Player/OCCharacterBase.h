#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "OCCharacterBase.generated.h"

class UOCAnimDataAsset;
class UCameraComponent;
class UAbilitySystemComponent;
class AOCPlayerState;
class USpringArmComponent;

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
	FORCEINLINE FRotator GetAimRotation() const { return AimRotation; }

	UFUNCTION(Server, Unreliable)
	void ServerSetAimRotation(FRotator InAimRotation);

	UFUNCTION()
	FORCEINLINE FGameplayTag GetCurrentTag() const { return CharacterTag; }

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	TSubclassOf<UGameplayAbility> GetAbilityClassByTag(FGameplayTag AbilityTag) const;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	virtual void GiveStartupAbilities();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(class UAnimMontage* Montage, float InPlayRate, FName InSection);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySequenceAsDynamicMontage(class UAnimSequenceBase* Source, FName SlotName, float InPlayRate, FName Section);

	UFUNCTION(BlueprintCallable) 
	FORCEINLINE UOCAnimDataAsset* GetAnimDataAsset() const { return OCAnimDataAsset; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset")
	TObjectPtr<UOCAnimDataAsset> OCAnimDataAsset;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Character")
	FGameplayTag CharacterTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float JumpVelocity;

	UPROPERTY(Replicated)
	FRotator AimRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> AbilityMapByTag;
};
