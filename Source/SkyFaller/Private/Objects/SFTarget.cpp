// Sky Faller. All rights reserved.


#include "Objects/SFTarget.h"

#include "Player/SFPlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Player/BaseCharacter.h"

ASFTarget::ASFTarget()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bWasHit = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
}

void ASFTarget::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ASFTarget::OnHit);
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ASFTarget::OnOverlapBegin);
}

void ASFTarget::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      FVector NormalImpulse, const FHitResult& Hit)
{
	const auto Player = Cast<ACharacter>(OtherActor->GetInstigator());
	if (!Player) return;

	ASFPlayerState* PlayerState = Cast<ASFPlayerState>(Player->GetPlayerState());
	if (!PlayerState) return;

	if (bWasHit) return;
	bWasHit = true;

	PlayerState->AddScore(RewardPoints + PlayerState->GetSeries() * SeriesPoints);
	PlayerState->SetSeries(PlayerState->GetSeries() + 1);
	PlayerState->bInSeries = false;

	// Physics
	StaticMeshComponent->SetSimulatePhysics(true);

	SetLifeSpan(LifeSpan);
}

void ASFTarget::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult)
{
	const auto Player = Cast<ACharacter>(OtherActor->GetInstigator());
	if (!Player) return;

	ASFPlayerState* PlayerState = Cast<ASFPlayerState>(Player->GetPlayerState());
	if (!PlayerState) return;

	if (bWasHit) return;
	bWasHit = true;

	PlayerState->AddScore(RewardPoints + PlayerState->GetSeries() * SeriesPoints);
	PlayerState->SetSeries(PlayerState->GetSeries() + 1);
	PlayerState->bInSeries = false;

	// Physics
	StaticMeshComponent->SetSimulatePhysics(true);

	SetLifeSpan(LifeSpan);
}
