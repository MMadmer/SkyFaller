//Easy Multi Save - Copyright (C) 2022 by Michael Hegemann.  

#include "EMSFunctionLibrary.h"


/**
Save Game Users
**/

void UEMSFunctionLibrary::SetCurrentSaveUserName(UObject* WorldContextObject, const FString& UserName)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		EMS->SetCurrentSaveUserName(UserName);
	}
}

void UEMSFunctionLibrary::DeleteSaveUser(UObject* WorldContextObject, const FString& UserName)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		EMS->DeleteAllSaveDataForUser(UserName);
	}
}

TArray<FString> UEMSFunctionLibrary::GetAllSaveUsers(const UObject* WorldContextObject)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		return EMS->GetAllSaveUsers();
	}

	return TArray<FString>();
}

/**
Save Slots
**/

void UEMSFunctionLibrary::SetCurrentSaveGameName(UObject* WorldContextObject, const FString& SaveGameName)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		EMS->SetCurrentSaveGameName(SaveGameName);
	}
}

TArray<FString> UEMSFunctionLibrary::GetSortedSaveSlots(const UObject* WorldContextObject)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		return EMS->GetSortedSaveSlots();
	}

	return TArray<FString>();
}

UEMSInfoSaveGame* UEMSFunctionLibrary::GetSlotInfoSaveGame(const UObject* WorldContextObject, FString& SaveGameName)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		SaveGameName = EMS->GetCurrentSaveGameName();
		return EMS->GetSlotInfoObject();
	}

	return nullptr;
}

UEMSInfoSaveGame* UEMSFunctionLibrary::GetNamedSlotInfo(const UObject* WorldContextObject, const FString& SaveGameName)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		return EMS->GetSlotInfoObject(SaveGameName);
	}

	return nullptr;
}

bool UEMSFunctionLibrary::DoesSaveSlotExist(const UObject* WorldContextObject, const FString& SaveGameName)
{
	if (SaveGameName.IsEmpty())
	{
		return false;
	}

	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		return EMS->DoesSaveGameExist(SaveGameName);
	}

	return false;
}

/**
File System
**/

void UEMSFunctionLibrary::DeleteAllSaveDataForSlot(UObject* WorldContextObject, const FString& SaveGameName)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		EMS->DeleteAllSaveDataForSlot(SaveGameName);
	}
}

/**
Thumbnail Saving
Simple saving as .png from a 2d scene capture render target source.
**/

UTexture2D* UEMSFunctionLibrary::ImportSaveThumbnail(UObject* WorldContextObject, const FString& SaveGameName)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		return EMS->ImportSaveThumbnail(SaveGameName);
	}

	return nullptr;
}

void UEMSFunctionLibrary::ExportSaveThumbnail(UObject* WorldContextObject, UTextureRenderTarget2D* TextureRenderTarget,
                                              const FString& SaveGameName)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		return EMS->ExportSaveThumbnail(TextureRenderTarget, SaveGameName);
	}
}

/**
Other Functions
**/

void UEMSFunctionLibrary::SetActorSaveProperties(UObject* WorldContextObject, const bool bSkipSave,
                                                 const bool bPersistent, const bool bSkipTransform)
{
	AActor* SaveActor = Cast<AActor>(WorldContextObject);
	if (SaveActor)
	{
		if (bSkipSave)
		{
			SaveActor->Tags.AddUnique(SkipSaveTag);
		}
		else
		{
			SaveActor->Tags.Remove(SkipSaveTag);
		}

		if (bPersistent)
		{
			SaveActor->Tags.AddUnique(PersistentTag);
		}
		else
		{
			SaveActor->Tags.Remove(PersistentTag);
		}

		if (bSkipTransform)
		{
			SaveActor->Tags.AddUnique(SkipTransformTag);
		}
		else
		{
			SaveActor->Tags.Remove(SkipTransformTag);
		}
	}
}

bool UEMSFunctionLibrary::IsSavingOrLoading(const UObject* WorldContextObject)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		return EMS->IsAsyncSaveOrLoadTaskActive(ESaveGameMode::MODE_All, EAsyncCheckType::CT_Both, false);
	}

	return false;
}

/**
Custom Objects
**/

bool UEMSFunctionLibrary::SaveCustom(UObject* WorldContextObject, UEMSCustomSaveGame* SaveGame)
{
	if (SaveGame)
	{
		if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
		{
			return EMS->SaveCustom(SaveGame);
		}
	}

	return false;
}

UEMSCustomSaveGame* UEMSFunctionLibrary::GetCustomSave(const UObject* WorldContextObject,
                                                       const TSubclassOf<UEMSCustomSaveGame> SaveGameClass)
{
	if (SaveGameClass->GetDefaultObject())
	{
		if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
		{
			return EMS->GetCustomSave(SaveGameClass);
		}
	}

	return nullptr;
}

void UEMSFunctionLibrary::DeleteCustomSave(UObject* WorldContextObject, UEMSCustomSaveGame* SaveGame)
{
	if (UEMSObject* EMS = UEMSObject::Get(WorldContextObject))
	{
		EMS->DeleteCustomSave(SaveGame);
	}
}
