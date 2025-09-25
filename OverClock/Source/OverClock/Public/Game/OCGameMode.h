// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Interface/OCGameInterface.h"
#include "OCGameMode.generated.h"

namespace MatchState
{
	extern OVERCLOCK_API const FName PreMatchCountdown;
}

UCLASS()
class OVERCLOCK_API AOCGameMode : public AGameMode, public IOCGameInterface
{
	GENERATED_BODY()

public:
	AOCGameMode();

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	virtual void OnMatchStateSet() override;

	virtual void HandleMatchHasWaitingCountdownEnded();

	virtual FTransform GetRespawnTransform() const override;
	virtual void OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn) override;

protected:
	virtual void PostInitializeComponents() override;


// Match State Machine Section
protected:
	FTimerHandle GameTimerHandle;

	void DefaultGameTimer();

	void MatchCountdown();
	void FinishMatch();

protected:
	TArray<TObjectPtr<class APlayerStart>> PlayerStartArray;


private:
	UPROPERTY(EditAnywhere, Category = PlayerCount, meta = (AllowPrivateAccess = "true"))
	int32 MaxPlayerCount;

	UPROPERTY(EditAnywhere, Category = Score, meta = (AllowPrivateAccess = "true"))
	int32 MatchScore;

	UPROPERTY(EditAnywhere, Category = Score, meta = (AllowPrivateAccess = "true"))
	int32 VictoryScore;

	bool IsFullPlayers();
	void AssignTeam();
	void ReassignTeam();

};
