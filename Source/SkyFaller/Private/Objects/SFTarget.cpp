// Sky Faller. All rights reserved.


#include "Objects/SFTarget.h"
#include "Components/StaticMeshComponent.h"
#include "Player/Weapon/SFArrow.h"
#include "Components/SFProgressComponent.h"
#include "Player/BaseCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

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

void ASFTarget::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHitted) return;
	bHitted = true;

	const auto Arrow = Cast<ASFArrow>(OtherActor);
	if (!Arrow) return;

	const auto Player = Cast<ABaseCharacter>(Arrow->GetOwner());
	if (!Player) return;

	USFProgressComponent* ProgressComponent = Cast<USFProgressComponent>(Player->GetComponentByClass(USFProgressComponent::StaticClass()));
	if (!ProgressComponent) return;

	ProgressComponent->AddScore(RewardPoints);

	// Physics
	StaticMeshComponent->SetSimulatePhysics(true);
	FVector Force = Arrow->GetVelocity() * Arrow->GetMesh()->GetMass() * Arrow->ImpactForceMultiplier;
	StaticMeshComponent->AddForceAtLocation(Force, Hit.ImpactPoint);

	SetLifeSpan(LifeSpan);
	// UE_LOG(LogTarget, Display, TEXT("ArrowSpeed: %f"), Arrow->GetVelocity().Size());
	// Arrow->SetActorHiddenInGame(true);
	// Arrow->SetLifeSpan(Arrow->GetLifeHitTrace() + 0.1f);
	// Destroy();
}
