// Sky Faller. All rights reserved.


#include "BPFL/SFGeneralFunctions.h"

AActor* USFGeneralFunctions::SpawnActorByClassName(UWorld* World, const FString& ClassName,
                                                   const FVector& SpawnLocation,
                                                   const FRotator& SpawnRotation)
{
	// Load class by name
	UClass* ActorClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassName);

	if (ActorClass && ActorClass->IsChildOf(AActor::StaticClass()))
	{
		return World->SpawnActor<AActor>(ActorClass, SpawnLocation, SpawnRotation);
	}

	// Not valid class name or class itself
	UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor: %s"), *ClassName);
	return nullptr;
}
