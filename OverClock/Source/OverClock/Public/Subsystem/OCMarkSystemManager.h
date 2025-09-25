#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OCMarkSystemManager.generated.h"

struct FGameplayTag;
class AMarkVisualActor;

UCLASS()
class OVERCLOCK_API UOCMarkSystemManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Mark System")
	void CreateMarkVisual(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Mark System")
	void RemoveMarkVisual(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark System")
	TSubclassOf<AMarkVisualActor> MarkVisualActorClass;

protected:
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<AMarkVisualActor>> ActiveMarks;

private:
	void BindGASEvents(AActor* Actor);
	//void UnbindGASEvents(AActor* Actor);
    
	UFUNCTION()
	void OnActorTagChanged(AActor* Actor, const FGameplayTag& Tag, int32 NewCount);
};
