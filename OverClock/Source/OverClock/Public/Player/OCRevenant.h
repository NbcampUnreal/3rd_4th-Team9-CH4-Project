// OCRevenant.h
#pragma once

#include "CoreMinimal.h"
#include "Player/OCCharacterBase.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "OCRevenant.generated.h"

class UOCAnimDataAsset;

UCLASS()
class OVERCLOCK_API AOCRevenant : public AOCCharacterBase
{
	GENERATED_BODY()
public:
	AOCRevenant();

	/** 태그→애님 매핑 데이터에셋 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
	TObjectPtr<UOCAnimDataAsset> AnimDataAsset = nullptr;

	/** 이 캐릭터의 타입 태그 (예: Character.Type.Revenant) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
	FGameplayTag CharacterTypeTag;

    /** 캐릭터별로 어빌리티를 태그로 선언: 컨트롤러는 이 맵만 참조 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> AbilityMapByTag;

    /** 태그로 GA 클래스를 찾는다(없으면 nullptr) */
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    TSubclassOf<UGameplayAbility> GetAbilityClassByTag(FGameplayTag AbilityTag) const;

	// GA에서 접근
	UFUNCTION(BlueprintCallable) FORCEINLINE UOCAnimDataAsset* GetAnimDataAsset() const { return AnimDataAsset; }
	UFUNCTION(BlueprintCallable) FORCEINLINE FGameplayTag GetCharacterTypeTag() const { return CharacterTypeTag; }

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayMontage(class UAnimMontage* Montage, float InPlayRate, FName InSection);
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayReloadByAsset(class UAnimSequenceBase* ReloadSource, FName SlotName, float InPlayRate, FName Section);
    UFUNCTION(Server, Reliable)
    void Server_RequestReload();

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	void GiveStartupAbilities();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
