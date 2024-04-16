// Sky Faller. All rights reserved.


#include "Objects/SF_FloatFog.h"

#include "SFHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Damage/SFPureDamage.h"
#include "Kismet/GameplayStatics.h"


DEFINE_LOG_CATEGORY_STATIC(LogFog, All, All)

ASF_FloatFog::ASF_FloatFog()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	FogMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(FogMesh);
}

void ASF_FloatFog::BeginPlay()
{
	Super::BeginPlay();

	FogMesh->OnComponentBeginOverlap.AddDynamic(this, &ASF_FloatFog::KillActor);
}

void ASF_FloatFog::KillActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                             const FHitResult& SweepResult)
{
	const USFHealthComponent* HealthComp = Cast<USFHealthComponent>(
		OtherActor->GetComponentByClass(USFHealthComponent::StaticClass()));
	if (!HealthComp) return;

	UGameplayStatics::ApplyDamage(OtherActor, HealthComp->MaxHealth, nullptr, this, USFPureDamage::StaticClass());
}
