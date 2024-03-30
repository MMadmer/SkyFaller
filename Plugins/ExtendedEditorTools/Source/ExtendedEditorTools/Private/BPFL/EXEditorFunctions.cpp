// Sky Faller. All rights reserved.


#include "BPFL/EXEditorFunctions.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"

UBlueprint* UEXEditorFunctions::CreateBlueprintFromActorInstance(AActor* ActorInstance,
	const FString& Path,
	const FString& Name)
{
	UBlueprint* NewActorBlueprint = nullptr;
#if WITH_EDITORONLY_DATA

	// Actor instance is valid
	if (ActorInstance)
	{
		const UWorld* World = ActorInstance->GetWorld();

		if (World)
		{
			// Package path
			FString BlueprintName = Name + TEXT("_1");
			const FString PackagePath = Path;
			FString PackageName = FString::Printf(TEXT("%s%s"), *PackagePath, *BlueprintName);

			const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
				"AssetRegistry");
			const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

			// Validate path
			if (!AssetRegistry.PathExists(PackagePath))
			{
				UE_LOG(LogTemp, Warning, TEXT("Path invalid: %s"), *PackagePath);
				return nullptr;
			}

			// Validate asset name
			int32 NameCounter = 2;
			while (AssetRegistry.GetAssetByObjectPath(FName(*(PackageName + TEXT(".") + BlueprintName)), true).
				IsValid())
			{
				TArray<FString> Substrings;
				BlueprintName.ParseIntoArray(Substrings, TEXT("_"));

				BlueprintName = BlueprintName.LeftChop(Substrings.Last().Len()) + FString::FromInt(NameCounter++);

				PackageName = FString::Printf(TEXT("%s%s"), *PackagePath, *BlueprintName);
				UE_LOG(LogTemp, Display, TEXT("New path: %s"), *(PackageName + TEXT(".") + BlueprintName));
			}

			// Blueprint params
			FKismetEditorUtilities::FCreateBlueprintFromActorParams BlueprintFromActorParams{};
			BlueprintFromActorParams.bReplaceActor = false;
			BlueprintFromActorParams.bOpenBlueprint = false;

			// Create new blueprint
			NewActorBlueprint = FKismetEditorUtilities::CreateBlueprintFromActor(
				PackageName, ActorInstance, BlueprintFromActorParams);

			if (NewActorBlueprint)
			{
				// Save actor blueprint
				NewActorBlueprint->MarkPackageDirty();
				FAssetRegistryModule::AssetCreated(NewActorBlueprint);
				UE_LOG(LogTemp, Display, TEXT("Actor blueprint created: %s"), *NewActorBlueprint->GetPathName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to create actor blueprint: %s"), *PackageName);
			}
		}
	}

#endif
	return NewActorBlueprint;
}

