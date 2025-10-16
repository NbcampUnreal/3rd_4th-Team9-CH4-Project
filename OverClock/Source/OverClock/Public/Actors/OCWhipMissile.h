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
	
	virtual void UnInit() override;

protected:
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FTimerHandle EndTimerHandle;
	
public:	
	void SetVelocity(const FVector& NewVelocity);
};
