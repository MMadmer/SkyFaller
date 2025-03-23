// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/UTEnumIterateAction.h"

UUTEnumIterateAction* UUTEnumIterateAction::IterateByEnum(UEnum* Enum, const bool SkipMax)
{
	const auto Action = NewObject<UUTEnumIterateAction>();
	if (!IsValid(Action)) return nullptr;

	Action->EnumToIterate = Enum;
	Action->bSkipMax = SkipMax;
	Action->bReverse = false;

	return Action;
}

UUTEnumIterateAction* UUTEnumIterateAction::IterateByEnumReverse(UEnum* Enum, const bool SkipMax)
{
	const auto Action = NewObject<UUTEnumIterateAction>();
	if (!IsValid(Action)) return nullptr;

	Action->EnumToIterate = Enum;
	Action->bSkipMax = SkipMax;
	Action->bReverse = true;

	return Action;
}

void UUTEnumIterateAction::Activate()
{
	Super::Activate();

	if (!bReverse) IterateByEnum_Internal();
	else IterateByEnumReverse_Internal();
}

void UUTEnumIterateAction::IterateByEnum_Internal() const
{
	if (!EnumToIterate.IsValid() || EnumToIterate->NumEnums() == 0) return;

	for (int32 Index = 0; Index < EnumToIterate->NumEnums(); ++Index)
	{
		//if (!EnumToIterate->HasMetaData(TEXT("Hidden"), Index)) // Skip hidden values
		//{
		const int64 EnumValue = EnumToIterate->GetValueByIndex(Index);
		if (bSkipMax && EnumValue == EnumToIterate->GetMaxEnumValue()) continue;

		const uint8 EnumValueAsByte = static_cast<uint8>(EnumValue);

		OnEnumIteration.Broadcast(EnumValueAsByte);
		//}
	}
}

void UUTEnumIterateAction::IterateByEnumReverse_Internal() const
{
	if (!EnumToIterate.IsValid() || EnumToIterate->NumEnums() == 0) return;

	for (int32 Index = EnumToIterate->NumEnums() - 1; Index >= 0; --Index)
	{
		//if (!EnumToIterate->HasMetaData(TEXT("Hidden"), Index)) // Skip hidden values
		//{
		const int64 EnumValue = EnumToIterate->GetValueByIndex(Index);
		if (bSkipMax && EnumValue == EnumToIterate->GetMaxEnumValue()) continue;

		const uint8 EnumValueAsByte = static_cast<uint8>(EnumValue);

		OnEnumIteration.Broadcast(EnumValueAsByte);
		//}
	}
}
