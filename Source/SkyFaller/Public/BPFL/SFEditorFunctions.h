// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SFEditorFunctions.generated.h"

/**
 * 
 */
UCLASS()
class SKYFALLER_API USFEditorFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Create blueprint asset based on level actor instance. Auto-rename if asset name already exists.
	 *  Do nothing and print to log if directory path doesn't exists.
	 */
	UFUNCTION(BlueprintCallable)
	static UBlueprint*
	CreateBlueprintFromActorInstance(AActor* ActorInstance, const FString& Path = "/Game/",
	                                 const FString& Name = "BP_ActorBlueprint");
};
