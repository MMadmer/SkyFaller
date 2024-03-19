// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFPlatformMesh.generated.h"

UCLASS()
class SKYFALLER_API ASFPlatformMesh : public AActor
{
	GENERATED_BODY()

public:
	ASFPlatformMesh();

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<bool> TestArray;
	
	virtual void BeginPlay() override;
};
