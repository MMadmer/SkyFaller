// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/AsyncTraceAction.h"

UAsyncSingleTraceAction* UAsyncSingleTraceAction::AsyncSingleLineTraceByChannel(
	const UObject* Context, const FVector Start, const FVector End, const ECollisionChannel Collision,
	const bool TraceComplex)
{
	UAsyncSingleTraceAction* AsyncTraceAction = NewObject<UAsyncSingleTraceAction>();
	AsyncTraceAction->World = IsValid(Context) ? Context->GetWorld() : nullptr;
	AsyncTraceAction->StartLoc = Start;
	AsyncTraceAction->EndLoc = End;
	AsyncTraceAction->CollisionChannel = Collision;
	AsyncTraceAction->bTraceComplex = TraceComplex;

	return AsyncTraceAction;
}

void UAsyncSingleTraceAction::Activate()
{
	Super::Activate();

	if (!World.IsValid()) return;

	TraceDelegate.BindLambda([&](const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum)
	{
		OnSingleTraceFinished.Broadcast(TraceDatum.OutHits.Num() > 0 ? TraceDatum.OutHits[0] : FHitResult{});
		SetReadyToDestroy();
	});

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = bTraceComplex;
	QueryParams.bReturnFaceIndex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	World->AsyncLineTraceByChannel(EAsyncTraceType::Single, StartLoc, EndLoc, CollisionChannel, QueryParams,
	                               FCollisionResponseParams{}, &TraceDelegate);
}

UAsyncMultiTraceAction* UAsyncMultiTraceAction::AsyncMultiLineTraceByChannel(
	const UObject* Context, const FVector Start, const FVector End, const ECollisionChannel Collision,
	const bool TraceComplex)
{
	UAsyncMultiTraceAction* AsyncTraceAction = NewObject<UAsyncMultiTraceAction>();
	AsyncTraceAction->World = IsValid(Context) ? Context->GetWorld() : nullptr;
	AsyncTraceAction->StartLoc = Start;
	AsyncTraceAction->EndLoc = End;
	AsyncTraceAction->CollisionChannel = Collision;
	AsyncTraceAction->bTraceComplex = TraceComplex;

	return AsyncTraceAction;
}

void UAsyncMultiTraceAction::Activate()
{
	Super::Activate();

	if (!World.IsValid()) return;

	TraceDelegate.BindLambda([&](const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum)
	{
		OnMultiTraceFinished.Broadcast(TraceDatum.OutHits);
		SetReadyToDestroy();
	});

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = bTraceComplex;
	QueryParams.bReturnFaceIndex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	World->AsyncLineTraceByChannel(EAsyncTraceType::Single, StartLoc, EndLoc, CollisionChannel, QueryParams,
	                               FCollisionResponseParams{}, &TraceDelegate);
}
