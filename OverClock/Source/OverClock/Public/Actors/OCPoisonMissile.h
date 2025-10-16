// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/OCMissileBase.h"
#include "OCPoisonMissile.generated.h"

class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class OVERCLOCK_API AOCPoisonMissile : public AOCMissileBase
{
	GENERATED_BODY()
	
public:
	AOCPoisonMissile();

	UPROPERTY(EditAnywhere, Category="Effects")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float Damage = 20.0f;

protected:
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class AOCPoisonField> PoisonFieldClass;

public:	
	void SetVelocity(const FVector& NewVelocity);
};
