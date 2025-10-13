#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OCBarrierInteract.generated.h"

class AOCBarrierField;
struct FHitResult;

UENUM(BlueprintType)
enum class EOCBarrierHitResponse : uint8
{
	DestroySelf UMETA(DisplayName = "Destroy Self"),
	ExplodeAtContact UMETA(DisplayName = "Explode At Contact"),
	Ignore UMETA(DisplayName = "Ignore")
};

UINTERFACE(BlueprintType)
class UOCBarrierInteract : public UInterface
{
	GENERATED_BODY()
};


class OVERCLOCK_API IOCBarrierInteract
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Barrier")
	float GetBarrierDamage() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Barrier")
	EOCBarrierHitResponse GetBarrierHitResponse() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Barrier")
	void OnBarrierContact(AOCBarrierField* Barrier, const FHitResult& HitResult);
};
