#pragma once

#include "EXCoreTypes.generated.h"

UENUM(BlueprintType)
enum EEXLogVerbosity
{
	/** Not used */
	NoLogging = 0,
	Error = 2,
	Warning = 3,
	Display = 4
};

USTRUCT(BlueprintType)
struct FUniqueMesh
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Info")
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Info")
	TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

	FORCEINLINE bool operator==(const FUniqueMesh& Other) const
	{
		return StaticMesh == Other.StaticMesh && Materials == Other.Materials;
	}

	friend uint32 GetTypeHash(const FUniqueMesh& Other)
	{
		const uint32 MeshHash = Other.StaticMesh ? GetTypeHash(Other.StaticMesh) : 0;
		const uint32 MaterialHash = GetTypeHash(&Other.Materials);

		return HashCombine(MeshHash, MaterialHash);
	}
};
