// Sky Faller. All rights reserved.


#include "Objects/SFPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "SFListener.h"
#include "SFPlayerState.h"
#include "Objects/SFTarget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlatform, All, All)

ASFPlatform::ASFPlatform() : PlatformVariation(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASFPlatform::BeginPlay()
{
	Super::BeginPlay();

	SetTemplate();

	if (FMath::RandBool()) SpawnTarget();

	ListenerConnecting();

	LocalTime = static_cast<float>(FMath::RandHelper(10));
	if (SelfID == 0) ZeroY = GetActorLocation().Y;
}

void ASFPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bDespawned)
	{
		Spawned ? Spawner(DeltaTime) : Mover(DeltaTime);
	}
	else
	{
		Despawner(DeltaTime);
	}
}

void ASFPlatform::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	if (bTouched) return;

	const auto Player = Cast<ACharacter>(OtherActor);
	if (!Player) return;

	if (Player->GetCharacterMovement()->IsFalling()) return;

	bTouched = true;

	ScoringPoints(Player->GetPlayerState(), RewardPoints);
	SpawnNext();
}

void ASFPlatform::SetTemplate()
{
	int32 CurrentIndex;

	if (AssetsIndexes.Num() >= Assets.Num()) AssetsIndexes.Empty();

	if (Assets.Num() == 0)
	{
		UE_LOG(LogPlatform, Warning, TEXT("Platform variations not found. Set is empty."));
		return;
	}

	do
	{
		CurrentIndex = FMath::RandRange(0, Assets.Num() - 1);
	}
	while (AssetsIndexes.Contains(CurrentIndex));
	AssetsIndexes.Add(CurrentIndex);

	auto AssetIt = Assets.CreateIterator();
	for (int32 i = 0; i < CurrentIndex; ++i)
	{
		++AssetIt;
	}

	const auto AssetClass = AssetIt->LoadSynchronous();
	if (!AssetClass)
	{
		UE_LOG(LogPlatform, Warning, TEXT("Platform variation class not loaded."));
		return;
	}

	if (PlatformVariation)
	{
		TArray<AActor*> Actors;
		PlatformVariation->GetAttachedActors(Actors);
		for (const auto& Actor : Actors)
		{
			Actor->Destroy();
		}

		PlatformVariation->Destroy();
	}

	PlatformVariation = GetWorld()->SpawnActor(AssetClass, &GetActorTransform());
	if (!PlatformVariation)
	{
		UE_LOG(LogPlatform, Warning, TEXT("Platform variation instance not loaded."));
		return;
	}

	PlatformVariation->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);

	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(
		PlatformVariation->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (!PrimComp)
	{
		UE_LOG(LogPlatform, Warning, TEXT("Platform variation instance doesn't have a primitive component."));
		return;
	}

	PlatformVariation->SetOwner(this);
	PrimComp->OnComponentHit.AddDynamic(this, &ASFPlatform::OnHit);
}

void ASFPlatform::SpawnNext()
{
	// Set new platform spawn location and rotation
	float NextAngle;
	if (GlobalRotation > 75.0f)
	{
		NextAngle = -SpawnAngle;
	}
	else if (GlobalRotation < -75.0f)
	{
		NextAngle = SpawnAngle;
	}
	else
	{
		NextAngle = FMath::RandRange(-SpawnAngle, SpawnAngle);
	}

	FRotator SpawnDirection(0.0f, NextAngle, 0.0f);
	// UE_LOG(LogPlatform, Display, TEXT("Global rotation: %f"), *pGlobalRotation);

	// Set location and rotation of new platform
	FVector SpawnLocation;
	FRotator SpawnRotation;

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	GlobalRotation += SpawnDirection.Yaw;

	ASFPlatform* NewPlatform = GetWorld()->SpawnActorDeferred<ASFPlatform>(PlatformClass, SpawnTransform);
	NewPlatform->SelfID = SelfID + 1;
	NewPlatform->bIsHub = NewPlatform->SelfID % HubIndentID == 0;
	NewPlatform->ZeroY = ZeroY;
	NewPlatform->AssetsIndexes.Append(AssetsIndexes);

	// Update spawn transform
	SpawnLocation = GetActorLocation() + (GetActorRotation() + SpawnDirection).Vector() * (PLATFORM_RADIUS +
		FMath::RandRange(MinDist, MaxDist) + NewPlatform->PLATFORM_RADIUS);
	// Get end point of new platform spawn location
	if ((SpawnLocation.Y > ZeroY + SpawnY) || (SpawnLocation.Y < ZeroY - SpawnY)) // Invisible spawn borders
	{
		SpawnLocation.Y > ZeroY + SpawnY ? SpawnDirection.Yaw = -SpawnAngle : SpawnDirection.Yaw = SpawnAngle;
		SpawnLocation = GetActorLocation() + (GetActorRotation() + SpawnDirection).Vector() * (PLATFORM_RADIUS +
			FMath::RandRange(MinDist, MaxDist) + NewPlatform->PLATFORM_RADIUS);
	}
	SpawnLocation.Z += NewPlatform->SpawnHeight;
	SpawnRotation = FRotator(0.0f, (GetActorLocation() - SpawnLocation).ToOrientationRotator().Yaw + 180.0f, 0.0f);
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(FQuat(SpawnRotation));

	NewPlatform->FinishSpawning(SpawnTransform);
	if (!NewPlatform) return;

	NewPlatform->ParentZ = GetActorLocation().Z;
	NewPlatform->Speed = FMath::RandRange(NewPlatform->Speed, NewPlatform->Speed + 100.0f);
	NewPlatform->Speed = FMath::RandBool() ? NewPlatform->Speed : -NewPlatform->Speed;
	NewPlatform->Threshold = FMath::RandRange(NewPlatform->Threshold - ThresholdOffset, NewPlatform->Threshold);
	NewPlatform->GlobalRotation = GlobalRotation;

	// UE_LOG(LogPlatform, Display, TEXT("Global rotation:  %f"), GlobalRotation);
}

