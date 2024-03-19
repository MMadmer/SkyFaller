// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SFEditorToolsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USFEditorToolsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKYFALLER_API ISFEditorToolsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category="Editor Tools")
	UStaticMesh* GetStaticMesh();

	UFUNCTION(BlueprintNativeEvent, Category="Editor Tools")
	void SetStaticMesh(UStaticMesh* StaticMesh);
};
