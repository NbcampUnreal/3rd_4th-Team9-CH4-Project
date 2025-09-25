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

	return true; // 성공
}

// GC_RevFire.cpp (또는 너의 GCN cpp) 안
USkeletalMeshComponent* UGC_RevFire::ResolveAttachMesh(
    AActor* Target, const FGameplayCueParameters& Parameters) const
{
    // 1) GA에서 넘긴 SourceObject 우선
    if (const UObject* SO = Parameters.SourceObject.Get())
    {
        if (const USkeletalMeshComponent* FromSourceConst = Cast<const USkeletalMeshComponent>(SO))
        {
            if (MuzzleSocket.IsNone() || FromSourceConst->DoesSocketExist(MuzzleSocket))
            {
                return const_cast<USkeletalMeshComponent*>(FromSourceConst); // ★ 핵심
            }
        }
    }

    // 2) Target에서 소켓 있는 SkeletalMeshComponent 자동 탐색
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
