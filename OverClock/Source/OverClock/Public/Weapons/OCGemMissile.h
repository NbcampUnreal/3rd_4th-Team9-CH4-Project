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

	virtual void Init() override;
	virtual void UnInit() override;

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FTimerHandle EndTimerHandle;

public:	
	void SetVelocity(const FVector& NewVelocity);
};
