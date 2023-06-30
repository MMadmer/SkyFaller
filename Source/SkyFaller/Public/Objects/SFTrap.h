// Sky Faller. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFTrap.generated.h"

class UStaticMeshComponent;

UCLASS()
class SKYFALLER_API ASFTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ASFTrap();

	FName GetSocketName() const { return SocketName; };

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* TrapMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FName SocketName;

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
