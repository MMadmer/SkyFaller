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
	BackgroundManage();
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
		int32 HalfObjectsNum = BackInst->GetObjectsNumOfLayer(i) / 2;
		for (int j = -HalfObjectsNum; BackInst->GetObjectsNumOfLayer(i) % 2 == 1 ? j <= HalfObjectsNum : j < HalfObjectsNum; j++)
		{
			FTransform NewTransform;

			/// <Original actor>
			ASFBackgroundActor* Actor = GetWorld()->SpawnActorDeferred<ASFBackgroundActor>(BackgroundClass, NewTransform);

			Actor->SetLayer(i);

			FBackAssets ActorLayer = Actor->GetCurrentLayer();
			FVector NewLocation = Player->GetActorLocation();
			NewLocation.Y += ActorLayer.DistY;
			NewLocation.X += ActorLayer.BetweenX * j;
			NewLocation.Z += ActorLayer.DistZ;
			NewTransform.SetLocation(NewLocation);

			Actor->SetParentZ(NewLocation.Z);

			Actor->FinishSpawning(NewTransform);
			BackgroundActors[i].Add(Actor);
			/// </Original actor>
			
			/// <Mirror actor>
			NewLocation.Y -= ActorLayer.DistY * 2;
			NewTransform.SetLocation(NewLocation);
			ASFBackgroundActor* MirrorActor = GetWorld()->SpawnActorDeferred<ASFBackgroundActor>(BackgroundClass, NewTransform);

			MirrorActor->SetLayer(i);
			MirrorActor->SetParentZ(NewLocation.Z);

			MirrorActor->FinishSpawning(NewTransform);
			BackgroundActors[i].Add(MirrorActor);
			/// </Mirror actor>
		}
	}
	// UE_LOG(LogListener, Display, TEXT("Background initialized"));
}

void ASFListener::BackgroundManage()
{
	if (!GetWorld()) return;
	const auto Player = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	if (BackgroundActors.Num() <= 0) return;

	for (auto& Layer : BackgroundActors)
	{
		if (Layer.Num() < 2) continue;

		while (BackgroundSpawner(Layer, Player, Layer.Num() - 1));
	}
}

bool ASFListener::BackgroundSpawner(TArray<ASFBackgroundActor*>& Layer, const ACharacter* Player, const int32 Index)
{
	// UE_LOG(LogListener, Display, TEXT("Objects link: %d"), Layer.Num());

	const auto LayerStruct = Layer[Index]->GetCurrentLayer();
	if (Player->GetActorLocation().X - (Layer[Index]->GetActorLocation().X + LayerStruct.DistX) >= 0)
	{
		const auto Origin = Layer[Index]->SpawnNext(BackgroundClass);
		const auto Mirror = Layer[Index - 1]->SpawnNext(BackgroundClass);
		if (!(Origin && Mirror))
		{
			UE_LOG(LogListener, Warning, TEXT("Background not spawned"));
			return false;
		}
		Layer.Add(Origin);
		Layer.Add(Mirror);
		return true;
	}

	return false;
}
