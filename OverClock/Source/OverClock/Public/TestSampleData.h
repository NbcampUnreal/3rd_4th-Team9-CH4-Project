#pragma once

#include "CoreMinimal.h"
#include "TestSampleData.generated.h"

USTRUCT(BlueprintType)
struct OVERCLOCK_API FTestSampleData
{
	GENERATED_BODY()

public:
	FTestSampleData()
		: Index(0)
		, Width(0.f)
		, Height(0.f)
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height;

};
