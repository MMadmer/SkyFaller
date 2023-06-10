// Sky Faller. All rights reserved.


#include "Objects/SFPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Player/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlatform, All, All)

// Sets default values
ASFPlatform::ASFPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlatformMesh");
	SetRootComponent(PlatformMesh);
}

// Called when the game starts or when spawned
void ASFPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	PlatformMesh->OnComponentHit.AddDynamic(this, &ASFPlatform::SpawnNext);
}

// Called every frame
void ASFPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Spawned ? Spawner(DeltaTime) : Mover(DeltaTime);
}

void ASFPlatform::SpawnNext(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto World = GetWorld();
	if (!World || bTouched) return;

	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;

	// Spawn new platform
	bTouched = true;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Get the location and rotation of the new platform
	FVector SpawnLocation = GetActorLocation();
	// UE_LOG(LogPlatform, Display, TEXT("%f %f"), PlatformMesh->Bounds.BoxExtent.X, PlatformMesh->Bounds.BoxExtent.Y);
	SpawnLocation.X += PlatformMesh->Bounds.BoxExtent.X * 2 + FMath::RandRange(150.0f, 450.0f);
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
