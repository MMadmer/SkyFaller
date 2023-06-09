// Sky Faller. All rights reserved.


#include "Objects/SFPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Player/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "SFListener.h"
#include "Components/SFProgressComponent.h"
#include "Objects/SFTarget.h"
#include "Objects/SFPlatformSkin.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SFTrapComponent.h"
#include "..\..\Public\Objects\SFPlatform.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlatform, All, All)

ASFPlatform::ASFPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlatformMesh");
	SkinMesh = CreateDefaultSubobject<UStaticMeshComponent>("SkinMesh");
	SetRootComponent(PlatformMesh);
	SkinMesh->SetupAttachment(PlatformMesh);

	TrapComponent = CreateDefaultSubobject<USFTrapComponent>("TrapComponent");
}

void ASFPlatform::BeginPlay()
{
	Super::BeginPlay();

	SetTemplate();

	TrapComponent->SpawnTraps();

	PlatformMesh->OnComponentHit.AddDynamic(this, &ASFPlatform::OnHit);

	ListenerConnecting();

	LocalTime = (float)FMath::RandHelper(10);
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
	float NextAngle = 0.0f;
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
	FVector SpawnLocation = GetActorLocation() + (GetActorRotation() + SpawnDirection).Vector() * (MESH_DIAMETER + FMath::RandRange(MinDist, MaxDist)); // Get end point of new platform spawn location
	if ((SpawnLocation.Y > ZeroY + SpawnY) || (SpawnLocation.Y < ZeroY - SpawnY))
	{
		SpawnLocation.Y > ZeroY + SpawnY ? SpawnDirection.Yaw = -SpawnAngle : SpawnDirection.Yaw = SpawnAngle;
		SpawnLocation = GetActorLocation() + (GetActorRotation() + SpawnDirection).Vector() * (MESH_DIAMETER + FMath::RandRange(MinDist, MaxDist)); // Invisible spawn borders
	}
	SpawnLocation.Z += SpawnHeight;

	FRotator SpawnRotation = FRotator(0.0f, (GetActorLocation() - SpawnLocation).ToOrientationRotator().Yaw + 180.0f, 0.0f);

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	GlobalRotation += SpawnDirection.Yaw;

	ASFPlatform* NewPlatform = World->SpawnActorDeferred<ASFPlatform>(PlatformClass, SpawnTransform);
	NewPlatform->SelfID = SelfID + 1;
	NewPlatform->ZeroY = ZeroY;
	NewPlatform->AssetsIndexes.Append(AssetsIndexes);
	NewPlatform->FinishSpawning(SpawnTransform);
	if (!NewPlatform) return;

	NewPlatform->ParentZ = GetActorLocation().Z;
	NewPlatform->Speed = FMath::RandRange(NewPlatform->Speed, NewPlatform->Speed + 100.0f);
	NewPlatform->Speed = FMath::RandBool() ? NewPlatform->Speed : -NewPlatform->Speed;
	NewPlatform->Threshold = FMath::RandRange(NewPlatform->Threshold - ThresholdOffset, NewPlatform->Threshold);
	NewPlatform->GlobalRotation = GlobalRotation;

	// UE_LOG(LogPlatform, Display, TEXT("Global rotation:  %f"), GlobalRotation);

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
		if (FMath::Abs((Player->GetActorLocation() - GetActorLocation()).Size()) > DespawnDist && bTouched)
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
	if (ListenerInst)
	{
		// UE_LOG(LogPlatform, Display, TEXT("Listener added"));
		PlatformMesh->OnComponentHit.AddDynamic(ListenerInst, &ASFListener::OnPlatformHit);
	}
}
