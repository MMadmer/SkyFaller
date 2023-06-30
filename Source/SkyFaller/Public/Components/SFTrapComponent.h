// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFTrapComponent.generated.h"

class ASFTrap;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYFALLER_API USFTrapComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USFTrapComponent();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Traps")
	TArray<TSubclassOf<ASFTrap>> Traps;

	virtual void BeginPlay() override;

private:
	void SpawnTraps();
	void AttachTrapToSocket(ASFTrap* Trap, USceneComponent* SceneComponent, const FName& SocketName);
		
};
