// Fill out your copyright notice in the Description page of Project Settings.


#include "GC/GC_Fire.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

bool UGC_Fire::OnExecute_Implementation(
	AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (!NiagaraSystem)
	{
		return false;
	}

	USkeletalMeshComponent* AttachComp = ResolveAttachMesh(Target, Parameters);
	if (!AttachComp)
	{
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

    if (FireSound)
    {
        UGameplayStatics::SpawnSoundAttached(
            FireSound,
            AttachComp,
            MuzzleSocket,                      // ���� ����
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            /*bStopWhenAttachedToDestroyed*/ true,
            VolumeMultiplier,
            PitchMultiplier,
            0.0f,                               // StartTime
            AttenuationSettings,
            ConcurrencySettings
        );
    }

	return true; // ����
}

// GC_Fire.cpp (�Ǵ� ���� GCN cpp) ��
USkeletalMeshComponent* UGC_Fire::ResolveAttachMesh(
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
