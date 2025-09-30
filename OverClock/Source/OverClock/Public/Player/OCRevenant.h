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

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> DeadlyBulletAbilityClass;

	/** ��Ŭ�� GA Ŭ���� (BP_Peacekeeper ��) */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> PeacekeeperAbilityClass;

	/** �±ס�ִ� ���� �����Ϳ��� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
	TObjectPtr<UOCAnimDataAsset> AnimDataAsset = nullptr;

	/** �� ĳ������ Ÿ�� �±� (��: Character.Type.Revenant) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
	FGameplayTag CharacterTypeTag;

    /** ĳ���ͺ��� �����Ƽ�� �±׷� ����: ��Ʈ�ѷ��� �� �ʸ� ���� */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> AbilityMapByTag;

    /** �±׷� GA Ŭ������ ã�´�(������ nullptr) */
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    TSubclassOf<UGameplayAbility> GetAbilityClassByTag(FGameplayTag AbilityTag) const;

	// GA���� ����
	UFUNCTION(BlueprintCallable) FORCEINLINE UOCAnimDataAsset* GetAnimDataAsset() const { return AnimDataAsset; }
	UFUNCTION(BlueprintCallable) FORCEINLINE FGameplayTag GetCharacterTypeTag() const { return CharacterTypeTag; }

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	void GiveRevenantStartupAbilities();

public:
    // źâ
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

    // �� �� �Ҹ�(���� �� true)
    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    bool ConsumeAmmo(int32 Amount = 1);

    // ����(���� ä��)
    UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
    void RefillAmmo();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // ============================================================================ 
};
