#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "OCMissileBase.h"
#include "OCRBMissile.generated.h"

UCLASS()
class OVERCLOCK_API AOCRBMissile : public AOCMissileBase
{
	GENERATED_BODY()
	
public:	
	AOCRBMissile();

	void SetTarget(AActor* NewTarget);

	UPROPERTY()
	TWeakObjectPtr<AActor> NewTargetInTick;

	virtual void PostInitializeComponents() override;

protected:

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			   FVector NormalImpulse, const FHitResult& Hit) override;

	void ApplyMarkToTarget(UAbilitySystemComponent* InOwnerASC, UAbilitySystemComponent* InTargetASC);

	// 타겟 > BP편집X
	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	TWeakObjectPtr<AActor> HomingTarget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float HomingAcceleration = 3000.0f;

	// 이펙트들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> TrailEffect;

};
