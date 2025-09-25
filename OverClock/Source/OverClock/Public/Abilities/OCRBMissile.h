#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OCRBMissile.generated.h"

class UGE_MarkEffect;
class USphereComponent;
class UProjectileMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileHit, AActor*, HitTarget, const FHitResult&, HitResult);

UCLASS()
class OVERCLOCK_API AOCRBMissile : public AActor
{
	GENERATED_BODY()
	
public:	
	AOCRBMissile();

	 void SetTarget(AActor* NewTarget);

	UPROPERTY()
	TWeakObjectPtr<AActor> NewTargetInTick;

	// 적중 브로드캐스트 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnProjectileHit FOnProHit;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mark")
	TSubclassOf<UGE_MarkEffect> MarkEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float LifeSpan = 30.0f;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			   FVector NormalImpulse, const FHitResult& Hit);

	void ApplyMarkToTarget(AActor* Target);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	// 타겟 > BP편집X
	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	TWeakObjectPtr<AActor> HomingTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float InitialSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float MaxSpeed = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float HomingAcceleration = 3000.0f;


	// 이펙트들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	UParticleSystemComponent* TrailEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* HitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundBase* LaunchSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundBase* HitSound;
	

public:	
	virtual void Tick(float DeltaTime) override;

};
