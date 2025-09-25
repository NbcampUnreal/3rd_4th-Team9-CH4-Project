// Fill out your copyright notice in the Description page of Project Settings.


#include "GC/GC_RevFire.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

bool UGC_RevFire::OnExecute_Implementation(
	AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (!NiagaraSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GC_RevFire] NiagaraSystem is null"));
		return false;
	}

	USkeletalMeshComponent* AttachComp = ResolveAttachMesh(Target, Parameters);
	if (!AttachComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GC_RevFire] No mesh to attach (Target=%s)"), *GetNameSafe(Target));
		return false;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		NiagaraSystem,
		AttachComp,
		MuzzleSocket,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		/*bAutoDestroy*/ true);

	return true; // ����
}

// GC_RevFire.cpp (�Ǵ� ���� GCN cpp) ��
USkeletalMeshComponent* UGC_RevFire::ResolveAttachMesh(
    AActor* Target, const FGameplayCueParameters& Parameters) const
{
    // 1) GA���� �ѱ� SourceObject �켱
    if (const UObject* SO = Parameters.SourceObject.Get())
    {
        if (const USkeletalMeshComponent* FromSourceConst = Cast<const USkeletalMeshComponent>(SO))
        {
            if (MuzzleSocket.IsNone() || FromSourceConst->DoesSocketExist(MuzzleSocket))
            {
                return const_cast<USkeletalMeshComponent*>(FromSourceConst); // �� �ٽ�
            }
        }
    }

    // 2) Target���� ���� �ִ� SkeletalMeshComponent �ڵ� Ž��
    if (Target)
    {
        TArray<USkeletalMeshComponent*> Skels;
        Target->GetComponents(Skels);
        for (USkeletalMeshComponent* Sk : Skels)
        {
            if (IsValid(Sk) && (MuzzleSocket.IsNone() || Sk->DoesSocketExist(MuzzleSocket)))
            {
                return Sk;
            }
        }
    }
    return nullptr;
}
