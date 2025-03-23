// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UTExtendedObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class USEFULTOOLS_API UUTExtendedObject : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	explicit UUTExtendedObject(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitProperties() override;

	/**
	 * Game thread only!
	 * Create object without calling BeginPlay() function.
	 * Use FinishCreate() to complete creating object.
	 */
	template <typename T>
	static T* CreateDeferred(UObject* Outer, const UClass* Class)
	{
		static_assert(TIsDerivedFrom<T, UUTExtendedObject>::IsDerived,
		              "T must be derived from UUTExtendedObject");
		checkf(IsInGameThread(), TEXT("CreateDeferred must be called from the GameThread!"));

		WaitingDeferred = true;
		T* Obj = NewObject<T>(Outer, Class);
		WaitingDeferred = false;

		return Obj;
	}

	void FinishCreate();

protected:
	//--------------------------------------------------------------------
	// BeginPlay
	//--------------------------------------------------------------------

	/** Use it like actor's BeginPlay().*/
	virtual void BeginPlay();

	/** Calls when object created.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="BeginPlay")
	void EventBeginPlay();
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Tick
	//--------------------------------------------------------------------

public:
	/** If true, will call tick function every frame. GetWorld() still must be valid.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tick")
	uint8 bCanTick : 1;

	/** Should tick when the game is paused.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tick")
	uint8 bTickWhenPaused : 1;

protected:
	/**
	 * Internal class function. Use TickImpl(const float DeltaTime) instead.
	 * Override only if u know what u doing.
	 * GetWorld() must be valid.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Use it like actor's Tick()
	 * GetWorld() must be valid.
	 */
	virtual void TickImpl(const float DeltaTime);

	/**
	 * Calls every frame if object is tickable.
	 * GetWorld() must be valid.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Tick")
	void EventTick(const float DeltaTime);

	virtual bool IsTickable() const override { return !HasAnyFlags(RF_ClassDefaultObject) && GetWorld() && bCanTick; }

	virtual bool IsTickableWhenPaused() const override { return bTickWhenPaused; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual TStatId GetStatId() const override;

	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Replication
	//--------------------------------------------------------------------

	/** Should support networking(RPC and replication).Object still must have owner.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Replication")
	uint8 bIsNetworkSupported : 1;

	virtual bool IsSupportedForNetworking() const override { return bIsNetworkSupported; }

	/** Don't override this function. Use GetLifetimeReplicatedPropsImpl.*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Use it instead of GetLifetimeReplicatedProps.*/
	virtual void GetLifetimeReplicatedPropsImpl(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;

	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override
	{
		return GetOuter() ? GetOuter()->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local;
	}

	//--------------------------------------------------------------------

	virtual UWorld* GetWorld() const override;
	virtual void BeginDestroy() override;

private:
	/** Used with CreateDeferred function.*/
	static bool WaitingDeferred;
};
