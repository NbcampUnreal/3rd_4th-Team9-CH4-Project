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
	/** ���� Niagara �ý��� (BP �Ļ����� �����ص� ��) */
	UPROPERTY(EditDefaultsOnly, Category = "MuzzleFX")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

	/** ���� ���ϸ� (ĳ����/���� Mesh�� �����ؾ� ��) */
	UPROPERTY(EditDefaultsOnly, Category = "MuzzleFX")
	FName MuzzleSocket = FName(TEXT("Muzzle"));

	/** SourceObject�� ���� ����, ������ Target���� �ڵ� Ž�� */
	virtual bool OnExecute_Implementation(
		AActor* Target,
		const FGameplayCueParameters& Parameters) const override;

private:
	USkeletalMeshComponent* ResolveAttachMesh(AActor* Target, const FGameplayCueParameters& Parameters) const;
};
