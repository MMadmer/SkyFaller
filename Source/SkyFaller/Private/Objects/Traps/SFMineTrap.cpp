// Sky Faller. All rights reserved.


#include "Objects/Traps/SFMineTrap.h"
#include "Components/SFExplosionComponent.h"
#include "Components/SphereComponent.h"
#include "Player/BaseCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

ASFMineTrap::ASFMineTrap()
{
	ExplosionComponent = CreateDefaultSubobject<USFExplosionComponent>("ExplosionComponent");
	Damage = ExplosionComponent->GetDamage();

	InnerSphere = CreateDefaultSubobject<USphereComponent>("InnerSphere");
	InnerSphere->SetupAttachment(TrapMesh);

	OuterSphere = CreateDefaultSubobject<USphereComponent>("OuterSphere");
	OuterSphere->SetupAttachment(InnerSphere);

	TickSound = nullptr;
}

void ASFMineTrap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!ExplosionComponent) return;

	Damage = ExplosionComponent->GetDamage();
	if (InnerSphere) InnerSphere->SetSphereRadius(ExplosionComponent->GetInnerRadius());
	if (OuterSphere) OuterSphere->SetSphereRadius(ExplosionComponent->GetOuterRadius());
}

void ASFMineTrap::BeginPlay()
{
	Super::BeginPlay();

	InnerSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	OuterSphere->SetCollisionResponseToAllChannels(ECR_Overlap);

	InnerSphere->SetSphereRadius(ExplosionComponent->GetInnerRadius());
	OuterSphere->SetSphereRadius(ExplosionComponent->GetOuterRadius());
	// InnerSphere->AttachToComponent(TrapMesh, FAttachmentTransformRules::KeepRelativeTransform);
	// OuterSphere->AttachToComponent(TrapMesh, FAttachmentTransformRules::KeepRelativeTransform);

	TrapMesh->OnComponentHit.AddDynamic(this, &ASFMineTrap::OnHit);
	InnerSphere->OnComponentBeginOverlap.AddDynamic(this, &ASFMineTrap::OnInnerOverlap);
	OuterSphere->OnComponentBeginOverlap.AddDynamic(this, &ASFMineTrap::OnOuterOverlap);
}

void ASFMineTrap::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	Explosion();
}

void ASFMineTrap::OnInnerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if (!Cast<ABaseCharacter>(OtherActor)) return;

	Explosion();
}

void ASFMineTrap::OnOuterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
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
