// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SF_FloatFog.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASF_FloatFog : public AActor
{
	GENERATED_BODY()
	
public:	
	ASF_FloatFog();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* FogMesh;

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	FVector GetPlayerLocation() const;
	void Mover();
};
