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
	/*const auto Mesh = Meshes.IsValidIndex(0) ? Meshes[FMath::RandRange(0, Meshes.Num() - 1)] : nullptr;
	if (!Mesh)
	{
		UE_LOG(LogPlatform, Warning, TEXT("No mesh"));
		return;
	}
	PlatformMesh->SetStaticMesh(Mesh);*/
	const auto Asset = Assets.IsValidIndex(0) ? Assets[FMath::RandRange(0, Assets.Num() - 1)] : FAssets();
	if (!Asset.Platform || !Asset.Skin)
	{
		UE_LOG(LogPlatform, Warning, TEXT("No mesh"));
		return;
	}
	PlatformMesh->SetStaticMesh(Asset.Platform);
	SkinMesh->SetStaticMesh(Asset.Skin);
}

void ASFPlatform::SpawnNext(UWorld* World, ABaseCharacter* Player)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Get the location and rotation of the new platform
	FVector SpawnLocation = GetActorLocation();
	// UE_LOG(LogPlatform, Display, TEXT("%f %f"), PlatformMesh->Bounds.BoxExtent.X, PlatformMesh->Bounds.BoxExtent.Y);
	SpawnLocation.Y += FMath::RandRange(-PlatformMesh->Bounds.BoxExtent.Y * MaxRightOffset, PlatformMesh->Bounds.BoxExtent.Y * MaxRightOffset);
	SpawnLocation.Z += SpawnHeight;
	FRotator SpawnRotation = GetActorRotation();// (GetActorLocation() - SpawnLocation).ToOrientationRotator();

	ASFPlatform* NewPlatform = World->SpawnActor<ASFPlatform>(PlatformClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!NewPlatform) return;

	NewPlatform->ParentZ = GetActorLocation().Z;
	NewPlatform->Speed = FMath::RandRange(NewPlatform->Speed, NewPlatform->Speed + 100.0f);
	NewPlatform->Speed = FMath::RandBool() ? NewPlatform->Speed : -NewPlatform->Speed;
	NewPlatform->Threshold = FMath::RandRange(NewPlatform->Threshold - ThresholdOffset, NewPlatform->Threshold);

	// Move to normal X
	// UE_LOG(LogPlatform, Display, TEXT("Spawned to far"));
	FVector CorrectLocation = NewPlatform->GetActorLocation();
	CorrectLocation.X = GetActorLocation().X + PlatformMesh->Bounds.BoxExtent.X + NewPlatform->PlatformMesh->Bounds.BoxExtent.X + FMath::RandRange(MinDist, MaxDist);
	NewPlatform->SetActorLocation(CorrectLocation);

	FRotator CorrectRotation = FRotator(0.0f, (GetActorLocation() - NewPlatform->GetActorLocation()).ToOrientationRotator().Yaw + 180.0f, 0.0f);
	NewPlatform->SetActorRotation(CorrectRotation);

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
