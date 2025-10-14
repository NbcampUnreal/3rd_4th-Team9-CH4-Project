#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OCMarkComponent.generated.h"

class UNiagaraSystem;
struct FGameplayTag;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OVERCLOCK_API UOCMarkComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UOCMarkComponent();

	virtual void BeginPlay() override;

	void ASCBind();

	UFUNCTION()
	void OnTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
private:
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetMarkVisibility(bool InVis);
	   
	UPROPERTY(EditAnywhere, Category = "Positioning")
	FVector MarkOffset = FVector(0, 0, 120);
};