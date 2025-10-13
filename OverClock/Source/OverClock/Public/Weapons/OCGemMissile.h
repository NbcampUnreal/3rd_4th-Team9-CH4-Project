#pragma once

#include "CoreMinimal.h"
#include "Weapons/OCMissileBase.h"
#include "OCGemMissile.generated.h"

class UNiagaraComponent;

UCLASS()
class OVERCLOCK_API AOCGemMissile : public AOCMissileBase
{
	GENERATED_BODY()

public:
	AOCGemMissile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraComponent> NiagaraComp;

protected:
	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

public:	
	void SetVelocity(const FVector& NewVelocity);
};
