#include "Component/OCUIComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"

UOCUIComponent::UOCUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UOCUIComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UOCUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HideUI();
	RootHUD = nullptr;
	HealthWidget = nullptr;
	SkillAmmoWidget = nullptr;
	KillPointWidget = nullptr;
	CachedPC = nullptr;

	Super::EndPlay(EndPlayReason);
}

bool UOCUIComponent::ResolveLocalPC(APlayerController*& OutPC) const
{
	OutPC = nullptr;

	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return false;

	if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
	{
		if (PC->IsLocalPlayerController())
		{
			OutPC = PC;
			return true;
		}
	}
	return false;
}

void UOCUIComponent::InitializeForOwner()
{
	APlayerController* PC = nullptr;
	if (!ResolveLocalPC(PC))
	{
		return; // 아직 로컬 포제션 전일 수 있음
	}
	CachedPC = PC;

	EnsureRootAndChildren(PC);

	// 초기 캐시 푸시
	PushHealth(CachedHealthCur, CachedHealthMax);
	PushAmmo(CachedAmmoCur, CachedAmmoMax);
	PushKillPoint(CachedKillPoint);
}

void UOCUIComponent::EnsureRootAndChildren(APlayerController* PC)
{
	if (RootHUDClass && !RootHUD)
	{
		RootHUD = CreateWidget<UUserWidget>(PC, RootHUDClass);
	}

	if (HealthWidgetClass && !HealthWidget)
		HealthWidget = CreateWidget<UUserWidget>(PC, HealthWidgetClass);

	if (SkillAmmoWidgetClass && !SkillAmmoWidget)
		SkillAmmoWidget = CreateWidget<UUserWidget>(PC, SkillAmmoWidgetClass);

	if (KillPointWidgetClass && !KillPointWidget)
		KillPointWidget = CreateWidget<UUserWidget>(PC, KillPointWidgetClass);

	if (RootHUD)
	{
		AttachChildToPanelByName(RootHUD, HealthSlotName, HealthWidget);
		AttachChildToPanelByName(RootHUD, SkillAmmoSlotName, SkillAmmoWidget);
		AttachChildToPanelByName(RootHUD, KillPointSlotName, KillPointWidget);
	}
}

void UOCUIComponent::AttachChildToPanelByName(UUserWidget* Root, FName PanelName, UUserWidget* Child)
{
	if (!Root || !Child || PanelName.IsNone()) return;

	if (UWidgetTree* WT = Root->WidgetTree)
	{
		if (UPanelWidget* Panel = Cast<UPanelWidget>(WT->FindWidget(PanelName)))
		{
			Child->RemoveFromParent();

			const int32 C = Panel->GetChildrenCount();
			for (int32 i = 0; i < C; ++i)
				if (Panel->GetChildAt(i) == Child) return;

			Panel->AddChild(Child);
		}
	}
}

void UOCUIComponent::ShowUI()
{
	APlayerController* PC = CachedPC.Get();

	if (!PC && !ResolveLocalPC(PC)) return;

	CachedPC = PC;

	EnsureRootAndChildren(PC);

	if (RootHUD && !RootHUD->IsInViewport())
	{
		RootHUD->AddToViewport(RootZOrder);
	}
}

void UOCUIComponent::HideUI()
{
	if (RootHUD) 
	{ 
		RootHUD->RemoveFromParent(); 
	}
}

bool UOCUIComponent::IsShown() const
{
	return RootHUD && RootHUD->IsInViewport();
}

void UOCUIComponent::PushHealth(int32 Cur, int32 Max)
{
	CachedHealthCur = Cur;
	CachedHealthMax = Max;

	if (HealthWidget)
	{
		struct { int32 CurParam; int32 MaxParam; } Params{ Cur, Max };
		CallWidgetEvent(HealthWidget, FName(TEXT("OnHealthChanged")), &Params);
	}
}

void UOCUIComponent::PushAmmo(int32 Cur, int32 Max)
{
	CachedAmmoCur = Cur;
	CachedAmmoMax = Max;

	if (SkillAmmoWidget)
	{
		struct { int32 CurParam; int32 MaxParam; } Params{ Cur, Max };
		CallWidgetEvent(SkillAmmoWidget, FName(TEXT("OnAmmoChanged")), &Params);
	}
}

void UOCUIComponent::PushKillPoint(int32 Value)
{
	CachedKillPoint = Value;

	if (KillPointWidget)
	{
		struct { int32 V; } Params{ Value };
		CallWidgetEvent(KillPointWidget, FName(TEXT("OnKillPointChanged")), &Params);
	}
}

void UOCUIComponent::CallWidgetEvent(UUserWidget* Widget, FName FuncName, void* Params)
{
	if (!Widget) return;
	if (UFunction* Fn = Widget->FindFunction(FuncName))
	{
		Widget->ProcessEvent(Fn, Params);
	}
}