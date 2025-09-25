#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OCMarkComponent.generated.h"

struct FGameplayTag;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OVERCLOCK_API UOCMarkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOCMarkComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark System")
	TSubclassOf<class AMarkVisualActor> MarkVisualActorClass;

private:
	void SetupGlobalASCEvents();
    
	UFUNCTION()
	void OnActorSpawned(AActor* SpawnedActor);
    
	void OnMarkTagChanged(const FGameplayTag CallbackTag, int32 NewCount, AActor* Actor);
    
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<class AMarkVisualActor>> ActiveMarks;
    
	FDelegateHandle ActorSpawnedHandle;
};