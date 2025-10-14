#include "GC/GC_Reload.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

bool UGC_Reload::OnExecute_Implementation(
    AActor* Target, const FGameplayCueParameters& Parameters) const
{
    USkeletalMeshComponent* AttachComp = ResolveAttachMesh(Target, Parameters);
    if (!AttachComp)
    {
        return false;
    }

    if (ReloadSound)
    {
        UGameplayStatics::SpawnSoundAttached(
            ReloadSound,
            AttachComp,
            AttachSocket,                      // 소켓 기준
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

    return true; // 성공
}

// GC_RevFire.cpp (또는 너의 GCN cpp) 안
USkeletalMeshComponent* UGC_Reload::ResolveAttachMesh(
    AActor* Target, const FGameplayCueParameters& Parameters) const
{
    // 1) GA에서 넘긴 SourceObject 우선
    if (const UObject* SO = Parameters.SourceObject.Get())
    {
        if (const USkeletalMeshComponent* FromSourceConst = Cast<const USkeletalMeshComponent>(SO))
        {
            if (AttachSocket.IsNone() || FromSourceConst->DoesSocketExist(AttachSocket))
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
            if (IsValid(Sk) && (AttachSocket.IsNone() || Sk->DoesSocketExist(AttachSocket)))
            {
                return Sk;
            }
        }
    }
    return nullptr;
}