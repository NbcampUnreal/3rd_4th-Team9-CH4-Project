#include "Game/OCGameMode.h"
#include "Game/OCGameState.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Util/UtilTeam.h"
#include "Interface/TeamInfoInterface.h"
#include "OverClock.h"

namespace MatchState
{
	const FName PreMatchCountdown = FName(TEXT("PreMatchCountdown"));
}

AOCGameMode::AOCGameMode()
{
	VictoryScore = 10;
}

void AOCGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	//OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (IsFullPlayers())
	{
		ErrorMessage = TEXT("Join Full Players.");

		return;
	}

	//OC_LOG(LogOCNetwork, Log, TEXT("End"));
}

APlayerController* AOCGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	APlayerController* LoginController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	OC_LOG(LogOCNetwork, Log, TEXT("End"));

	return LoginController;
}

void AOCGameMode::PostLogin(APlayerController* NewPlayer)
{
	//OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	Super::PostLogin(NewPlayer);

	AssignTeam();

	//OC_LOG(LogOCNetwork, Log, TEXT("End"));
}

void AOCGameMode::StartPlay()
{
	OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	Super::StartPlay();

	OC_LOG(LogOCNetwork, Log, TEXT("End"));
}

void AOCGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	if (MatchState == MatchState::PreMatchCountdown)
	{
		HandleMatchHasWaitingCountdownEnded();
	}
}

void AOCGameMode::HandleMatchHasWaitingCountdownEnded()
{

}

FTransform AOCGameMode::GetRespawnTransform() const
{
	if (PlayerStartArray.Num() == 0)
	{
		return FTransform(FVector(0.f, 0.f, 230.f));
	}

	int32 RandomIndex = FMath::RandRange(0, PlayerStartArray.Num() - 1);
	return PlayerStartArray[RandomIndex]->GetActorTransform();
}

void AOCGameMode::PostInitializeComponents()
{
	OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	Super::PostInitializeComponents();

	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AOCGameMode::DefaultGameTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);

	OC_LOG(LogOCNetwork, Log, TEXT("End"));
}

void AOCGameMode::OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn)
{
	OC_LOG(LogOCNetwork, Log, TEXT("Begin"));

	APlayerState* KillerPlayerState = Killer->PlayerState;
	check(KillerPlayerState);

	AOCGameState* OCGameState = GetGameState<AOCGameState>();
	check(OCGameState);

	KillerPlayerState->SetScore(KillerPlayerState->GetScore() + 1);
	ITeamInfoInterface* TeamInfoInterface = Cast<ITeamInfoInterface>(KillerPlayerState);
	check(TeamInfoInterface);

	int32 TeamId = TeamInfoInterface->GetInfo()->GetTeamId();
	OCGameState->AddKillCount(TeamId);

	int32 KillCount = OCGameState->GetKillCount(TeamId);
	if (KillCount != MatchScore)
	{
		return;
	}

	OCGameState->AddMatchCount(TeamId);

	int32 MatchCount = OCGameState->GetMatchCount(TeamId);
	if (MatchCount != VictoryScore)
	{
		ReassignTeam();
		MatchCountdown();

		return;
	}

	FinishMatch();
}

void AOCGameMode::DefaultGameTimer()
{
	AOCGameState* const OCGameState = Cast<AOCGameState>(GameState);

	if (OCGameState && OCGameState->RemainingTime > 0)
	{
		--OCGameState->RemainingTime;
		if (OCGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::PreMatchCountdown)
			{
				StartMatch();
			}
			else if (GetMatchState() == MatchState::InProgress)
			{
				int32 WinningTeamId = OCGameState->GetWinningTeamId();
				if (WinningTeamId > 0)
				{
					OCGameState->AddMatchCount(WinningTeamId);
				}
				else
				{
					OCGameState->RemainingTime = OCGameState->GetOverTimeToGetMatch();
				}

				int32 OutTeamId;
				int32 OverMatchCount = OCGameState->GetOverMatchCount(OutTeamId);
				if (OverMatchCount != VictoryScore)
				{
					ReassignTeam();
					MatchCountdown();
				}
				else
				{
					FinishMatch();
				}
			}
		}
	}
}

void AOCGameMode::MatchCountdown()
{
	AOCGameState* const OCGameState = Cast<AOCGameState>(GameState);
	if (IsMatchInProgress())
	{
		SetMatchState(MatchState::PreMatchCountdown);
		OCGameState->RemainingTime = OCGameState->GetWaitingMatchTime();
	}
}

void AOCGameMode::FinishMatch()
{
	AOCGameState* const OCGameState = Cast<AOCGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();
	}
}

bool AOCGameMode::IsFullPlayers()
{
	return GetNumPlayers() == MaxPlayerCount;
}

void AOCGameMode::AssignTeam()
{
	if (IsFullPlayers())
	{
		int32 TeamID = 0;
		for (auto PS : GameState->PlayerArray)
		{
			ITeamInfoInterface* TeamInfo = Cast<ITeamInfoInterface>(PS);
			if (TeamInfo)
			{
				int32 RealTeamID = TeamID % 2;
				TeamInfo->SetInfo(RealTeamID);

				OC_LOG(LogOCNetwork, Log, TEXT("Player %s assinged to Team %d"), *PS->GetPlayerName(), RealTeamID);

				TeamID++;
			}
		}
	}
}

void AOCGameMode::ReassignTeam()
{
	for (auto PS : GameState->PlayerArray)
	{
		ITeamInfoInterface* TeamInfo = Cast<ITeamInfoInterface>(PS);
		if (TeamInfo)
		{
			int32 CurrentTeamID = TeamInfo->GetInfo()->GetTeamId();
			TeamInfo->SetInfo(CurrentTeamID ^ 1);
		}
	}
}
