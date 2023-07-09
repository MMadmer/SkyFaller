// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFListener.generated.h"

class ASFPlatform;
class ASFBackgroundActor;
class ACharacter;

UCLASS()
class SKYFALLER_API ASFListener : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFListener();

	UFUNCTION()
	void OnPlatformHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Background")
	TSubclassOf<ASFBackgroundActor> BackgroundClass;

	virtual void BeginPlay() override;

private:
	ASFPlatform* MainPlatform;
	TArray<TArray<ASFBackgroundActor*>> BackgroundActors;
	int32 CachedId = -1;

	void FogMoving(const ACharacter* Player);
	void PlatformConnecting(int32 Index);
	void BackgroundInit();
	void BackgroundManage(const ACharacter* Player);
	bool BackgroundSpawner(TArray<ASFBackgroundActor*>& Layer, const ACharacter* Player, const int32 Index, bool bFront);
	bool BackgroundDespawner(TArray<ASFBackgroundActor*>& Layer, const ACharacter* Player, const int32 Index);

};
