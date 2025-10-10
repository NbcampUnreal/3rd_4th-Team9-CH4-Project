#include "Player/OCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Player/OCCharacterBase_V2.h"
#include "Data/DA_OCInputConfig.h"

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
