#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OCMissileBase.generated.h"

class UGameplayEffect;
class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class OVERCLOCK_API AOCMissileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AOCMissileBase();

	UPROPERTY(EditDefaultsOnly, Category = "GameEffect")
	TSubclassOf<UGameplayEffect> GameEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Life")
	float LifeSpan = 30.0f;
	
protected:
	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		   FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float InitialSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float MaxSpeed = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<USoundBase> LaunchSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<USoundBase> HitSound;

};
