#pragma once

#include "CoreMinimal.h"
#include "Interface/PooledInterface.h"


template <typename InPooled>
class TItemPool
{
	static_assert(TIsDerivedFrom<InPooled, AActor>::Value,"InPooled must derive from AActor");
	static_assert(TIsDerivedFrom<InPooled, IPooledInterface>::Value,"InPooled must implement IPooledInterface");

public:
	TItemPool(UWorld* World, int32 Size = 10) : PoolSize(Size), CachedWorld(World)//GetWorld()
	{
		if (!CachedWorld)
		{
			UE_LOG(LogTemp, Error, TEXT("[TItemPool] World is null"));
		}
	}
	~TItemPool()
	{
		//ReleasePool();
	}

protected:

	TArray<TObjectPtr<InPooled>> PooledActors;

	TSubclassOf<InPooled> PooledClass;

	int32 PoolSize=10;

	TObjectPtr<UWorld> CachedWorld;
		

public:

	void SetSize(int32 NewSize)
	{
		PoolSize=NewSize;
	}

	void AllocateAll(TSubclassOf<InPooled> PoolingClass, int32 NewSize=10)
	{
		if (!CachedWorld) return;
		
		if (!PoolingClass) return;
		PooledClass = PoolingClass;
				
		if (NewSize > PoolSize)
		{
			PoolSize = NewSize;
		}
		for (int32 i = 0; i < PoolSize; i++)
		{
			Alloc(PooledClass);
		}		
	}
	
	void Alloc(TSubclassOf<InPooled> PoolingClass)
	{
		if (!CachedWorld) return;
		if (!PoolingClass) return;
		
		if (InPooled* PooledActor = CachedWorld->SpawnActor<InPooled>(
			PoolingClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator
			))
		{
			PooledActor->UnInit();
			PooledActors.Emplace(PooledActor);
		}
	}

	void Init(InPooled* PooledActor)
	{
		PooledActor->Init();
	}

	void InitializeAll()
	{
		for (InPooled* PooledActor : PooledActors)
		{
			if (PooledActor)
			{
				PooledActor->Init();
			}
		}
	}

	void UninitializeAll()
	{
		for (InPooled* PooledActor : PooledActors)
		{
			PooledActor->UnInit();
		}
	}

	void ReleasePool()
	{
		// if (PooledActors.Num())
		// {
		// 	if (CachedWorld || !CachedWorld->bIsTearingDown)
		// 	{
		// 		for (InPooled* Actor : PooledActors)
		// 		{
		// 			if (IsValid(Actor))
		// 			{
		// 				Actor->Release();//Destroy();
		// 			}
		// 		}
		// 	}
		// 	PooledActors.Empty();
		// }
	}
	
	TObjectPtr<InPooled> GetPooledActor()
	{
		for (InPooled* Actor : PooledActors)
		{
			if (IsValid(Actor) && Actor->bInitial)
			{
				return Actor;
			}
		}
		// 없으면 새로 할당
		if (PooledClass)
		{
			for (int32 i = 0; i < 10; i++)
			{
				Alloc(PooledClass);
			}
			// 첫 번째 새 액터 반환
			if (PooledActors.Num() > 0)
			{
				InPooled* NewActor = PooledActors.Last();
				if (NewActor)
				{
					NewActor->Init();
					return NewActor;
				}
			}
		}
   
		return nullptr;
	}
};
