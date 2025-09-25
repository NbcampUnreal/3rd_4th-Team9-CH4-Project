#include "Util/UtilTeam.h"

UUtilTeam::UUtilTeam()
{

}

void UUtilTeam::Initialize(int32 InTeamId)
{
	this->TeamId = InTeamId;
	this->Team = static_cast<ETeam>(InTeamId);
}
