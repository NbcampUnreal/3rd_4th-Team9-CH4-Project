#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OCPoisonField.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class USphereComponent;

UCLASS()
class OVERCLOCK_API AOCPoisonField : public AActor
{
	GENERATED_BODY()
public:
	AOCPoisonField();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> FieldMesh;

	UPROPERTY(EditDefaultsOnly)
	float Duration = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float TickInterval = 1.0f;

	// 틱당 데미지
	UPROPERTY(EditDefaultsOnly, Category = "Poison")
	float DamagePerTick = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StunEffect;

	FTimerHandle TickTimer;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;
	
	float DeltaSum;

	UFUNCTION()
	void ApplyDamage();

private:
	// 장판 내부에 있는 대상들의 ASC
	UPROPERTY()
	TArray<UAbilitySystemComponent*> OverlappingASCs;

	// 적 판별
	bool IsTeam(AActor* TargetActor) const;
};
