// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFL/UsefulFunctions.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/Texture2DDynamic.h"
#include "Engine/TextureRenderTarget2D.h"

UTexture2D* UUsefulFunctions::ConvertDynamicToTexture2D(const UTexture2DDynamic* DynamicTexture)
{
	if (!DynamicTexture) return nullptr;

	// Make render target
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
	RenderTarget->InitAutoFormat(DynamicTexture->SizeX, DynamicTexture->SizeY);
	RenderTarget->UpdateResourceImmediate(true);

	// Get texture resource
	const FTexture* Texture = DynamicTexture->GetResource();
	if (!Texture) return nullptr;

	// Draw texture
	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FCanvas Canvas(RenderTargetResource, nullptr, 0, 0, 0, GMaxRHIFeatureLevel);

	// Make TileItem
	FCanvasTileItem TileItem(FVector2D(0, 0), Texture, FVector2D(DynamicTexture->SizeX, DynamicTexture->SizeY),
	                         FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Opaque;
	Canvas.DrawItem(TileItem);
	Canvas.Flush_GameThread();

	// Cope render target data
	TArray<FColor> Pixels;
	RenderTargetResource->ReadPixels(Pixels);

	// Make UTexture2D
	UTexture2D* NewTexture = UTexture2D::CreateTransient(DynamicTexture->SizeX, DynamicTexture->SizeY, PF_B8G8R8A8);
	if (!NewTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create UTexture2D!"));
		return nullptr;
	}

	// Draw UTexture2D
	void* TextureData = NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Pixels.GetData(), Pixels.Num() * sizeof(FColor));
	NewTexture->PlatformData->Mips[0].BulkData.Unlock();

	NewTexture->UpdateResource();
	RenderTarget->ConditionalBeginDestroy();

	return NewTexture;
}

FVector2D UUsefulFunctions::GetViewportMousePosition(UObject* WorldContext)
{
	if (!WorldContext) return FVector2D::ZeroVector;

	const auto ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(WorldContext);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(WorldContext);

	FVector2D PixelPosition{};
	FVector2D ViewportPosition{};
	USlateBlueprintLibrary::LocalToViewport(WorldContext, ViewportGeometry, MousePosition, PixelPosition,
	                                        ViewportPosition);

	return PixelPosition;
}

APlayerController* UUsefulFunctions::GetLocalPlayerController(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	if (!World) return nullptr;

	const ULocalPlayer* LocalPlayerController = World->GetFirstLocalPlayerFromController();
	return LocalPlayerController ? LocalPlayerController->PlayerController : nullptr;
}

void UUsefulFunctions::DestroyAttachedActorsByClass(const AActor* OwnerActor, const TSubclassOf<AActor> ActorClass)
{
	if (!OwnerActor) return;

	TArray<AActor*> AttachedActors;
	OwnerActor->GetAttachedActors(AttachedActors);

	for (const auto& AttachedActor : AttachedActors)
	{
		if (AttachedActor->GetClass()->IsChildOf(ActorClass)) AttachedActor->Destroy();
	}
}

void UUsefulFunctions::DestroyComponentsByClass(const AActor* OwnerActor,
                                                const TSubclassOf<UActorComponent> ComponentClass)
{
	if (!OwnerActor) return;

	TArray<UActorComponent*> Components;
	OwnerActor->GetComponents<UActorComponent>(Components);

	for (const auto& Component : Components)
	{
		if (Component->GetClass()->IsChildOf(ComponentClass)) Component->DestroyComponent();
	}
}
