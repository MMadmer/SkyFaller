// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UTEnumIterateAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnumIteration, const uint8, EnumValue);

/**
 * 
 */
UCLASS()
class USEFULTOOLS_API UUTEnumIterateAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnEnumIteration OnEnumIteration;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static UUTEnumIterateAction* IterateByEnum(UEnum* Enum, const bool SkipMax = true);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static UUTEnumIterateAction* IterateByEnumReverse(UEnum* Enum, const bool SkipMax = true);

protected:
	virtual void Activate() override;

private:
	TWeakObjectPtr<UEnum> EnumToIterate;
	uint8 bSkipMax : 1;
	uint8 bReverse : 1;

	void IterateByEnum_Internal() const;
	void IterateByEnumReverse_Internal() const;
};
