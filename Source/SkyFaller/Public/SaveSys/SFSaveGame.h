// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SFSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSaveDataRecord
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	UClass* ActorClass;

	UPROPERTY(SaveGame)
	FString ActorName;

	UPROPERTY(SaveGame)
	FTransform ActorTransform;

	UPROPERTY(SaveGame)
	TArray<uint8> BinaryData;
};

UCLASS()
class SKYFALLER_API USFSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<uint8> ByteData;
};
