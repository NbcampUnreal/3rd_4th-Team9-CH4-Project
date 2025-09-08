#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SampleDataStruct.generated.h"

USTRUCT(BlueprintType)
struct OVERCLOCK_API FSampleDataStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	FSampleDataStruct()
		: Index(1)
		, SampleName(TEXT("SampleName"))
		, Width(0.f)
		, Height(0.f)
	{

	}


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	int32 Index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	FString SampleName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	float Height;
};
