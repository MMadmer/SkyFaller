// Sky Faller. All rights reserved.


#include "Objects/SFPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Player/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/SF_FloatFog.h"
#include "Components/SFProgressComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlatform, All, All)

ASFPlatform::ASFPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlatformMesh");
	SetRootComponent(PlatformMesh);
}

void ASFPlatform::BeginPlay()
{
	Super::BeginPlay();

	PlatformMesh->OnComponentHit.AddDynamic(this, &ASFPlatform::OnHit);

	FogConnecting();
}

void ASFPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Spawned ? Spawner(DeltaTime) : Mover(DeltaTime);
}

void ASFPlatform::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto World = GetWorld();
	if (!World || bTouched) return;

	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;

	bTouched = true;

	ScoringPoints(World, Player);
	SpawnNext(World, Player);
}

void ASFPlatform::SpawnNext(UWorld* World, ABaseCharacter* Player)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Get the location and rotation of the new platform
	FVector SpawnLocation = GetActorLocation();
	// UE_LOG(LogPlatform, Display, TEXT("%f %f"), PlatformMesh->Bounds.BoxExtent.X, PlatformMesh->Bounds.BoxExtent.Y);
	SpawnLocation.X += PlatformMesh->Bounds.BoxExtent.X * 2 + FMath::RandRange(150.0f, 550.0f);
	SpawnLocation.Y += FMath::RandRange(-PlatformMesh->Bounds.BoxExtent.Y * 2, PlatformMesh->Bounds.BoxExtent.Y * 2);
	SpawnLocation.Z += SpawnHeight;
	FRotator SpawnRotation = GetActorRotation();// (GetActorLocation() - SpawnLocation).ToOrientationRotator();

	ASFPlatform* NewPlatform = World->SpawnActor<ASFPlatform>(PlatformClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!NewPlatform) return;

	NewPlatform->ParentZ = GetActorLocation().Z;
	NewPlatform->Speed = FMath::RandRange(NewPlatform->Speed, NewPlatform->Speed + 100.0f);
	NewPlatform->Speed = FMath::RandBool() ? NewPlatform->Speed : -NewPlatform->Speed;
	NewPlatform->Threshold = FMath::RandRange(NewPlatform->Threshold - ThresholdOffset, NewPlatform->Threshold);
	// UE_LOG(LogPlatform, Display, TEXT("%f %f"), NewPlatform->PlatformMesh->Bounds.BoxExtent.X, NewPlatform->PlatformMesh->Bounds.BoxExtent.Y);
}

void ASFPlatform::ScoringPoints(UWorld* World, ABaseCharacter* Player)
{
	USFProgressComponent* ProgressComponent = Cast<USFProgressComponent>(Player->GetComponentByClass(USFProgressComponent::StaticClass()));
	if (!ProgressComponent) return;

	ProgressComponent->AddScore(5);
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

void ASFPlatform::Mover(float DeltaTime)
{
	Speed = (Offset >= Threshold || Offset <= -Threshold) ? -Speed : Speed;
	float CurrentOffset = Speed * DeltaTime;
	AddActorLocalOffset(GetActorRightVector() * CurrentOffset, false);
	Offset += CurrentOffset;
	// UE_LOG(LogPlatform, Display, TEXT("%f    %f"), Offset, Threshold);
}

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
