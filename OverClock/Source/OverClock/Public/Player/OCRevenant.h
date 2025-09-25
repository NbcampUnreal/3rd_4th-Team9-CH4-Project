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

	/** 좌클릭 GA 클래스 (BP_Peacekeeper 등) */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> PeacekeeperAbilityClass;

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

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	void GiveRevenantStartupAbilities();

public:
    // 탄창
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
    int32 MaxAmmo = 6;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    int32 CurrentAmmo = 0;

    UFUNCTION()
    void OnRep_CurrentAmmo();

    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    int32 GetAmmo() const { return CurrentAmmo; }

    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    bool IsAmmoEmpty() const { return CurrentAmmo <= 0; }

    // 한 발 소모(성공 시 true)
    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    bool ConsumeAmmo(int32 Amount = 1);

    // 장전(가득 채움)
    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    void RefillAmmo();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // ============================================================================ 
};
