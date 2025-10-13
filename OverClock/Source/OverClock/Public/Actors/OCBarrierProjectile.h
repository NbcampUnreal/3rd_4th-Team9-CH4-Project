#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OCBarrierProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class AOCBarrierField;

UCLASS()
class OVERCLOCK_API AOCBarrierProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AOCBarrierProjectile();

	void Init(AController* InOwnerController, TSubclassOf<AOCBarrierField> InBarrierClass, float InBarrierRadius, float InBarrierHalfHeight, float InBarrierLifeSeconds, float InBarrierMaxHealth);
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(EditDefaultsOnly, Category="Barrier")
	TSubclassOf<AOCBarrierField> BarrierClass;

	UPROPERTY(Replicated)
	TObjectPtr<AController> OwnerController = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrier|Property")
	float BarrierRadius = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrier|Property")
	float BarrierHalfHeight = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrier|Property")
	float BarrierLifeSeconds = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Barrier|Property")
	float BarrierMaxHealth = 500.f;

};
