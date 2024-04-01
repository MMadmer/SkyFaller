// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logging/LogVerbosity.h"
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
	UFUNCTION(BlueprintCallable, Category="Extended Editor Tools",
		meta=(Keywords="Create Blueprint From Actor Instance"))
	static UBlueprint*
	CreateBlueprintFromActorInstance(AActor* ActorInstance, const FString& Path = "/Game/",
	                                 const FString& Name = "BP_ActorBlueprint");

	/** Send message notify to editor window and into output log.
	 *  LogVerbosity: 4 is Display, 3 is Warning, 2 is Error. Default Info. Out of range is info.
	 */
	UFUNCTION(BlueprintCallable, Category="Extended Editor Tools", meta=(Keywords="Notify With Log"))
	static void NotifyWithLog(const FString Message, const uint8& LogVerbosity = 4, const float Duration = 1.0f);
};
