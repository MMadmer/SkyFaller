// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EXEditorFunctions.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDEDEDITORTOOLS_API UEXEditorFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Create blueprint asset based on level actor instance. Auto-rename if asset name already exists.
	 *  Do nothing and print to log if directory path doesn't exists.
	 */
	UFUNCTION(BlueprintCallable, Category="Extended Editor Tools")
	static UBlueprint*
	CreateBlueprintFromActorInstance(AActor* ActorInstance, const FString& Path = "/Game/",
	                                 const FString& Name = "BP_ActorBlueprint");
};
