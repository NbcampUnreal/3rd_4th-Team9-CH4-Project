#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "OCCharacterBase.generated.h"

class UGameplayAbility;
class UOCMarkComponent;
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
	float GetAimPitch() const;
	
	UFUNCTION(Server, Unreliable)
	void ServerSetAimPitch(float InAimPitch);
	
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

	void OnStunTagChanged(const FGameplayTag GameplayTag, int32 NewCount);

	void UpdateRotation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset")
	TObjectPtr<UOCAnimDataAsset> OCAnimDataAsset;

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadOnly, Category = "Character")
	FGameplayTag CharacterTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	TObjectPtr<UOCMarkComponent> MarkComp;
	
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
	float RotationTemp=0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> AbilityMapByTag;
};
