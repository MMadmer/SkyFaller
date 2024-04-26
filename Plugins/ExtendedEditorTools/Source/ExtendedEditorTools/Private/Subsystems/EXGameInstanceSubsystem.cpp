// Sky Faller. All rights reserved.


#include "Subsystems/EXGameInstanceSubsystem.h"

void UEXGameInstanceSubsystem::Array_RegisterRandom(const TArray<int32>& TargetArray)
{
}

/*void UEXGameInstanceSubsystem::GetRandomItem(const TArray<int32>& TargetArray, int32& OutItem, int32& OutIndex)
{
}*/

void UEXGameInstanceSubsystem::GenericArray_RegisterRandom(const UObject* WorldContext, void* TargetArray,
                                                           const FArrayProperty* ArrayProp)
{
	if (!TargetArray || !WorldContext) return;

	const auto World = WorldContext->GetWorld();
	if (!World) return;

	const auto ThisSubsystem = UGameInstance::GetSubsystem<UEXGameInstanceSubsystem>(World->GetGameInstance());
	if (!ThisSubsystem) return;

	const FScriptArrayHelper ArrayHelper{ArrayProp, TargetArray};
	const TBitArray<> NewIndexes{false, ArrayHelper.Num()};
	ThisSubsystem->RandomPairs.Add(TargetArray, NewIndexes);

	UE_LOG(LogTemp, Display, TEXT("Random pair added: %d"), ThisSubsystem->RandomPairs.Num());
}
