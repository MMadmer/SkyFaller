// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/UTExtendedObject.h"

bool UUTExtendedObject::WaitingDeferred = false;

UUTExtendedObject::UUTExtendedObject(const FObjectInitializer& ObjectInitializer)
{
	bCanTick = false;
	bTickWhenPaused = false;

	bIsNetworkSupported = false;
}

void UUTExtendedObject::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject) || (IsInGameThread() && WaitingDeferred)) return;

	FinishCreate();
}

void UUTExtendedObject::FinishCreate()
{
	BeginPlay();
	EventBeginPlay();
}

void UUTExtendedObject::BeginPlay()
{
}

void UUTExtendedObject::BeginDestroy()
{
	bCanTick = false;

	UObject::BeginDestroy();
}

void UUTExtendedObject::Tick(float DeltaTime)
{
	TickImpl(DeltaTime);
	EventTick(DeltaTime);
}

void UUTExtendedObject::TickImpl(const float DeltaTime)
{
}

TStatId UUTExtendedObject::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUTExtendedObject, STATGROUP_Tickables);
}

void UUTExtendedObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	GetLifetimeReplicatedPropsImpl(OutLifetimeProps);

	const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass) BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
}

void UUTExtendedObject::GetLifetimeReplicatedPropsImpl(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
}

bool UUTExtendedObject::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	if (!GEngine) return false;

	bool bProcessed = false;

	if (AActor* MyOwner = Cast<AActor>(GetOuter()))
	{
		FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
		if (Context)
		{
			for (const FNamedNetDriver& Driver : Context->ActiveNetDrivers)
			{
				if (Driver.NetDriver && Driver.NetDriver->ShouldReplicateFunction(MyOwner, Function))
				{
					Driver.NetDriver->ProcessRemoteFunction(MyOwner, Function, Parms, OutParms, Stack, this);
					bProcessed = true;
				}
			}
		}
	}

	return bProcessed;
}

UWorld* UUTExtendedObject::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld) return nullptr;
	if (IsValid(GetOuter())) return GetOuter()->GetWorld();

	return nullptr;
}
