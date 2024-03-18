// Sky Faller. All rights reserved.


#include "Tools/SFPlatformCreator.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ASFPlatformCreator::ASFPlatformCreator()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Handler = CreateDefaultSubobject<USphereComponent>(TEXT("Handler"));
}

void ASFPlatformCreator::BeginPlay()
{
	Super::BeginPlay();

	// Destroy();
}

void ASFPlatformCreator::ConvertToPlatform()
{
#if WITH_EDITORONLY_DATA
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	for (const auto& Actor : Actors)
	{
		if ((Actor->GetActorLocation() - GetActorLocation()).Size() > Handler->GetCollisionShape().GetSphereRadius() ||
			Actor == this)
			continue;

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s"), *Actor->GetName()));
	}
#endif
}
