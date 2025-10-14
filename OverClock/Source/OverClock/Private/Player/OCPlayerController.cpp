#include "Player/OCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Player/OCCharacterBase_V2.h"
#include "Data/DA_OCInputConfig.h"

// #include "InputActionValue.h"
// #include "GameplayTagContainer.h"
// #include "GameFramework/Pawn.h"
// #include "GameFramework/Character.h"
// #include "Player/OCCharacterBase.h"
// #include "Player/OCPlayerState.h"
// #include "GA/GA_RangedAttack.h"
// #include "Abilities/OCAbilityDataAsset.h"
// #include "GameFramework/InputSettings.h"
// #include "Player/OCRevenant.h"
// #include "OverClock.h"

AOCPlayerController::AOCPlayerController()
{
    bReplicates = true;
}

void AOCPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AOCPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    UInputMappingContext* IMC = ResolvePawnMappingContext(InPawn);
    
    if (!IMC && FallbackMappingContext)
    {
        IMC = FallbackMappingContext;
    }
    
    ApplyMappingContext(IMC,0);

	// PS(Owner)에 ASC가 붙어있다는 가정 → Avatar를 현재 Pawn으로 초기화
	// if (APlayerState* PS = GetPlayerState<APlayerState>())
	// {
	// 	 GetASC()->InitAbilityActorInfo(PS, InPawn);
	// }

	//GA 부여 로직
	// Ability Tag : Character Tag / AbilityStruct : Character의 GA Struct
	// if (AOCCharacterBase* C = Cast<AOCCharacterBase>(InPawn))
	// {
	// 	if (AbilityDataAsset)
	// 	{
	// 		AbilityStruct = AbilityDataAsset->CharacterAbilityTags[C->GetCurrentTag()];

	// 		return;
	// 	}
	// }
}

void AOCPlayerController::OnUnPossess()
{
    RemoveMappingContext();

    Super::OnUnPossess();
}


UInputMappingContext* AOCPlayerController::ResolvePawnMappingContext(APawn* InPawn) const
{
    if (!InPawn) return nullptr;
    
    if (const AOCCharacterBase_V2* V2 = Cast<AOCCharacterBase_V2>(InPawn))
    {
        if (const UDA_OCInputConfig* IC = V2->GetInputConfig())
        {
            return IC->DefaultMappingContext;
        }
    }
    
    return nullptr;
}

void AOCPlayerController::ApplyMappingContext(UInputMappingContext* IMC, int32 Priority)
{
    if (!IMC) return;
    
    if (AppliedMappingContext == IMC)
    {
        return;
    }
    
    RemoveMappingContext();

    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    UEnhancedInputLocalPlayerSubsystem* Subsys = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

    check(Subsys);
    
    Subsys->AddMappingContext(IMC, Priority);
    AppliedMappingContext = IMC;
}

void AOCPlayerController::RemoveMappingContext()
{
    if (!AppliedMappingContext) return;

    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    UEnhancedInputLocalPlayerSubsystem* Subsys = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

    check(Subsys);
    
    Subsys->RemoveMappingContext(AppliedMappingContext);
    AppliedMappingContext = nullptr;
}
