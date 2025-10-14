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

	// 태그 변화
	UFUNCTION()
	void OnTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetMarkVisibility(bool InVis);
	
	// 상태별 표시 컴포넌트들
	//UPROPERTY(VisibleAnywhere, Category = "Status Effects")
	//TObjectPtr<UStaticMeshComponent> MarkStaticMesh;
    
	//UPROPERTY(VisibleAnywhere, Category = "Status Effects")
	//TObjectPtr<UNiagaraSystem> MarkEffect;
    
	// 애니메이션용
	float VerTime = 0.0f;
    
	UPROPERTY(EditAnywhere, Category = "Animation")
	float BobSpeed = 2.0f;
    
	UPROPERTY(EditAnywhere, Category = "Animation")
	float BobAmplitude = 10.0f;
    
	UPROPERTY(EditAnywhere, Category = "Positioning")
	FVector MarkOffset = FVector(0, 0, 120);
	
};