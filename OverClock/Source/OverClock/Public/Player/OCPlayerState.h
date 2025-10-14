#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerState.h"
#include "Interface/TeamInfoInterface.h"
#include "OCPlayerState.generated.h"

class UOCAttributeSet_Resources;
class UOCAttributeSet_Health;
class UOCPawnData;
class UOCAbilitySystemComponent;

UCLASS()
class OVERCLOCK_API AOCPlayerState : public APlayerState, public IAbilitySystemInterface, public ITeamInfoInterface
{
	GENERATED_BODY()

public:
	AOCPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void InitASCForAvatar(AActor* NewAvatar);

// TeamInfoInterface Section
public:
	virtual void SetInfo(int32 InTeamId) override;
	virtual class UUtilTeam* GetInfo() const override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UOCAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UOCAttributeSet_Health> HealthAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UOCAttributeSet_Resources> ResourcesAttributeSet;
	
	UPROPERTY(VisibleAnywhere, Transient, Category = Team)
	TObjectPtr<class UUtilTeam> UtilTeamInfo;
	
	bool bGrantedOnce = false;

public:
	FORCEINLINE UOCAbilitySystemComponent* GetOCASC() const { return AbilitySystemComponent; }
	FORCEINLINE UOCAttributeSet_Health* GetHealthAttributeSet() const { return HealthAttributeSet; }
	FORCEINLINE UOCAttributeSet_Resources* GetResourcesAttributeSet() const {return ResourcesAttributeSet;}
};
