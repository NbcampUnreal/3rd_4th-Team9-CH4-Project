#include "Subsystem/OCDataTableSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Data/GameDataType.h"

UOCDataTableSubsystem::UOCDataTableSubsystem()
{

}

void UOCDataTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AddDataTable(EGameDataType::SampleData, FString(TEXT("SampleData")));
}

UCacheDataTable* UOCDataTableSubsystem::GetTable(EGameDataType InDataType)
{
	TSoftObjectPtr<UCacheDataTable>& CacheDataTable = DataTables[InDataType];
	if (!CacheDataTable)
	{
		return nullptr;
	}

	if (CacheDataTable.IsPending())
	{
		CacheDataTable.LoadSynchronous();
	}

	return CacheDataTable.Get();
}

void UOCDataTableSubsystem::AddDataTable(EGameDataType InType, const FString& TableName)
{
	UCacheDataTable* CacheTable = NewObject<UCacheDataTable>(this);
	class UDataTable* DataTable = Cast<class UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *FPaths::Combine(*TablePath, *FString::Printf(TEXT("%s.%s"), *TableName, *TableName))));

	CacheTable->Init(InType, DataTable);

	DataTables.Add(InType, CacheTable);
}
