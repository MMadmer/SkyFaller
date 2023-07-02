// Sky Faller. All rights reserved.


#include "Objects/SFBearTrap.h"
#include "Components/StaticMeshComponent.h"

ASFBearTrap::ASFBearTrap()
{
	FirstMesh = CreateDefaultSubobject<UStaticMeshComponent>("FirstMesh");
	SecondMesh = CreateDefaultSubobject<UStaticMeshComponent>("SecondMesh");

	FirstMesh->SetupAttachment(RootComponent);
	SecondMesh->SetupAttachment(RootComponent);
}

void ASFBearTrap::BeginPlay()
{
	Super::BeginPlay();

	FirstMesh->OnComponentHit.AddDynamic(this, &ASFBearTrap::OnHit);
	SecondMesh->OnComponentHit.AddDynamic(this, &ASFBearTrap::OnHit);
}

void ASFBearTrap::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bActivated || !GetWorld()) return;
	bActivated = true;

	OtherActor->TakeDamage(Damage, FDamageEvent::FDamageEvent(), nullptr, this);

	FirstMesh->AddLocalOffset(FVector(0.0f, 6.9f, 0.0f));
	SecondMesh->AddLocalOffset(FVector(0.0f, -6.9f, 0.0f));
	GetWorld()->GetTimerManager().SetTimer(AnimHandle, this, &ASFBearTrap::Animation, AnimTick, true);

	PlayTrapSound();
}

void ASFBearTrap::Animation()
{
	if (!GetWorld()) return;

	AnimTime -= AnimTick;
	if (AnimTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(AnimHandle);
		return;
	}

	FirstMesh->AddLocalRotation(FRotator(0.0f, 0.0f, -Offset));
	SecondMesh->AddLocalRotation(FRotator(0.0f, 0.0f, Offset));
}
