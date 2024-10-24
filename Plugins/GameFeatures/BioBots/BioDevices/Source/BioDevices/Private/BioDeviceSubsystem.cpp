// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BioDeviceSubsystem.h"

FString FActorDeviceStateChangeParams::ToString() const
{
	FString Result;
	Result += FString::Printf(TEXT("OwningActor: %s\n"), *GetNameSafe(OwningActor));
	Result += FString::Printf(TEXT("DeviceName: %s\n"), *DeviceName.ToString());
	Result += FString::Printf(TEXT("Implementer: %s\n"), *GetNameSafe(Implementer));
	Result += FString::Printf(TEXT("bDeviceEnabled: %s\n"), bDeviceEnabled ? TEXT("true") : TEXT("false"));
	Result += FString::Printf(TEXT("bDeviceActive: %s\n"), bDeviceActive ? TEXT("true") : TEXT("false"));
	return Result;
}

UBioDeviceSubsystem::UBioDeviceSubsystem()
{
}

bool UBioDeviceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	// Make sure there are no child subsystems
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(GetClass(), ChildClasses, false);

	return ChildClasses.Num() == 0;
}

void UBioDeviceSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UBioDeviceSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UBioDeviceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_EDITORONLY_DATA
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &ThisClass::PostGarbageCollect);
#endif
}

UBioDeviceSubsystem* UBioDeviceSubsystem::GetForActor(const AActor* Actor)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}

	if (UWorld* World = Actor->GetWorld())
	{
		return World->GetSubsystem<UBioDeviceSubsystem>();
	}

	return nullptr;
}

bool UBioDeviceSubsystem::RegisterDevice(AActor* Actor, FName DeviceName, UObject* Implementer, bool bDefaultEnabled, bool bDefaultActive)
{
	if (!ensureMsgf(!DeviceName.IsNone(), TEXT("Device cannot be registered with a none name.")))
	{
		return false;
	}
	
	if (!ensureMsgf(Actor, TEXT("Device with name: %s cannot be registered on a null actor."), *DeviceName.ToString()))
	{
		return false;
	}

	FActorDeviceData& DeviceData = FindOrAddDeviceData(Actor);
	FActorDeviceState* FoundState = nullptr;

	for (auto& State : DeviceData.RegisteredDeviceStates)
	{
		if (State.DeviceName == DeviceName)
		{
			FoundState = &State;
			break;
		}
	}

	if (FoundState == nullptr)
	{
		FoundState = &DeviceData.RegisteredDeviceStates.Emplace_GetRef(DeviceName);
	}

	FoundState->Implementer = Implementer;
	FoundState->bDeviceEnabledState = bDefaultEnabled;
	FoundState->bDeviceActiveState = bDefaultActive;
	return true;
}

FDelegateHandle UBioDeviceSubsystem::RegisterDeviceCallback(AActor* Actor, FBioDeviceStateChangeDelegate Delegate, FName DeviceName, bool bCallImmediately)
{
	if (ensure(Actor && Delegate.IsBound()))
	{
		// We often must register delegates before the actor is fully initialized
		FActorDeviceData& DeviceData = FindOrAddDeviceData(Actor);

		FActorDeviceStateDelegate& NewDelegate = DeviceData.DeviceStateDelegates.Emplace_GetRef(MoveTemp(Delegate), DeviceName);

		// Cache the handle
		FDelegateHandle ReturnHandle = NewDelegate.DelegateHandle;

		if (bCallImmediately)
		{
			FActorDeviceStateDelegate DelegateCopy = NewDelegate;
			CallDeviceStateDelegatesForMatchingDevices(Actor, DelegateCopy);
		}

		return ReturnHandle;
	}

	return FDelegateHandle();
}

