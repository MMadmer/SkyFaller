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

	BackgroundInit();
}

void ASFListener::OnPlatformHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{
	const auto Player = Cast<ACharacter>(OtherActor);
	if (!Player) return;

	const auto Platform = Cast<ASFPlatform>(HitComponent->GetOwner()->GetOwner());
	if (!Platform) return;

	if (Platform->GetSelfID() == CachedId) return;
	CachedId = Platform->GetSelfID();

	FogMoving(Player);
	BackgroundManage(Player);
}

// Move fog to player after step on platform
void ASFListener::FogMoving(const ACharacter* Player) const
{
	if (!GetWorld() || !Player) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASF_FloatFog::StaticClass(), FoundActors);

	const FVector PlayerLocation = Player->GetActorLocation();
	for (AActor* Actor : FoundActors)
	{
		Actor->SetActorLocation(FVector(PlayerLocation.X, PlayerLocation.Y, -1500.0f));
	}
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
		const int32 HalfObjectsNum = BackInst->GetObjectsNumOfLayer(i) / 2;
		for (int j = -HalfObjectsNum; BackInst->GetObjectsNumOfLayer(i) % 2 == 1
			                              ? j <= HalfObjectsNum
			                              : j < HalfObjectsNum; j++)
		{
			FTransform NewTransform;

			/// <Original actor>
			ASFBackgroundActor* Actor = GetWorld()->SpawnActorDeferred<ASFBackgroundActor>(
				BackgroundClass, NewTransform);

			Actor->SetLayer(i);

			const FBackAssets ActorLayer = Actor->GetCurrentLayer();
			FVector NewLocation = FVector(0.0f);
			NewLocation.Y += ActorLayer.DistY + FMath::RandRange(-ActorLayer.OffsetY, ActorLayer.OffsetY);
			NewLocation.X += ActorLayer.BetweenX * j + FMath::RandRange(-ActorLayer.OffsetX, ActorLayer.OffsetX);
			NewLocation.Z += ActorLayer.DistZ + FMath::RandRange(-ActorLayer.OffsetZ, ActorLayer.OffsetZ);
			NewTransform.SetLocation(NewLocation);

			Actor->SetParentZ(NewLocation.Z);

			Actor->FinishSpawning(NewTransform);
			BackgroundActors[i].Add(Actor);
			/// </Original actor>

			/// <Mirror actor>
			NewLocation = FVector(0.0f);
			NewLocation.Y -= ActorLayer.DistY + FMath::RandRange(-ActorLayer.OffsetY, ActorLayer.OffsetY);
			NewLocation.X += ActorLayer.BetweenX * j + FMath::RandRange(-ActorLayer.OffsetX, ActorLayer.OffsetX);
			NewLocation.Z += ActorLayer.DistZ + FMath::RandRange(-ActorLayer.OffsetZ, ActorLayer.OffsetZ);
			NewTransform.SetLocation(NewLocation);
			ASFBackgroundActor* MirrorActor = GetWorld()->SpawnActorDeferred<ASFBackgroundActor>(
				BackgroundClass, NewTransform);

			MirrorActor->SetLayer(i);
			MirrorActor->SetParentZ(NewLocation.Z);

			MirrorActor->FinishSpawning(NewTransform);
			BackgroundActors[i].Add(MirrorActor);
			/// </Mirror actor>
		}
	}
	// UE_LOG(LogListener, Display, TEXT("Background initialized"));
}

void ASFListener::BackgroundManage(const ACharacter* Player)
{
	if (!GetWorld()) return;

	if (BackgroundActors.Num() <= 0) return;

	for (auto& Layer : BackgroundActors)
	{
		if (Layer.Num() < 2) continue;

		while (BackgroundSpawner(Layer, Player, Layer.Num() - 2, true))
		{
		}
		while (BackgroundSpawner(Layer, Player, 0, false))
		{
		}
		while (BackgroundDespawner(Layer, Player, 0))
		{
		}
		while (BackgroundDespawner(Layer, Player, Layer.Num() - 2));
	}
}

bool ASFListener::BackgroundSpawner(TArray<ASFBackgroundActor*>& Layer, const ACharacter* Player, const int32 Index,
                                    const bool bFront) const
{
	// UE_LOG(LogListener, Display, TEXT("Objects link: %d"), Layer.Num());
	if (Index > Layer.Num() - 2)
	{
		UE_LOG(LogListener, Warning, TEXT("Wrong index"));
		return false;
	}

	const auto LayerStruct = Layer[Index]->GetCurrentLayer();
	const float PlayerX = Player->GetActorLocation().X;
	const float ActorX = Layer[Index]->GetActorLocation().X;

	if (!((PlayerX - (LayerStruct.SpawnDist + LayerStruct.BetweenX + LayerStruct.OffsetX) <= ActorX) && (ActorX <=
		PlayerX + (LayerStruct.SpawnDist + LayerStruct.BetweenX + LayerStruct.OffsetX))))
		return false;
	const float BetweenX = bFront ? LayerStruct.BetweenX : -LayerStruct.BetweenX;
	if (!((PlayerX - (LayerStruct.SpawnDist + LayerStruct.BetweenX + LayerStruct.OffsetX) <= ActorX + BetweenX +
		LayerStruct.OffsetX) && (ActorX + BetweenX + LayerStruct.OffsetX <= PlayerX + (LayerStruct.SpawnDist +
		LayerStruct.BetweenX + LayerStruct.OffsetX))))
		return false;

	const auto Origin = Layer[Index]->SpawnNext(BackgroundClass, bFront);
	const auto Mirror = Layer[Index + 1]->SpawnNext(BackgroundClass, bFront);
	if (!(Origin && Mirror))
	{
		UE_LOG(LogListener, Warning, TEXT("Background not spawned"));
		return false;
	}

	if (Index < Layer.Num() - 2)
	{
		Layer.Insert(Origin, Index);
		Layer.Insert(Mirror, Index + 1);
	}
	else
	{
		Layer.Add(Origin);
		Layer.Add(Mirror);
	}

	return true;
}

bool ASFListener::BackgroundDespawner(TArray<ASFBackgroundActor*>& Layer, const ACharacter* Player, const int32 Index)
{
	// UE_LOG(LogListener, Display, TEXT("Objects link: %d"), Layer.Num());
	if (!(Layer.IsValidIndex(Index) && Layer.IsValidIndex(Index + 1))) return false;

	const auto LayerStruct = Layer[Index]->GetCurrentLayer();
	const float PlayerX = Player->GetActorLocation().X;
	const float ActorX = Layer[Index]->GetActorLocation().X;

	if ((PlayerX - (LayerStruct.SpawnDist + LayerStruct.BetweenX + LayerStruct.OffsetX) <= ActorX) && (ActorX <= PlayerX
		+ (LayerStruct.SpawnDist + LayerStruct.BetweenX + LayerStruct.OffsetX)))
		return false;

	Layer[Index]->Despawn();
	Layer[Index + 1]->Despawn();

	Layer.RemoveAt(Index);
	Layer.RemoveAt(Index);

	return true;
}
