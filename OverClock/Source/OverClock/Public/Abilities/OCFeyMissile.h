#pragma once

#include "CoreMinimal.h"
#include "OCMissileBase.h"
#include "OCFeyMissile.generated.h"

UCLASS()
class OVERCLOCK_API AOCFeyMissile : public AOCMissileBase
{
	GENERATED_BODY()
	
public:	
	AOCFeyMissile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float Damage = 20.0f;

protected:
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

public:	
	virtual void Tick(float DeltaTime) override;

};
