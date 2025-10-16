#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PooledInterface.h"
#include "OCMissileBase.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;
struct FGameplayTag;

UCLASS()
class OVERCLOCK_API AOCMissileBase : public AActor, public IPooledInterface
{
	GENERATED_BODY()
	
public:	
	AOCMissileBase();

	UPROPERTY(EditDefaultsOnly, Category = "GameEffect")
	TSubclassOf<UGameplayEffect> GameEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Life")
	float LifeSpan;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
protected:
	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		   FVector NormalImpulse, const FHitResult& Hit) {};

	virtual FGameplayTag GetTeamTag(AActor* TargetActor);

	TObjectPtr<UAbilitySystemComponent> GetASC(AActor* TargetActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float InitialSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float MaxSpeed = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<USoundBase> LaunchSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<USoundBase> HitSound;

	bool IsFirst=true;
	bool GetIsFirst() const { return IsFirst; }
	
public:
	virtual void Alloc() override;
	virtual void Init() override;//사용할 때 속도 방향 구현
	virtual void UnInit() override;//사용이 끝나면 호출
	virtual void Release() override;
};
