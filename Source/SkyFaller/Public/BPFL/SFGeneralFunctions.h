// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SFGeneralFunctions.generated.h"

/**
 * 
 */
UCLASS()
class SKYFALLER_API USFGeneralFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Spawn actor by class name. */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="World", Keywords="Spawn, Actor, By, Class, Name"))
	static AActor* SpawnActorByClassName(UWorld* World, const FString& ClassName, const FVector& SpawnLocation,
	                                     const FRotator& SpawnRotation);
};
