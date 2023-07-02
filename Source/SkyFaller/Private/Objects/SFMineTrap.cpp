// Sky Faller. All rights reserved.


#include "Objects/SFMineTrap.h"
#include "Components/SFExplosionComponent.h"
#include "Components/SphereComponent.h"
#include "Player/BaseCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogMineTrap, All, All)

ASFMineTrap::ASFMineTrap()
{
	ExplosionComponent = CreateDefaultSubobject<USFExplosionComponent>("ExplosionComponent");
	InnerSphere = CreateDefaultSubobject<USphereComponent>("InnerSphere");
	OuterSphere = CreateDefaultSubobject<USphereComponent>("OuterSphere");
}

void ASFMineTrap::BeginPlay()
{
	Super::BeginPlay();

	InnerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	OuterSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	InnerSphere->SetSphereRadius(ExplosionComponent->GetInnerRadius());
	OuterSphere->SetSphereRadius(ExplosionComponent->GetOuterRadius());
	InnerSphere->AttachToComponent(TrapMesh, FAttachmentTransformRules::KeepRelativeTransform);
	OuterSphere->AttachToComponent(TrapMesh, FAttachmentTransformRules::KeepRelativeTransform);

	TrapMesh->OnComponentHit.AddDynamic(this, &ASFMineTrap::OnHit);
	InnerSphere->OnComponentBeginOverlap.AddDynamic(this, &ASFMineTrap::OnInnerOverlap);
	OuterSphere->OnComponentBeginOverlap.AddDynamic(this, &ASFMineTrap::OnOuterOverlap);
}

void ASFMineTrap::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explosion();
}

void ASFMineTrap::OnInnerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<ABaseCharacter>(OtherActor)) return;

	Explosion();
}

void ASFMineTrap::OnOuterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!(GetWorld() && Cast<ABaseCharacter>(OtherActor))) return;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASFMineTrap::Timer, TimerTick, true);
}

void ASFMineTrap::Explosion()
{
	if (!GetWorld()) return;

	bExploded = true;
	ExplosionComponent->Explode(GetActorLocation());
	InnerSphere->DestroyComponent();
	OuterSphere->DestroyComponent();

	TrapMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TrapMesh->SetVisibility(false);
	SetLifeSpan(5.0f);
}

void ASFMineTrap::Timer()
{
	if (!GetWorld()) return;

	if (bExploded)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		return;
	}

	TimerTime -= TimerTick;
	if (TimerTime <= 0.0f)
	{
		Explosion();
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	// Explosion sound
	UAudioComponent* AudioComponent = NewObject<UAudioComponent>(this);
	USoundAttenuation* AttenuationSettings = NewObject<USoundAttenuation>(this);
	if (AudioComponent && TickSound && AttenuationSettings)
	{
		// Set sound radius
		AttenuationSettings->Attenuation.bAttenuate = true;
		AttenuationSettings->Attenuation.AttenuationShape = EAttenuationShape::Sphere;
		AttenuationSettings->Attenuation.AttenuationShapeExtents = FVector(ExplosionComponent->GetOuterRadius());

		// Play sound
		AudioComponent->SetSound(TickSound);
		AudioComponent->bAllowSpatialization = true;
		AudioComponent->bAutoDestroy = true;
		AudioComponent->AttenuationSettings = AttenuationSettings;
		AudioComponent->SetWorldLocation(GetActorLocation());
		AudioComponent->Play();
	}
}
