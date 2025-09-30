#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/Team.h"
#include "UtilTeam.generated.h"

UCLASS()
class OVERCLOCK_API UUtilTeam : public UObject
{
	GENERATED_BODY()
	
public:
	UUtilTeam();

	void Initialize(int32 InTeamId);

protected:
	ETeam Team;
	int32 TeamId;


// Getter, Setter Section
public:
	FORCEINLINE bool IsOpponent(ETeam InTeam) const { return InTeam != Team; }
	FORCEINLINE int32 GetTeamId() const { return TeamId; }
};
