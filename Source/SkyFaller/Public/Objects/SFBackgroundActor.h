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

	/** Distance that adds to the layer object Y pos for spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DistY;

	/** Distance that adds to the layer object X pos for spawn next */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DistX;

	/** Distance that adds to the layer object Z pos for first spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DistZ;

	/** Distance between layer objects */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BetweenX;

	/** First spawn only */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ObjectsNum;

	/** Swing frequency */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	float Frequency = 0.5f;

	/** Swing amplitude */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,  meta = (ClampMin = "0"))
	float Amplitude = 300.0f;

	/** Distance that adds to the layer object Z pos for spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpawnHeight = -2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	float SpawnSpeed = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DespawnDist = 100000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DespawnSpeed = 200.0f;

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
	void SetParentZ(float NewZ) { ParentZ = NewZ; };
	ASFBackgroundActor* SpawnNext(TSubclassOf<ASFBackgroundActor> BackgroundClass);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Assets")
	TArray<FBackAssets> BackLayers;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BackMesh;

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	int32 Layer = 0;
	float ParentZ = 0.0f;
	float LocalTime = 0.0f;
	bool bSpawned = true;
	bool bDespawned = false;

	void SetTemplate();
	void Mover(float DeltaTime);
	void Spawner(float DeltaTime);
	void Despawner(float DeltaTime);

};
