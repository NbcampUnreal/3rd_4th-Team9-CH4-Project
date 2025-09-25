#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "MarkVisualActor.generated.h"

UCLASS()
class OVERCLOCK_API AMarkVisualActor : public AActor
{
	GENERATED_BODY()
	
public:    
	AMarkVisualActor();

	UFUNCTION(BlueprintCallable, Category = "Mark")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Mark")
	void RemoveMark();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MarkMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* MarkEffect;

	UPROPERTY(BlueprintReadWrite, Category = "Mark")
	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark Settings")
	FVector MarkOffset = FVector(0.0f, 0.0f, 250.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark Settings")
	float RotationSpeed = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark Settings")
	float BobAmplitude = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark Settings")
	float BobSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* DestroyEffect;

private:
	float BobTime = 0.0f;
	FVector BaseOffset;
};
