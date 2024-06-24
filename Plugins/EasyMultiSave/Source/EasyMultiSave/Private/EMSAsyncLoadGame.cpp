//Easy Multi Save - Copyright (C) 2022 by Michael Hegemann.  

#include "EMSAsyncLoadGame.h"
#include "EMSFunctionLibrary.h"
#include "EMSObject.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectIterator.h"
#include "Engine/Engine.h"


/**
Init
**/

UEMSAsyncLoadGame::UEMSAsyncLoadGame(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bIsActive(0), Data(0), Mode(), WorldContextObject(nullptr), EMS(nullptr),
	  bFullReload(0), bDeferredLoadSuccess(0), LoadedActorNum(0), bLoadFailed(0)
{
}

ESaveGameMode UEMSAsyncLoadGame::GetMode(int32 Data)
{
	if (Data & ENUM_TO_FLAG(ELoadTypeFlags::LF_Player))
	{
		if (Data & ENUM_TO_FLAG(ELoadTypeFlags::LF_Level))
		{
			return ESaveGameMode::MODE_All;
		}
		else
		{
			return ESaveGameMode::MODE_Player;
		}
	}

	return ESaveGameMode::MODE_Level;
}

UEMSAsyncLoadGame* UEMSAsyncLoadGame::AsyncLoadActors(UObject* WorldContext, const int32 Data, const bool bFullReload)
{
	if (UEMSObject* EMSObject = UEMSObject::Get(WorldContext))
	{
		if (!EMSObject->IsAsyncSaveOrLoadTaskActive(GetMode(Data)))
		{
			UEMSAsyncLoadGame* LoadTask = NewObject<UEMSAsyncLoadGame>(GetTransientPackage());
			LoadTask->WorldContextObject = WorldContext;
			LoadTask->Data = Data;
			LoadTask->Mode = GetMode(Data);
			LoadTask->bFullReload = bFullReload;
			LoadTask->EMS = EMSObject;
			LoadTask->bIsActive = true;

			return LoadTask;
		}
	}

	return nullptr;
}

void UEMSAsyncLoadGame::Activate()
{
	if (EMS)
	{
		EMS->PrepareLoadAndSaveActors(Data, EAsyncCheckType::CT_Load, bFullReload);

		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncLoadGame::StartLoading);

		const float WaitTime = UEMSPluginSettings::Get()->AsyncWaitDelay;
		if (WaitTime > 0.f)
		{
			EMS->GetTimerManager().SetTimer(FailedTimerHandle,
			                                FTimerDelegate::CreateUObject(this, &UEMSAsyncLoadGame::FailLoadingTask),
			                                WaitTime, false);
		}
	}
}

void UEMSAsyncLoadGame::ClearFailTimer()
{
	if (EMS)
	{
		if (FailedTimerHandle.IsValid())
		{
			EMS->GetTimerManager().ClearTimer(FailedTimerHandle);
		}
	}
}

/**
Load Actors
**/

void UEMSAsyncLoadGame::StartLoading()
{
	if (EMS)
	{
		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncLoadGame::LoadPlayer);
	}
}

void UEMSAsyncLoadGame::LoadPlayer()
{
	bLoadFailed = true;

	if (EMS)
	{
		if (Data & ENUM_TO_FLAG(ELoadTypeFlags::LF_Player))
		{
			if (!EMS->HasValidPlayer())
			{
				EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncLoadGame::LoadPlayer);
				return;
			}

			if (EMS->TryLoadPlayerFile())
			{
				SetLoadNotFailed();

				EMS->LoadPlayerActors(this);
			}
		}

		EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncLoadGame::LoadLevel);
	}
}

void UEMSAsyncLoadGame::LoadLevel()
{
	if (EMS)
	{
		if (Data & ENUM_TO_FLAG(ELoadTypeFlags::LF_Level))
		{
			if (!EMS->HasValidGameMode())
			{
				EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncLoadGame::LoadLevel);
				return;
			}

			if (EMS->TryLoadLevelFile())
			{
				SetLoadNotFailed();

				EMS->LoadLevelActors(this);
			}
			else
			{
				FinishLoading();
			}
		}
		else
		{
			FinishLoading();
		}
	}
}

/**
Finish
**/

void UEMSAsyncLoadGame::FinishLoading()
{
	if (EMS)
	{
		//Has to be a tick before broadcast.
		bIsActive = false;

		ClearFailTimer();

		if (bLoadFailed)
		{
			EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncLoadGame::FailLoadingTask);
		}
		else
		{
			EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncLoadGame::CompleteLoadingTask);
		}
	}
}

void UEMSAsyncLoadGame::CompleteLoadingTask()
{
	OnCompleted.Broadcast();
	SetReadyToDestroy();
}

void UEMSAsyncLoadGame::FailLoadingTask()
{
	OnFailed.Broadcast();
	SetReadyToDestroy();
}

/**
Deferred Loading
**/

void UEMSAsyncLoadGame::StartDeferredLoad()
{
	if (EMS)
	{
		SavedActors = EMS->SavedActors;
		DeferredLoadActors();
	}
}

void UEMSAsyncLoadGame::DeferredLoadActors()
{
	if (EMS)
	{
		const uint32 Batch = UEMSPluginSettings::Get()->DeferredLoadStackSize;

		if (LoadedActorNum < SavedActors.Num())
		{
			uint32 LoadNum = 0;
			while (LoadNum < Batch)
			{
				const int32 FullBatch = LoadNum + LoadedActorNum;
				if (FullBatch >= SavedActors.Num())
				{
					break;
				}

				const uint32 MinLoadNum = FMath::Min(FullBatch, SavedActors.Num());
				bDeferredLoadSuccess = EMS->SpawnOrUpdateLevelActor(SavedActors[MinLoadNum]);

				LoadNum++;
			}

			LoadedActorNum += Batch;

			EMS->GetTimerManager().SetTimerForNextTick(this, &UEMSAsyncLoadGame::DeferredLoadActors);
		}
		else
		{
			if (bDeferredLoadSuccess)
			{
				EMS->LogFinishLoadingLevel();
			}

			FinishLoading();
		}
	}
}
