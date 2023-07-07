// Sky Faller. All rights reserved.


#include "SFListener.h"
#include "Objects/SFPlatform.h"
#include "Objects/SF_FloatFog.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/SFBackgroundActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogListener, All, All)

ASFListener::ASFListener()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASFListener::BeginPlay()
{
	Super::BeginPlay();

	PlatformConnecting(0);
	BackgroundInit();
}

void ASFListener::OnPlatformHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto Player = Cast<ACharacter>(OtherActor);
	if (!Player) return;

	FogMoving(Player);

}

// Move fog to player after step on platform
void ASFListener::FogMoving(const ACharacter* Player)
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

void ASFListener::PlatformConnecting(int32 Index)
{
	if (!GetWorld()) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASFPlatform::StaticClass(), FoundActors);

	ASFPlatform* PlatformInst = nullptr;

	for (AActor* Actor : FoundActors)
	{
		ASFPlatform* FoundPlatform = Cast<ASFPlatform>(Actor);
		if (FoundPlatform)
		{
			PlatformInst = FoundPlatform;
			break;
		}
	}
	if (!PlatformInst) return;

	MainPlatform = PlatformInst;
}

void ASFListener::BackgroundInit()
{
	if (!GetWorld()) return;
	const auto Player = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	const auto BackInst = Cast<ASFBackgroundActor>(BackgroundClass->GetDefaultObject());
	if (!BackInst) return;

	BackgroundActors.SetNum(BackInst->GetBackLayers());

	for (int i = 0; i < BackgroundActors.Num(); i++)
	{
		for (int j = -BackInst->GetObjectsNumOfLayer(i) + 1; j <= 0; j++)
		{
			FTransform NewTransform;

			/// <First actor>
			ASFBackgroundActor* Actor = GetWorld()->SpawnActorDeferred<ASFBackgroundActor>(BackgroundClass, NewTransform);

			Actor->SetLayer(i);

			FBackAssets ActorLayer = Actor->GetCurrentLayer();
			FVector NewLocation = GetActorLocation();
			NewLocation.Y += ActorLayer.DistY;
			NewLocation.X += ActorLayer.BetweenX * j;
			NewTransform.SetLocation(NewLocation);

			Actor->FinishSpawning(NewTransform);
			BackgroundActors[i].Add(Actor);
			/// </First actor>
			
			/// <Second actor>
			NewLocation.Y -= ActorLayer.DistY * 2;
			NewTransform.SetLocation(NewLocation);
			ASFBackgroundActor* MirrorActor = GetWorld()->SpawnActorDeferred<ASFBackgroundActor>(BackgroundClass, NewTransform);
			Actor->SetLayer(i);
			MirrorActor->FinishSpawning(NewTransform);
			BackgroundActors[i].Add(MirrorActor);
			/// </Second actor>
		}
	}
	// UE_LOG(LogListener, Display, TEXT("Background initialized"));
}
