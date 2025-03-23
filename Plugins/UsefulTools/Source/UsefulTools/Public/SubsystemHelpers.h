#pragma once

template <typename T>
typename TEnableIf<TIsDerivedFrom<T, UGameInstanceSubsystem>::Value, T*>::Type
GetSubsystem(const UObject* InWorldContextObject)
{
	const UWorld* LWorld = GEngine ? GEngine->GetWorldFromContextObjectChecked(InWorldContextObject) : nullptr;
	UGameInstance* GameInstance = LWorld ? LWorld->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<T>() : nullptr;
}

template <typename T>
typename TEnableIf<TIsDerivedFrom<T, UWorldSubsystem>::Value, T*>::Type
GetSubsystem(const UObject* InWorldContextObject)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObjectChecked(InWorldContextObject) : nullptr;
	return World ? World->GetSubsystem<T>() : nullptr;
}

template <typename T>
typename TEnableIf<TIsDerivedFrom<T, UEngineSubsystem>::Value, T*>::Type
GetSubsystem(const UObject* InWorldContextObject = nullptr)
{
	return GEngine ? GEngine->GetEngineSubsystem<T>() : nullptr;
}
