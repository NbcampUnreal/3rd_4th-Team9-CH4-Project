#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Game/Team.h"
#include "OCGameState.generated.h"

UCLASS()
class OVERCLOCK_API AOCGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AOCGameState();

	virtual void HandleBeginPlay() override;
	virtual void OnRep_ReplicatedHasBegunPlay() override;
	virtual void OnRep_MatchState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	UFUNCTION()
	void OnRep_RedTeamKillCount();
	UFUNCTION()
	void OnRep_BlueTeamKillCount();
	UFUNCTION()
	void OnRep_RedTeamMatchPoint();
	UFUNCTION()
	void OnRep_BlueTeamMatchPoint();

	UFUNCTION(BlueprintCallable)
	void AddKillCount(int32 InTeamId);
	UFUNCTION(BlueprintCallable)
	int32 GetKillCount(int32 InTeamId) const;
	UFUNCTION(BlueprintCallable)
	void AddMatchCount(int32 InTeamId);
	UFUNCTION(BlueprintCallable)
	int32 GetMatchCount(int32 InTeamId) const;

protected:

	UPROPERTY(Transient, ReplicatedUsing = OnRep_RedTeamKillCount)
	int32 RedTeamKillCount;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BlueTeamKillCount)
	int32 BlueTeamKillCount;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_RedTeamMatchPoint)
	int32 RedTeamMatchPoint;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BlueTeamMatchPoint)
	int32 BlueTeamMatchPoint;

	int32 WaitingMatchTime;
	int32 DuringMatchTime;
	int32 OverTimeToGetMatch;

// Getter, Setter Section
public:
	FORCEINLINE int32 GetWaitingMatchTime() const { return WaitingMatchTime; }
	FORCEINLINE int32 GetDuringMatchTime() const { return DuringMatchTime; }
	FORCEINLINE int32 GetOverTimeToGetMatch() const { return OverTimeToGetMatch; }

	FORCEINLINE int32 GetWinningTeamId() const { return RedTeamKillCount > BlueTeamKillCount ? static_cast<int32>(ETeam::ET_RedTeam) : RedTeamKillCount != BlueTeamKillCount ? static_cast<int32>(ETeam::ET_BlueTeam) : -1; }
	FORCEINLINE int32 GetOverMatchCount(int32& OutTeamId) const 
	{
		if (RedTeamMatchPoint > BlueTeamMatchPoint)
		{
			OutTeamId = static_cast<int32>(ETeam::ET_RedTeam);

			return RedTeamMatchPoint;
		}
		else
		{
			OutTeamId = static_cast<int32>(ETeam::ET_BlueTeam);;

			return BlueTeamMatchPoint;
		}
	}
};
