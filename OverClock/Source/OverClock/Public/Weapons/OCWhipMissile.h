#pragma once

#include "CoreMinimal.h"
#include "OCMissileBase.h"
#include "OCWhipMissile.generated.h"

UCLASS()
class OVERCLOCK_API AOCWhipMissile : public AOCMissileBase
{
	GENERATED_BODY()
	
public:	
	AOCWhipMissile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float Damage = 20.0f;

	virtual void Init() override;

protected:
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

public:	
	void SetVelocity(const FVector& NewVelocity);
};
