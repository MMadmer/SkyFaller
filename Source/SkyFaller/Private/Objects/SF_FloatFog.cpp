// Sky Faller. All rights reserved.


#include "Objects/SF_FloatFog.h"

#include "Components/BGCHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DamageTypes/PureDamage.h"
#include "Kismet/GameplayStatics.h"


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
	const UBGCHealthComponent* HealthComp = Cast<UBGCHealthComponent>(
		OtherActor->GetComponentByClass(UBGCHealthComponent::StaticClass()));
	if (!HealthComp) return;

	UGameplayStatics::ApplyDamage(OtherActor, HealthComp->MaxHealth, nullptr, this, UPureDamage::StaticClass());
}
