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

	UPROPERTY(BlueprintReadWrite, Category="Mesh Info")
	UStaticMesh* StaticMesh;

	UPROPERTY(BlueprintReadWrite, Category="Mesh Info")
	TArray<UMaterialInterface*> Materials;

	FUniqueMesh()
	{
		StaticMesh = nullptr;
		Materials = TArray<UMaterialInterface*>();
	}

	explicit FUniqueMesh(UStaticMesh* NewMesh)
	{
		StaticMesh = NewMesh;
		for (const auto& StaticMaterial : NewMesh->GetStaticMaterials())
		{
			if (!StaticMaterial.MaterialInterface) continue;
			Materials.Add(StaticMaterial.MaterialInterface);
		}
	}

	FUniqueMesh(UStaticMesh* NewMesh, const TArray<UMaterialInterface*>& NewMaterials)
	{
		StaticMesh = NewMesh;
		Materials = NewMaterials;
	}

	bool operator==(const FUniqueMesh& Other) const
	{
		return StaticMesh == Other.StaticMesh && Materials == Other.Materials;
	}

	friend uint32 GetTypeHash(const FUniqueMesh& Other)
	{
		uint32 MaterialHash = 0;
		for (const auto& Material : Other.Materials)
		{
			MaterialHash = HashCombine(MaterialHash, GetTypeHash(Material));
		}

		return HashCombine(GetTypeHash(Other.StaticMesh), MaterialHash);
	}
};
