// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "OCUIComponent.generated.h"


UCLASS( ClassGroup=(UI), meta=(BlueprintSpawnableComponent) )
class OVERCLOCK_API UOCUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOCUIComponent();

	UPROPERTY(EditDefaultsOnly, Category = "UI|Classes")
	TSubclassOf<UUserWidget> RootHUDClass;

	// 서브 위젯 3개
	UPROPERTY(EditDefaultsOnly, Category = "UI|Classes")
	TSubclassOf<UUserWidget> HealthWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Classes")
	TSubclassOf<UUserWidget> SkillAmmoWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Classes")
	TSubclassOf<UUserWidget> KillPointWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI|Slots")
	FName HealthSlotName = TEXT("HealthSlot");

	UPROPERTY(EditAnywhere, Category = "UI|Slots")
	FName SkillAmmoSlotName = TEXT("SkillAmmoSlot");

	UPROPERTY(EditAnywhere, Category = "UI|Slots")
	FName KillPointSlotName = TEXT("KillPointSlot");

	UPROPERTY(EditAnywhere, Category = "UI|ZOrder")
	int32 RootZOrder = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI|Cache")
	int32 CachedHealthCur = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI|Cache")
	int32 CachedHealthMax = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI|Cache")
	int32 CachedAmmoCur = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI|Cache")
	int32 CachedAmmoMax = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI|Cache")
	int32 CachedKillPoint = 0;

	// 수명/표시
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitializeForOwner();   // 로컬 소유 준비 후 1회 호출

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowUI();               // 화면에 붙이기(같은 인스턴스 재사용)

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideUI();               // 화면에서 떼기(인스턴스 유지)

	UFUNCTION(BlueprintPure, Category = "UI")
	bool IsShown() const;

	// 값 푸시(이벤트 기반 갱신)
	UFUNCTION(BlueprintCallable, Category = "UI|Push")
	void PushHealth(int32 Cur, int32 Max);

	UFUNCTION(BlueprintCallable, Category = "UI|Push")
	void PushAmmo(int32 Cur, int32 Max);

	UFUNCTION(BlueprintCallable, Category = "UI|Push")
	void PushKillPoint(int32 Value);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// 생성된 인스턴스들
	UPROPERTY() UUserWidget* RootHUD = nullptr;
	UPROPERTY() UUserWidget* HealthWidget = nullptr;
	UPROPERTY() UUserWidget* SkillAmmoWidget = nullptr;
	UPROPERTY() UUserWidget* KillPointWidget = nullptr;

	// 로컬 소유 컨트롤러
	TWeakObjectPtr<class APlayerController> CachedPC;

	// 내부 유틸
	bool ResolveLocalPC(class APlayerController*& OutPC) const;
	void EnsureRootAndChildren(class APlayerController* PC);
	void AttachChildToPanelByName(UUserWidget* Root, FName PanelName, UUserWidget* Child);
	void CallWidgetEvent(UUserWidget* Widget, FName FuncName, void* Params);
};
