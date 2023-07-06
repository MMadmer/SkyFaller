// Sky Faller. All rights reserved.


#include "SFListener.h"
#include "Objects/SFPlatform.h"
#include "Objects/SF_FloatFog.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ASFListener::ASFListener()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASFListener::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASFListener::OnPlatformHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto Player = Cast<ACharacter>(OtherActor);
	if (!Player) return;

	FogConnecting(Player);
}

// Move fog to player after step on platform
void ASFListener::FogConnecting(const ACharacter* Player)
{
	if (!GetWorld()) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASF_FloatFog::StaticClass(), FoundActors);

	ASF_FloatFog* FogInst = nullptr;

	for (AActor* Actor : FoundActors)
	{
		ASF_FloatFog* FoundFog = Cast<ASF_FloatFog>(Actor);
		if (FoundFog)
		{
			FogInst = FoundFog;
			break;
		}
	}
	if (FogInst)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		FogInst->SetActorLocation(FVector(PlayerLocation.X, PlayerLocation.Y, -1500.0f));
	}
}
