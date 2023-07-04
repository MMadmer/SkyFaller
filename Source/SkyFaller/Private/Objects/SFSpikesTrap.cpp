// Sky Faller. All rights reserved.


#include "Objects/SFSpikesTrap.h"
#include "GameFramework/Pawn.h"

ASFSpikesTrap::ASFSpikesTrap()
{
	TrapMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

void ASFSpikesTrap::BeginPlay()
{
	Super::BeginPlay();

	TrapMesh->OnComponentBeginOverlap.AddDynamic(this, &ASFSpikesTrap::OnBeginOverlap);

}

void ASFSpikesTrap::DealingDamage()
{
	if (!GetWorld()) return;

	TArray<AActor*> OverlappedPawns;
	TrapMesh->GetOverlappingActors(OverlappedPawns, APawn::StaticClass());
	if (!OverlappedPawns.IsValidIndex(0))
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
		return;
	}

	for (const auto& Pawn : OverlappedPawns) Pawn->TakeDamage(Damage, FDamageEvent::FDamageEvent(), nullptr, this);

	PlayTrapSound();
}

void ASFSpikesTrap::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!GetWorld()) return;

	if (Cast<APawn>(OtherActor)) GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &ASFSpikesTrap::DealingDamage, TimerTick, true, 0.0f);
}
