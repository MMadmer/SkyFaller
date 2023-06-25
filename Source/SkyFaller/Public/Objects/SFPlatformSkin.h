// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFPlatformSkin.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASFPlatformSkin : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFPlatformSkin();

	void SetSckinMesh(UStaticMesh* NewMesh) { SkinMesh->SetStaticMesh(NewMesh); };

protected:

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* SkinMesh;

	virtual void BeginPlay() override;

};