void ASFPlatform::ScoringPoints(APlayerState* PlayerState, const float Points)
{
	ASFPlayerState* CurrentPlayerState = Cast<ASFPlayerState>(PlayerState);
	if (!CurrentPlayerState) return;

	CurrentPlayerState->AddScore(Points);
}

void ASFPlatform::Spawner(float DeltaTime)
{
	if (GetActorLocation().Z >= ParentZ)
	{
		Spawned = false;
		return;
	}

	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, SpawnSpeed * DeltaTime));
}

void ASFPlatform::Despawner(const float DeltaTime)
{
	if (!GetWorld()) return;
	if (GetActorLocation().Z <= GetWorld()->GetWorldSettings()->KillZ)
	{
		TArray<AActor*> Actors;
		GetAttachedActors(Actors);
		for (const auto Actor : Actors)
		{
			Actor->Destroy();
		}
		Destroy();
	}
	else
	{
		SetActorLocation(GetActorLocation() - FVector(0.0f, 0.0f, DespawnSpeed * DeltaTime));
		SetActorRotation(FRotator(0.0f, 0.0f, GetActorRotation().Roll + DespawnRotationSpeed * DeltaTime));
	}
}

void ASFPlatform::Mover(float DeltaTime)
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	const auto Player = PlayerController->GetPawn();
	if (Player)
	{
		if (FMath::Abs((Player->GetActorLocation() - GetActorLocation()).Size()) > DespawnDist && bTouched)
		{
			bDespawned = true;
		}
	}

	// Horizontal moving
	Speed = (Offset >= Threshold || Offset <= -Threshold) ? -Speed : Speed;
	const float CurrentSpeed = Speed * (MinSpeed + (1 - MinSpeed) * (1 - FMath::Abs(Offset) / Threshold));
	// Smooth speed
	const float CurrentOffset = CurrentSpeed * DeltaTime;

	// Vertical moving
	const float Time = GetWorld()->GetTimeSeconds() + LocalTime;
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = ParentZ + Amplitude * FMath::Sin(Frequency * Time);

	// Set new location
	SetActorLocation(GetActorRightVector() * CurrentOffset + NewLocation);

	Offset += CurrentOffset;
}

void ASFPlatform::SpawnTarget()
{
	if (!PlatformVariation)
	{
		UE_LOG(LogPlatform, Warning, TEXT("Can't spawn target. Platform variation doesn't exists."));
		return;
	}

	const UPrimitiveComponent* PlatformPrim = Cast<UPrimitiveComponent>(
		PlatformVariation->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (!PlatformPrim)
	{
		UE_LOG(LogPlatform, Warning, TEXT("Can't spawn target. Platform variation doesn't have primitive component."));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// Spawn coords
	FVector TargetLocation = GetActorLocation();
	TargetLocation.X += PlatformPrim->Bounds.BoxExtent.X + FMath::RandRange(50.0f, 75.0f);
	TargetLocation.Y += FMath::RandRange(
		-PlatformPrim->Bounds.BoxExtent.Y - FMath::RandRange(50.0f, 75.0f),
		PlatformPrim->Bounds.BoxExtent.Y + FMath::RandRange(50.0f, 75.0f)
	);
	TargetLocation.Z += FMath::RandRange(150.0f, 700.0f);

	// Rotate to spawned platform
	const FRotator TargetRotation = (GetActorLocation() - TargetLocation).ToOrientationRotator();

	// Spawn and attach to spawned platform
	const auto NewTarget = GetWorld()->SpawnActor<ASFTarget>(TargetClass, TargetLocation, TargetRotation, SpawnParams);
	if (!NewTarget) return;

	NewTarget->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

// Move fog to player after step on platform
void ASFPlatform::ListenerConnecting()
{
	if (!GetWorld()) return;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASFListener::StaticClass(), FoundActors);

	ASFListener* ListenerInst = nullptr;

	for (AActor* Actor : FoundActors)
	{
		ASFListener* FoundListener = Cast<ASFListener>(Actor);
		if (FoundListener)
		{
			ListenerInst = FoundListener;
			break;
		}
	}
	if (!ListenerInst)
	{
		UE_LOG(LogPlatform, Warning, TEXT("Can't connect listener. Listener not found."));
		return;
	}

	if (!PlatformVariation)
	{
		UE_LOG(LogPlatform, Warning, TEXT("Can't connect listener. Platform variation instance doesn't exists."));
		return;
	}

	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(
		PlatformVariation->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (!PrimComp)
	{
		UE_LOG(LogPlatform, Warning,
		       TEXT("Can't connect listener. Platform variation instance doesn't have a primitive component."));
		return;
	}

	PrimComp->OnComponentHit.AddDynamic(ListenerInst, &ASFListener::OnPlatformHit);
}
