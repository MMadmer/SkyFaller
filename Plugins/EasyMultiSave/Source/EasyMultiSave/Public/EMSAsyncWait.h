//Easy Multi Save - Copyright (C) 2022 by Michael Hegemann.  

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EMSData.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EMSAsyncWait.generated.h"

class UEMSObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncWaitOutputPin);

UCLASS()
class EASYMULTISAVE_API UEMSAsyncWait : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UEMSAsyncWait();

	virtual void Activate() override;

	EAsyncCheckType Type;

	UPROPERTY(BlueprintAssignable)
	FAsyncWaitOutputPin OnCompleted;

	/**
	* Wait until SaveGameActors or LoadGameActors have been completed.
	* For example, this is useful if you want access loaded variables in a BeginPlay event instead of ActorLoaded.
	* 
	* @param WorldContextObject - World context.
	* @param CheckType - The type of Async operation to wait for.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Actors",
		meta = (DisplayName = "Wait For Save or Load Completed", BlueprintInternalUseOnly = "true", WorldContext =
			"WorldContextObject"))
	static UEMSAsyncWait* AsyncWaitForOperation(UObject* WorldContextObject,
	                                            EAsyncCheckType CheckType = EAsyncCheckType::CT_Load);

private:
	UPROPERTY()
	UObject* WorldContextObject;

	UPROPERTY()
	UEMSObject* EMS;

	void StartWaitTask();
	void CompleteWaitTask();
	void ForceProceed();

	uint8 bSaveOrLoadTaskWasActive : 1;
};
