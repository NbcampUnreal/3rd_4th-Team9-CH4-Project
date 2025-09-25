#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Interface/TeamInfoInterface.h"
#include "OCPlayerState.generated.h"

class UAbilitySystemComponent;

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

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Transient, Category = Team)
	TObjectPtr<class UUtilTeam> UtilTeamInfo;


	bool bGrantedOnce = false;
};
