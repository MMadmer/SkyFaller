// Sky Faller. All rights reserved.


#include "Objects/SF_FloatFog.h"
#include "Components/StaticMeshComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogFog, All, All)

ASF_FloatFog::ASF_FloatFog()
{
	PrimaryActorTick.bCanEverTick = false;

	FogMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(FogMesh);
}

void ASF_FloatFog::BeginPlay()
{
	Super::BeginPlay();

	FogMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	FogMesh->OnComponentBeginOverlap.AddDynamic(this, &ASF_FloatFog::KillActor);
}

void ASF_FloatFog::KillActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                             const FHitResult& SweepResult)
{
	OtherActor->TakeDamage(1000000.0f, FDamageEvent(), nullptr, this);
}