bool UBioDeviceSubsystem::ChangeDeviceActiveState(AActor* Actor, FName DeviceName, UObject* Implementer, bool bActive)
{
	if (Actor == nullptr || DeviceName.IsNone())
	{
		return false;
	}

	FActorDeviceData& DeviceData = FindOrAddDeviceData(Actor);
	FActorDeviceState* FoundState = nullptr;
	for (auto& State : DeviceData.RegisteredDeviceStates)
	{
		if (State.DeviceName == DeviceName)
		{
			FoundState = &State;
			break;
		}
	}

	if (!FoundState)
	{
		FoundState = &DeviceData.RegisteredDeviceStates.Emplace_GetRef(DeviceName);
	}

	FoundState->bDeviceActiveState = bActive;
	FoundState->Implementer = Implementer;

	CallDeviceStateDelegates(Actor, *FoundState);

	return true;
}

bool UBioDeviceSubsystem::ChangeDeviceEnabledState(AActor* Actor, FName DeviceName, UObject* Implementer, bool bEnabled, bool bForce)
{
	if (Actor == nullptr || DeviceName.IsNone())
	{
		return false;
	}

	FActorDeviceData& DeviceData = FindOrAddDeviceData(Actor);
	FActorDeviceState* FoundState = nullptr;
	for (auto& State : DeviceData.RegisteredDeviceStates)
	{
		if (State.DeviceName == DeviceName)
		{
			FoundState = &State;
			break;
		}
	}

	if (!FoundState)
	{
		FoundState = &DeviceData.RegisteredDeviceStates.Emplace_GetRef(DeviceName);
	}

	FoundState->bDeviceEnabledState = bEnabled;
	FoundState->Implementer = Implementer;

	CallDeviceStateDelegates(Actor, *FoundState);
	return true;
}

bool UBioDeviceSubsystem::IsDeviceActive(AActor* Actor, FName DeviceName) const
{
	if (Actor == nullptr || DeviceName.IsNone())
	{
		return false;
	}

	const FActorDeviceData* DeviceData = ActorDeviceMap.Find(FObjectKey(Actor));
	if (DeviceData == nullptr)
	{
		return false;
	}

	for (const auto& State : DeviceData->RegisteredDeviceStates)
	{
		if (State.DeviceName == DeviceName)
		{
			return State.bDeviceActiveState;
		}
	}

	return false;
}

bool UBioDeviceSubsystem::IsDeviceEnabled(AActor* Actor, FName DeviceName) const
{
	if (Actor == nullptr || DeviceName.IsNone())
	{
		return false;
	}

	const FActorDeviceData* DeviceData = ActorDeviceMap.Find(FObjectKey(Actor));
	if (DeviceData == nullptr)
	{
		return false;
	}

	for (const auto& State : DeviceData->RegisteredDeviceStates)
	{
		if (State.DeviceName == DeviceName)
		{
			return State.bDeviceEnabledState;
		}
	}

	return false;
}

UBioDeviceSubsystem::FActorDeviceStateDelegate::FActorDeviceStateDelegate(
	FBioDeviceStateChangeDelegate&& InDelegate, FName InDeviceName)
		: Delegate(InDelegate)
		, DelegateHandle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle)
		, RequiredDeviceName(InDeviceName)
{
}

UBioDeviceSubsystem::FActorDeviceStateDelegate::FActorDeviceStateDelegate(
	FBioDeviceStateChangeBPDelegate&& InDelegate, FName InDeviceName)
		: BPDelegate(InDelegate)
		, DelegateHandle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle)
		, RequiredDeviceName(InDeviceName)
{
}

void UBioDeviceSubsystem::FActorDeviceStateDelegate::Execute(
	AActor* OwningActor, FName DeviceName, UObject* Implementer, bool bEnabled, bool bActive)
{
	FActorDeviceStateChangeParams Params(OwningActor, DeviceName, Implementer, bEnabled, bActive);
	if (Delegate.IsBound())
	{
		ensure(!BPDelegate.IsBound());
		Delegate.Execute(Params);
	}
	else if (BPDelegate.IsBound())
	{
		BPDelegate.Execute(Params);
	}
}


