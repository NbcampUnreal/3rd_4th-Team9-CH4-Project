#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "OCBarrierField.generated.h"

class USphereComponent;
class UOCAbilitySystemComponent;
class UOCAttributeSet_Health;
class UGameplayEffect;
class UParticleSystem;
class UParticleSystemComponent;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FOCBarrierPolicy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barrier|Policy")
	bool bAllowFriendly = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barrier|Policy")
	bool bBlockHitScan = true;
	
};

UCLASS()
class OVERCLOCK_API AOCBarrierField : public AActor
{
	GENERATED_BODY()
	
public:	
	AOCBarrierField();

	void InitOwnerController(AController* InOwnerController);
	void InitParams(float InRadius, float InHalfHeight, float InLifeSeconds, float InMaxHealth);

	UFUNCTION(BlueprintCallable, Category="Barrier")
	void ApplyHitscanImpact(float Damage, AActor* InstigatorActor, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category="Barrier")
	void ApplyAbilityImpact(float Damage, AActor* InstigatorActor, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category="Barrier")
	AController* GetOwnerControllerCached() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnBarrierBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void OnHealthChanged(const FOnAttributeChangeData& Data);

	void OnLifeExpired();

	void PauseEffect();

	bool IsFriendlyActor(const AActor* Other) const;

	void ApplyBarrierDamage(float Damage, AActor* DamageSource, const FHitResult& Where);

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier")
	TObjectPtr<USphereComponent> BarrierVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Barrier|VFX")
	TObjectPtr<UParticleSystemComponent> BarrierPSC;

#pragma endregion 

#pragma region Particle Effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier|VFX")
	TObjectPtr<UParticleSystem> BarrierEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier|VFX")
	float EffectUniformScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier|VFX")
	float FormationDelay = 0.25f;

#pragma endregion 

#pragma region Policy
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier")
	FOCBarrierPolicy BarrierPolicy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier")
	float Radius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier")
	float LifeSeconds = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier")
	float MaxHealth = 500.f;

#pragma endregion

#pragma region GAS
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier|GAS")
	TSubclassOf<UGameplayEffect> DamageTakenGE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier|GAS")
	FGameplayTag SetByCaller_DamageTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier|GAS")
	TSubclassOf<UGameplayEffect> InitHealthGE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier|GAS")
	TObjectPtr<UOCAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UOCAttributeSet_Health> HealthSet;

	UPROPERTY(Replicated)
	TObjectPtr<AController> OwnerController = nullptr;

#pragma endregion
	
	FTimerHandle TH_Life;
	FTimerHandle TH_Pause;
};
