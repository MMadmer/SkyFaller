// Sky Faller. All rights reserved.


#include "Components/SFStaticSkinComponent.h"
#include "Objects/SFPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Objects/SFPlatformSkin.h"

USFStaticSkinComponent::USFStaticSkinComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFStaticSkinComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnSkin();
}

void USFStaticSkinComponent::SpawnSkin()
{
	const auto World = GetWorld();
	if (!World) return;
	const auto Platform = Cast<ASFPlatform>(GetOwner());
	if (!Platform) return;
	auto SkinActor = World->SpawnActor<ASFPlatformSkin>(SkinClass);
	if (!Platform) return;

	// All checks done. Set weapon to current
	SkinActor->SetOwner(Platform);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	SkinActor->AttachToComponent(Platform->GetMesh(), AttachmentRules);

	CurrentSkin = SkinActor;
}
