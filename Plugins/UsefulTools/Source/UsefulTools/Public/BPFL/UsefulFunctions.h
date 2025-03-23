// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UsefulFunctions.generated.h"

/**
 * 
 */
UCLASS()
class USEFULTOOLS_API UUsefulFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Useful Tools|Function Library", meta=(BlueprintAutocast, CompactNodeTitle="->",
		Keywords="Convert|Dynamic|Texture|to|Texture2D|2D|Texture"))
	static UTexture2D* ConvertDynamicToTexture2D(const UTexture2DDynamic* DynamicTexture);

	UFUNCTION(BlueprintPure, Category="Useful Tools|Player", meta=(WorldContext="WorldContext",
		Keywords="Get|Viewport|Mouse|Position|Cursor|Location|XY|Coordinates|Coordinate|Player"))
	static FVector2D GetViewportMousePosition(UObject* WorldContext);

	/**
	 * Returns player controller by this rule:
	 * Client - first local player controller;
	 * Listen server - first player controller(host);
	 * Server - nullptr.
	 */
	UFUNCTION(BlueprintPure, Category="Useful Tools|Player", meta=(WorldContext="WorldContext",
		ReturnDisplayName="LocalPlayer", Keywords="Get|Local|Player|Controller|PlayerController"))
	static APlayerController* GetLocalPlayerController(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category="Useful Tools")
	static void DestroyAttachedActorsByClass(const AActor* OwnerActor, const TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable, Category="Useful Tools")
	static void DestroyComponentsByClass(const AActor* OwnerActor, const TSubclassOf<UActorComponent> ComponentClass);
};
