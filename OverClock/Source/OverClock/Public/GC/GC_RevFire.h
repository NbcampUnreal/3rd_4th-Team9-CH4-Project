#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Burst.h"
#include "GC_RevFire.generated.h"

class UNiagaraSystem;
class USkeletalMeshComponent;
 
UCLASS()
class OVERCLOCK_API UGC_RevFire : public UGameplayCueNotify_Burst
{
	GENERATED_BODY()
	
public:
	/** 붙일 Niagara 시스템 (BP 파생에서 지정해도 됨) */
	UPROPERTY(EditDefaultsOnly, Category = "MuzzleFX")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

	/** 붙일 소켓명 (캐릭터/무기 Mesh에 존재해야 함) */
	UPROPERTY(EditDefaultsOnly, Category = "MuzzleFX")
	FName MuzzleSocket = FName(TEXT("Muzzle"));

	/** SourceObject를 먼저 쓰고, 없으면 Target에서 자동 탐색 */
	virtual bool OnExecute_Implementation(
		AActor* Target,
		const FGameplayCueParameters& Parameters) const override;

private:
	USkeletalMeshComponent* ResolveAttachMesh(AActor* Target, const FGameplayCueParameters& Parameters) const;
};
