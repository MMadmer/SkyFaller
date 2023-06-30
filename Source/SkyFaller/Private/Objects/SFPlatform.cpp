// Sky Faller. All rights reserved.


#include "Objects/SFPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Player/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/SF_FloatFog.h"
#include "Components/SFProgressComponent.h"
#include "Objects/SFTarget.h"
#include "Objects/SFPlatformSkin.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "..\..\Public\Objects\SFPlatform.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlatform, All, All)

ASFPlatform::ASFPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlatformMesh");
	SkinMesh = CreateDefaultSubobject<UStaticMeshComponent>("SkinMesh");
	SetRootComponent(PlatformMesh);
	SkinMesh->SetupAttachment(PlatformMesh);
}

void ASFPlatform::BeginPlay()
{
	Super::BeginPlay();

	SetTemplate();

	PlatformMesh->OnComponentHit.AddDynamic(this, &ASFPlatform::OnHit);

	FogConnecting();

	LocalTime = (float)FMath::RandHelper(10);
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

void ASFPlatform::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto World = GetWorld();
	if (!World || bTouched) return;

	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;

	if (Player->GetCharacterMovement()->IsFalling()) return;

	bTouched = true;

	ScoringPoints(Player, RewardPoints);
	SpawnNext(World, Player);
}

void ASFPlatform::SetTemplate()
{
	int32 CurrentIndex;
	FAssets Asset;

	if (AssetsIndexes.Num() >= Assets.Num()) AssetsIndexes.Empty();

	if (!Assets.IsValidIndex(0)) return;

	do
	{
		CurrentIndex = FMath::RandRange(0, Assets.Num() - 1);

	} while (AssetsIndexes.Contains(CurrentIndex));
	AssetsIndexes.Add(CurrentIndex);

	Asset = Assets[CurrentIndex];

	PlatformMesh->SetStaticMesh(Asset.Platform);
	SkinMesh->SetStaticMesh(Asset.Skin);
}

void ASFPlatform::SpawnNext(UWorld* World, ABaseCharacter* Player)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Set new platform spawn location and rotation
	float NextAngle = FMath::RandRange(-SpawnAngle, SpawnAngle);
	FRotator SpawnDirection(0.0f, NextAngle, 0.0f);
	UE_LOG(LogPlatform, Display, TEXT("Angle: %f"),NextAngle);
	FVector SpawnLocation = GetActorLocation() + (GetActorRotation() + SpawnDirection).Vector() * (MESH_DIAMETER + FMath::RandRange(MinDist, MaxDist)); // Get end point of new platform spawn location
	SpawnLocation.Z += SpawnHeight;
	FRotator SpawnRotation = FRotator(0.0f, (GetActorLocation() - SpawnLocation).ToOrientationRotator().Yaw + 180.0f, 0.0f);

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	ASFPlatform* NewPlatform = World->SpawnActorDeferred<ASFPlatform>(PlatformClass, SpawnTransform);
	NewPlatform->AssetsIndexes = AssetsIndexes;
	NewPlatform->FinishSpawning(SpawnTransform);
	if (!NewPlatform) return;

	NewPlatform->ParentZ = GetActorLocation().Z;
	NewPlatform->Speed = FMath::RandRange(NewPlatform->Speed, NewPlatform->Speed + 100.0f);
	NewPlatform->Speed = FMath::RandBool() ? NewPlatform->Speed : -NewPlatform->Speed;
	NewPlatform->Threshold = FMath::RandRange(NewPlatform->Threshold - ThresholdOffset, NewPlatform->Threshold);

	// UE_LOG(LogPlatform, Display, TEXT("%f %f"), NewPlatform->PlatformMesh->Bounds.BoxExtent.X, NewPlatform->PlatformMesh->Bounds.BoxExtent.Y);

	// Target
	if (FMath::RandBool()) SpawnTarget(World, Player, NewPlatform);
}

void ASFPlatform::ScoringPoints(ABaseCharacter* Player, float Points)
{
	USFProgressComponent* ProgressComponent = Cast<USFProgressComponent>(Player->GetComponentByClass(USFProgressComponent::StaticClass()));
	if (!ProgressComponent) return;

	ProgressComponent->AddScore(Points);
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

void ASFPlatform::Despawner(float DeltaTime)
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
	if (!GetWorld()) return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	const auto Player = Cast<ABaseCharacter>(PlayerController->GetPawn());
	if (Player)
	{
		if (FMath::Abs((Player->GetActorLocation() - GetActorLocation()).Size()) > DespawnDist)
		{
			bDespawned = true;
		}
	}

	// Horizontal moving
	Speed = (Offset >= Threshold || Offset <= -Threshold) ? -Speed : Speed;
	float CurrentSpeed = Speed * (MinSpeed + (1 - MinSpeed) * (1 - FMath::Abs(Offset) / Threshold)); // Smooth speed
	float CurrentOffset = CurrentSpeed * DeltaTime;

	// Vertical moving
	float Time = GetWorld()->GetTimeSeconds() + LocalTime;
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = ParentZ + Amplitude * FMath::Sin(Frequency * Time);
	
	// Set new location
	SetActorLocation(GetActorRightVector() * CurrentOffset + NewLocation);

	Offset += CurrentOffset;
	// UE_LOG(LogPlatform, Display, TEXT("Speed %f"), CurrentSpeed);
	// UE_LOG(LogPlatform, Display, TEXT("Offset %f"), Offset);
}

void ASFPlatform::SpawnTarget(UWorld* World, ABaseCharacter* Player, ASFPlatform* NewPlatform)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// Spawn Coords
	FVector TargetLocation = NewPlatform->GetActorLocation();
	TargetLocation.X += NewPlatform->PlatformMesh->Bounds.BoxExtent.X + FMath::RandRange(50.0f, 75.0f);
	TargetLocation.Y += FMath::RandRange(
		-NewPlatform->PlatformMesh->Bounds.BoxExtent.Y - FMath::RandRange(50.0f, 75.0f),
		NewPlatform->PlatformMesh->Bounds.BoxExtent.Y + FMath::RandRange(50.0f, 75.0f)
	);
	TargetLocation.Z += FMath::RandRange(150.0f, 700.0f);

	// Rotate to spawned platform
	FRotator TargetRotation = (NewPlatform->GetActorLocation() - TargetLocation).ToOrientationRotator();

	// Spawn and attach to spawned platform
	const auto NewTarget = World->SpawnActor<ASFTarget>(TargetClass, TargetLocation, TargetRotation, SpawnParams);
	if (!NewTarget) return;
	NewTarget->AttachToActor(NewPlatform, FAttachmentTransformRules::KeepWorldTransform);
}

// Move fog to player after step on platform
void ASFPlatform::FogConnecting()
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
		// UE_LOG(LogPlatform, Display, TEXT("Added"));
		PlatformMesh->OnComponentHit.AddDynamic(FogInst, &ASF_FloatFog::Mover);
	}
}
