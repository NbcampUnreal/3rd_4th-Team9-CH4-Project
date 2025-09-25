#include "Game/OCGameState.h"
#include "Net/UnrealNetwork.h"
#include "OverClock.h"

AOCGameState::AOCGameState()
{
	WaitingMatchTime = 30;
	DuringMatchTime = 600;
}

void AOCGameState::HandleBeginPlay()
{
	OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	Super::HandleBeginPlay();

	OC_LOG(LogOCNetwork, Log, TEXT("End"));
}

void AOCGameState::OnRep_ReplicatedHasBegunPlay()
{
	OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	Super::OnRep_ReplicatedHasBegunPlay();

	OC_LOG(LogOCNetwork, Log, TEXT("End"));
}

void AOCGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();
}

void AOCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOCGameState, RemainingTime);
}

void AOCGameState::OnRep_RedTeamKillCount()
{

}

void AOCGameState::OnRep_BlueTeamKillCount()
{

}

void AOCGameState::OnRep_RedTeamMatchPoint()
{

}

void AOCGameState::OnRep_BlueTeamMatchPoint()
{

}

void AOCGameState::AddKillCount(int32 InTeamId)
{
	if (!GetNetDriver()->IsServer())
	{
		return;
	}

	ETeam Team = static_cast<ETeam>(InTeamId);
	if (Team == ETeam::ET_RedTeam)
	{
		++RedTeamKillCount;
	}
	else
	{
		++BlueTeamKillCount;
	}
}

int32 AOCGameState::GetKillCount(int32 InTeamId) const
{
	ETeam Team = static_cast<ETeam>(InTeamId);
	if (Team == ETeam::ET_RedTeam)
	{
		return RedTeamKillCount;
	}
	else
	{
		return BlueTeamKillCount;
	}
}

void AOCGameState::AddMatchCount(int32 InTeamId)
{
	if (!GetNetDriver()->IsServer())
	{
		return;
	}

	ETeam Team = static_cast<ETeam>(InTeamId);
	if (Team == ETeam::ET_RedTeam)
	{
		++RedTeamMatchPoint;
	}
	else
	{
		++BlueTeamMatchPoint;
	}
}

int32 AOCGameState::GetMatchCount(int32 InTeamId) const
{
	ETeam Team = static_cast<ETeam>(InTeamId);
	if (Team == ETeam::ET_RedTeam)
	{
		return RedTeamMatchPoint;
	}
	else
	{
		return BlueTeamMatchPoint;
	}
}
