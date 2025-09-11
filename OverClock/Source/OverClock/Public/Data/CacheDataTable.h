#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/GameDataType.h"
#include "Engine/DataTable.h"
#include "CacheDataTable.generated.h"

UCLASS()
class OVERCLOCK_API UCacheDataTable : public UObject
{
	GENERATED_BODY()

public:
	void Init(EGameDataType InDataType, UDataTable* NewDataTable);

	FORCEINLINE UDataTable* GetTable() const
	{
		return CachedDataTable;
	}

	FORCEINLINE const FName& GetKeyName(int32 InKey) const
	{
		return KeyList[InKey];
	}

	template<typename TRow>
	TRow* FindRow(int32 InKey, const FString& ContextString, bool bWarnIfRowMissing)
	{
		return CachedDataTable->FindRow<TRow>(KeyList[InKey], ContextString, bWarnIfRowMissing);
	}

private:
	UPROPERTY()
	TMap<int32, FName> KeyList;
	UPROPERTY()
	TObjectPtr<UDataTable> CachedDataTable;

	template<typename TRow>
	void InitKeyList(UDataTable* NewDataTable)
	{
		static_assert(std::is_base_of_v<class ITableKey, TRow>, "TRow must be a child of ITableKey");

		CachedDataTable = NewDataTable;

		const TArray<FName>& RowNames = NewDataTable->GetRowNames();
		for (const auto& RowName : RowNames)
		{
			const TRow* Row = reinterpret_cast<const TRow*>(NewDataTable->FindRowUnchecked(RowName));

			KeyList.Add(Row->GetKey(), RowName);
		}
	}
};
