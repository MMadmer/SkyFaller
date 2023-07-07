// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFBackgroundActor.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FBackAssets
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DistY;

	/** Distance that adds to the layer object X pos for spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DistX;

	/** Distance between layer objects */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BetweenX;

	/** First spawn only */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ObjectsNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<UStaticMesh*> Assets;
};

UCLASS()
class SKYFALLER_API ASFBackgroundActor : public AActor
{
	GENERATED_BODY()
	
public:
	bool bMain = false; // Background actor is main or not

	ASFBackgroundActor();

	int32 GetLayer() { return Layer; };
	void SetLayer(int32 NewLayer) { Layer = NewLayer; };
	int32 GetBackLayers() { return BackLayers.Num(); };
	FBackAssets GetCurrentLayer() { return BackLayers[Layer]; }; // Layer struct
	int32 GetObjectsNumOfLayer(int32 LayerNum) { return LayerNum < BackLayers.Num() ? BackLayers[LayerNum].ObjectsNum : -1; };

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Assets")
	TArray<FBackAssets> BackLayers;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BackMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0"))
	float Frequency = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0"))
	float Amplitude = 10.0f;

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	int32 Layer = 0;
	float ParentZ = 0.0f;
	float LocalTime = 0.0f;

	void SetTemplate();
	void Mover(float DeltaTime);

};
