#include "OverClockEditorModule.h"
#include "CSVToDataTableToolSubsystem.h"
#include "DataTableToolSettings.h"

#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "FPlayfabTestEditorModule"

IMPLEMENT_MODULE(FOverClockEditor, OverClockEditor);

void FOverClockEditor::StartupModule()
{
	ToolMenuExtensibilityManagerDelegateHandle = UToolMenus::RegisterStartupCallback(
		FSimpleDelegate::CreateRaw(this, &FOverClockEditor::RegisterMenus)
	);
}

void FOverClockEditor::ShutdownModule()
{
	UToolMenus::UnregisterOwner(TEXT("OverClockEditorModule"));

	if (ToolMenuExtensibilityManagerDelegateHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenuExtensibilityManagerDelegateHandle);
		ToolMenuExtensibilityManagerDelegateHandle.Reset();
	}
}

void FOverClockEditor::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(TEXT("OverClockEditorModule"));

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
		Section.AddSubMenu(
			"MyTool",
			LOCTEXT("MyToolMenuTitle", "My Tools"),
			LOCTEXT("MyToolsMenuTooltip", "Common custom editor tools for my project."),
			FNewMenuDelegate::CreateStatic(&FOverClockEditor::RegisterMyToolsMenu)
		);
	}
}

void FOverClockEditor::RegisterMyToolsMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString("Convert CSV to Data Tables"),
		LOCTEXT("ConvertCSVToDataTablesTooltip", "Converts all CSV files in the specified folder to Data Tables."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ImportAsset"),
		FUIAction(FExecuteAction::CreateStatic(&FOverClockEditor::OnConvertCSVToDataTablesClicked))
	);
}

void FOverClockEditor::OnConvertCSVToDataTablesClicked()
{
	UCSVToDataTableToolSubsystem* Subsystem = GEditor->GetEditorSubsystem<UCSVToDataTableToolSubsystem>();
	if (Subsystem)
	{
		Subsystem->ConvertAllCSVsToDataTables();
	}
	else
	{
		FNotificationInfo Info(LOCTEXT("SubsystemNotFound", "CSV to DataTable Tool: Subsystem not found or not initialized!"));
		Info.ExpireDuration = 5.f;
		Info.Image = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Notification.Fail").GetIcon();
		FSlateNotificationManager::Get().AddNotification(Info);
		UE_LOG(LogTemp, Error, TEXT("UCSVToDataTableToolSubsystem not found or not initialized!"));
	}
}
