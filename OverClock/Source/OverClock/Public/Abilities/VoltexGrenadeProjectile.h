// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoltexGrenadeProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UFXSystemAsset; 
class USoundBase;
class UGameplayEffect;
class UNiagaraSystem;
class UParticleSystem;

UCLASS()
class OVERCLOCK_API AVoltexGrenadeProjectile : public AActor
{
	GENERATED_BODY()
	
public:
    AVoltexGrenadeProjectile();

    void InitVelocity(const FVector& Dir, float Speed);

protected:

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
    TSubclassOf<UGameplayEffect> DamageEffect;

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    TObjectPtr<USphereComponent> Collision;

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    TObjectPtr<UProjectileMovementComponent> Movement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|FX")
    TObjectPtr<UNiagaraSystem> ExplosionNiagara = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|FX")
    TObjectPtr<USoundBase> ExplosionSFX = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
    float ExplosionRadius = 300.f;

    bool bExploded = false;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade|FX")
    float ExplosionScale = 1.0f;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void ExplodeAt(const FVector& ImpactLoc);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ExplosionFX(const FVector_NetQuantize Location);
};
