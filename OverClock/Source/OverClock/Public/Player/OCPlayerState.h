#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerState.h"
#include "OCPlayerState.generated.h"

class UOCPawnData;
class UAbilitySystemComponent;

UCLASS()
class OVERCLOCK_API AOCPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOCPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void InitASCForAvatar(AActor* NewAvatar);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	bool bGrantedOnce = false;
};