UBioDeviceSubsystem::FActorDeviceData& UBioDeviceSubsystem::FindOrAddDeviceData(AActor* Actor)
{
	check(Actor);

	FActorDeviceData& DeviceData = ActorDeviceMap.FindOrAdd(FObjectKey(Actor));
	if (!DeviceData.ActorClass.IsValid())
	{
		DeviceData.ActorClass = Actor->GetClass();
	}

	return DeviceData;
}

const UBioDeviceSubsystem::FActorDeviceState* UBioDeviceSubsystem::FindDeviceStateStruct( const FActorDeviceData* DeviceData, FName DeviceName) const
{
	if (DeviceData == nullptr)
	{
		return nullptr;
	}

	for (const FActorDeviceState& State : DeviceData->RegisteredDeviceStates)
	{
		if (State.DeviceName == DeviceName)
		{
			return &State;
		}
	}

	return nullptr;
}

void UBioDeviceSubsystem::CallDeviceStateDelegates(AActor* Actor, FActorDeviceState StateChange)
{
	FActorDeviceData* DeviceData = ActorDeviceMap.Find(FObjectKey(Actor));
	UClass* ActorClass = Actor->GetClass();
	bool bMoreActorDelegates = true;
	bool bMoreClassDelegates = true;
	int32 DelegateIndex = 0;

	while (DeviceData)
	{
		if (bMoreActorDelegates)
		{
			if (DeviceData->DeviceStateDelegates.IsValidIndex(DelegateIndex))
			{
				FActorDeviceStateDelegate& RegisteredDelegate = DeviceData->DeviceStateDelegates[DelegateIndex];

				if (RegisteredDelegate.RequiredDeviceName.IsNone() ||
					(RegisteredDelegate.RequiredDeviceName == StateChange.DeviceName))
				{
					// Call the delegate
					RegisteredDelegate.Execute(Actor, StateChange.DeviceName, StateChange.Implementer.Get(), StateChange.bDeviceEnabledState, StateChange.bDeviceActiveState);

					// That could have invalidated anything
					DeviceData = ActorDeviceMap.Find(FObjectKey(Actor));
				}

				DelegateIndex++;
			}
			else
			{
				DelegateIndex = 0;
				bMoreActorDelegates = false;
			}
		}
		else if (ActorClass)
		{
			// Now check general class delegates
			ActorClass = ActorClass->GetSuperClass();
		}
		else
		{
			return;
		}
	}
}

void UBioDeviceSubsystem::CallDeviceStateDelegatesForMatchingDevices(AActor* Actor, FActorDeviceStateDelegate& Delegate)
{
	FActorDeviceData* DeviceData = ActorDeviceMap.Find(FObjectKey(Actor));
	int32 DelegateIndex = 0;

	// If device is specified, just call the one
	if (!Delegate.RequiredDeviceName.IsNone())
	{
		const FActorDeviceState* FoundState = FindDeviceStateStruct(DeviceData, Delegate.RequiredDeviceName);

		if (FoundState)
		{
			Delegate.Execute(Actor, FoundState->DeviceName, FoundState->Implementer.Get(), FoundState->bDeviceEnabledState, FoundState->bDeviceActiveState);
		}

		return;
	}

	// If device is not specified, call all
	while (DeviceData && DeviceData->DeviceStateDelegates.IsValidIndex(DelegateIndex))
	{
		const FActorDeviceState* FoundState = &DeviceData->RegisteredDeviceStates[DelegateIndex];

		Delegate.Execute(Actor, FoundState->DeviceName, FoundState->Implementer.Get(), FoundState->bDeviceEnabledState, FoundState->bDeviceActiveState);

		// That could have invalidated anything
		DeviceData = ActorDeviceMap.Find(FObjectKey(Actor));

		DelegateIndex++;
	}
}

#if WITH_EDITORONLY_DATA
void UBioDeviceSubsystem::PostGarbageCollect()
{
	// Clear invalid devices
	for (auto It = AllDevices.CreateIterator(); It; ++It)
	{
		if (It->ResolveObjectPtr() == nullptr)
		{
			It.RemoveCurrent();
		}
	}
}
#endif
