// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTraceAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSingleTraceFinished, const FHitResult&, Hit);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiTraceFinished, const TArray<FHitResult>&, Hits);

/**
 * 
 */
UCLASS()
class USEFULTOOLS_API UAsyncSingleTraceAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnSingleTraceFinished OnSingleTraceFinished;

	/** Async trace for this frame. Guaranteed to be completed by the next frame.*/
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, meta=(WorldContext="Context"))
	static UAsyncSingleTraceAction* AsyncSingleLineTraceByChannel(const UObject* Context, const FVector Start,
	                                                              const FVector End,
	                                                              const ECollisionChannel Collision = ECC_Camera,
	                                                              const bool TraceComplex = true);

protected:
	virtual void Activate() override;

private:
	TWeakObjectPtr<UWorld> World;
	FVector StartLoc;
	FVector EndLoc;
	ECollisionChannel CollisionChannel;
	uint8 bTraceComplex : 1;

	FTraceDelegate TraceDelegate;
};

/**
 * 
 */
UCLASS()
class USEFULTOOLS_API UAsyncMultiTraceAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnMultiTraceFinished OnMultiTraceFinished;

	/** Async traces for this frame. Guaranteed to be completed by the next frame.*/
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, meta=(WorldContext="Context"))
	static UAsyncMultiTraceAction* AsyncMultiLineTraceByChannel(const UObject* Context, const FVector Start,
	                                                            const FVector End,
	                                                            const ECollisionChannel Collision = ECC_Camera,
	                                                            const bool TraceComplex = true);

protected:
	virtual void Activate() override;

private:
	TWeakObjectPtr<UWorld> World;
	FVector StartLoc;
	FVector EndLoc;
	ECollisionChannel CollisionChannel;
	uint8 bTraceComplex : 1;

	FTraceDelegate TraceDelegate;
};
