#include "Test/TestActor.h"
#include "OCGameInstance.h"
#include "Subsystem/OCDataTableSubsystem.h"
#include "Data/GameDataType.h"
#include "Data/SampleDataStruct.h"

ATestActor::ATestActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATestActor::BeginPlay()
{
	Super::BeginPlay();
	
	UOCDataTableSubsystem* OCDataTableSubsystem = GetGameInstance()->GetSubsystem<UOCDataTableSubsystem>();

	FSampleDataStruct* SampleData = OCDataTableSubsystem->GetData<FSampleDataStruct>(EGameDataType::SampleData, 1);

	UE_LOG(LogTemp, Log, TEXT("Index : %d"), SampleData->Index);
	UE_LOG(LogTemp, Log, TEXT("Test Sample Data Index : %d"), SampleData->TestSampleData.Index);
	UE_LOG(LogTemp, Log, TEXT("Test Array Sample Data Num : %d"), SampleData->TestSampleDataArray.Num());
}

