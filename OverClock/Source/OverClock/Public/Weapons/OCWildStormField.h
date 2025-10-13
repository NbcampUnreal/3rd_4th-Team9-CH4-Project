#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OCWildStormField.generated.h"

class UNiagaraComponent;
class USphereComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
struct FGameplayTag;

UCLASS()
class OVERCLOCK_API AOCWildStormField : public AActor
{
	GENERATED_BODY()
	
public:	
	AOCWildStormField();

	UFUNCTION()
	void ApplyGE();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* NiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float LifeTime;

	FGameplayTag GetTeamTag(UAbilitySystemComponent* ASC);
	
	FTimerHandle GETimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> HealGE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DamageGE;

public:	
	virtual void Tick(float DeltaTime) override;
};
