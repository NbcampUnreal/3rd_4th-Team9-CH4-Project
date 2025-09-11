#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TableRowConvertFunctionContainer.generated.h"

UCLASS()
class OVERCLOCKEDITOR_API UTableRowConvertFunctionContainer : public UObject
{
	GENERATED_BODY()

public:
	UTableRowConvertFunctionContainer();

	UFUNCTION()
	void CreateSampleData(class UDataTable* OutDataTable, const FString& InCSVString);

private:
	
	void OutHeaderAndValues(const FString& InCSVString, TMap<FName, int32>& Headers, TArray<TArray<FString>>& Values, const FString& CSVFileName);

	FORCEINLINE int32 ParseIntValue(const FString& ValueString) const
	{
		return FCString::Atoi(*ValueString);
	}

	FORCEINLINE float ParseFloatValue(const FString& ValueString) const
	{
		return FCString::Atof(*ValueString);
	}

	FORCEINLINE int64 ParseLongValue(const FString& ValueString) const
	{
		return FCString::Atoi64(*ValueString);
	}

	FORCEINLINE double ParseDoubleValue(const FString& ValueString) const
	{
		return FCString::Atod(*ValueString);
	}

	template<typename TEnum>
	FORCEINLINE TEnum ParseEnumValue(const FString& ValueString) const
	{
		const UEnum* EnumType = StaticEnum<TEnum>();
		check(EnumType);
		const int64 EnumValueInt = EnumType->GetValueByNameString(ValueString);

		return (TEnum)EnumValueInt;
	}

	template<typename T>
	FORCEINLINE TSoftObjectPtr<T> ParseSoftObjectValue(const FString& ValueString) const
	{
		return TSoftObjectPtr<T>(*ValueString);
	}

	FORCEINLINE int32 GetHeaderIndex(TMap<FName, int> Headers, const FName& HeaderName)
	{
		return Headers[HeaderName];
	}
};
