// #pragma once
//
// #include "CoreMinimal.h"
// #include "Engine/DataAsset.h"
// #include "Abilities/GameplayAbility.h"
// #include "GameplayAbilitySpec.h"
//
// #include "OCAbilitySet.generated.h"
//
// USTRUCT(BlueprintType)
// struct FOCData_GA
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditDefaultsOnly)
// 	TSubclassOf<UGameplayAbility> Ability = nullptr;
//
// 	UPROPERTY(EditDefaultsOnly)
// 	FGameplayTag InputTag;
// };
//
// USTRUCT(BlueprintType)
// struct FOCAblitySet_GrantedHandles
// {
// 	GENERATED_BODY()
//
// 	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
// 	void TakeFromAbilitySystem(UAbilitySystemComponent* InASC);
// protected:
//
// 	//여기다가 GA Spec의 Handle을 넣어서 관리하는 것
// 	UPROPERTY()
// 	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
// };
//
// UCLASS()
// class OVERCLOCK_API UOCAbilitySet : public UPrimaryDataAsset
// {
// 	GENERATED_BODY()
// 	
// public:
// 	UOCAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
// 	void GiveToAbilitySystem
// 			(UAbilitySystemComponent* ASC, FOCAblitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject);
//
// 	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
// 	TArray<FOCData_GA> GrantedGameplayAbilities;
// };
