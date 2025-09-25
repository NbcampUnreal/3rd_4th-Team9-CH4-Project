#include "GameEffects/MarkVisualActor.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMarkVisualActor::AMarkVisualActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 네트워크 설정
	bReplicates = true;
	//SetReplicateMovement(true);

	// 루트 컴포넌트 (더미)
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// 표식 메시 생성
	MarkMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkMesh"));
	MarkMesh->SetupAttachment(RootComponent);
	MarkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkMesh->SetCastShadow(false);
    
	// 기본 메시 설정 (화살표)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ArrowMesh(TEXT("/Engine/BasicShapes/Cone"));
	if (ArrowMesh.Succeeded())
	{
		MarkMesh->SetStaticMesh(ArrowMesh.Object);
		MarkMesh->SetWorldScale3D(FVector(0.5f, 0.5f, 1.0f));
		MarkMesh->SetRelativeRotation(FRotator(180.0f, 0.0f, 0.0f)); // 아래쪽 향하도록
	}

	// 발광 머티리얼 설정
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GlowMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	if (GlowMaterial.Succeeded())
	{
		UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(GlowMaterial.Object, this);
		if (DynamicMat)
		{
			DynamicMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); // 빨간색
			MarkMesh->SetMaterial(0, DynamicMat);
		}
	}

	// 파티클 효과
	MarkEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MarkEffect"));
	MarkEffect->SetupAttachment(RootComponent);
	MarkEffect->bAutoActivate = true;
	MarkEffect->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 기본 수명 설정 (안전장치)
	InitialLifeSpan = 15.0f; // GE가 10초이므로 여유있게

	BaseOffset = MarkOffset;
}

void AMarkVisualActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMarkVisualActor::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;
}

void AMarkVisualActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) return;
	
	// 타겟이 유효하지 않으면 제거
	if (!TargetActor.IsValid() || !IsValid(TargetActor.Get()))
	{
		RemoveMark();
		return;
	}

	// 타겟 위치로 이동
	FVector TargetLocation = TargetActor->GetActorLocation();
    
	BobTime += DeltaTime;

	// 회전
	FRotator CurrentRotation = GetActorRotation();
	CurrentRotation.Yaw += RotationSpeed * DeltaTime;
	SetActorRotation(CurrentRotation);

	// 상하 움직임 (사인파)
	float BobOffset = FMath::Sin(BobTime * BobSpeed) * BobAmplitude;
	FVector FinalOffset = BaseOffset;
	FinalOffset.Z += BobOffset;

	// 최종 위치 설정
	SetActorLocation(TargetLocation + FinalOffset);
	UE_LOG(LogTemp, Warning, TEXT("MarkLocation : %s"), *GetActorLocation().ToString())
}

void AMarkVisualActor::RemoveMark()
{
	UE_LOG(LogTemp, Warning, TEXT("Removing mark visual: %s"), *GetName());
    
	// 소멸 이펙트 재생
	if (DestroyEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation());
	}
    
	// 액터 제거
	Destroy();
}