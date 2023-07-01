// Sky Faller. All rights reserved.


#include "Objects/SFMineTrap.h"
#include "Components/SFExplosionComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogMineTrap, All, All)

ASFMineTrap::ASFMineTrap()
{
	ExplosionComponent = CreateDefaultSubobject<USFExplosionComponent>("ExplosionComponent");
}

void ASFMineTrap::BeginPlay()
{
	Super::BeginPlay();

	TrapMesh->OnComponentHit.AddDynamic(this, &ASFMineTrap::OnHit);
}

void ASFMineTrap::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!GetWorld()) return;

	ExplosionComponent->Explode();
	if (NiagaraSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, GetActorLocation(), GetActorRotation());
	}

	TrapMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TrapMesh->SetVisibility(false);
	SetLifeSpan(5.0f);
}
