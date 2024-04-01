// Sky Faller. All rights reserved.


#include "Objects/SFTarget.h"

#include "SFPlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Player/Weapon/SFArrow.h"
#include "Player/BaseCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogTarget, All, All)

ASFTarget::ASFTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(StaticMeshComponent);
}

void ASFTarget::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ASFTarget::OnHit);
}

void ASFTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASFTarget::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHitted) return;
	bHitted = true;

	const auto Arrow = Cast<ASFArrow>(OtherActor);
	if (!Arrow) return;

	const auto Player = Cast<ACharacter>(Arrow->GetOwner());
	if (!Player) return;

	ASFPlayerState* PlayerState = Cast<ASFPlayerState>(Player->GetPlayerState());
	if (!PlayerState) return;

	PlayerState->AddScore(RewardPoints + PlayerState->GetSeries() * SeriesPoints);
	PlayerState->SetSeries(PlayerState->GetSeries() + 1);
	PlayerState->bInSeries = false;

	// Physics
	StaticMeshComponent->SetSimulatePhysics(true);
	const FVector Force = Arrow->GetVelocity() * Arrow->GetMesh()->GetMass() * Arrow->ImpactForceMultiplier;
	StaticMeshComponent->AddForceAtLocation(Force, Hit.ImpactPoint);

	SetLifeSpan(LifeSpan);
}
