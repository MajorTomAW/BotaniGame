// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BehaviorTree/Services/BTService_SetFocus.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTService_SetFocus)

UBTService_SetFocus::UBTService_SetFocus(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Set focus";
	
	bTickIntervals = false;
	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	FocusPriority = EAIFocusPriority::Gameplay;

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_STRING_CHECKED(UBTService_SetFocus, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_STRING_CHECKED(UBTService_SetFocus, BlackboardKey));
}

void UBTService_SetFocus::InitializeMemory(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FBTSetFocusMemory>(NodeMemory, InitType);
}

void UBTService_SetFocus::CleanupMemory(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTSetFocusMemory>(NodeMemory, CleanupType);
}

void UBTService_SetFocus::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	FBTSetFocusMemory* MyMemory = CastInstanceNodeMemory<FBTSetFocusMemory>(NodeMemory);
	check(MyMemory);
	MyMemory->Reset();

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	
	if (OwnerController != nullptr && Blackboard != nullptr)
	{
		if (bClearFocus)
		{
			OwnerController->ClearFocus(FocusPriority);

			return;
		}

		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = Blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			AActor* TargetActor = Cast<AActor>(KeyValue);
			if (TargetActor)
			{
				OwnerController->SetFocus(TargetActor, FocusPriority);
				MyMemory->FocusActorSet = TargetActor;
				MyMemory->bActorSet = true;
			}
		}
		else
		{
			const FVector FocusLocation = Blackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
			OwnerController->SetFocalPoint(FocusLocation, FocusPriority);
			MyMemory->FocusLocationSet = FocusLocation;
		}

		const FBlackboard::FKey KeyID = BlackboardKey.GetSelectedKeyID();
		Blackboard->RegisterObserver(KeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTService_SetFocus::OnBlackboardKeyValueChange));
	}
}

void UBTService_SetFocus::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	FBTSetFocusMemory* MyMemory = CastInstanceNodeMemory<FBTSetFocusMemory>(NodeMemory);
	check(MyMemory);

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController != nullptr && bClearFocusOnCeaseRelevant)
	{
		bool bDidClearFocus = false;
		if (MyMemory->bActorSet)
		{
			bDidClearFocus = (MyMemory->FocusActorSet == OwnerController->GetFocusActorForPriority(FocusPriority));
		}
		else
		{
			bDidClearFocus = (MyMemory->FocusLocationSet == OwnerController->GetFocalPointForPriority(FocusPriority));
		}

		if (bDidClearFocus)
		{
			OwnerController->ClearFocus(FocusPriority);
		}
	}
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard)
	{
		Blackboard->UnregisterObserversFrom(this);
	}
}

EBlackboardNotificationResult UBTService_SetFocus::OnBlackboardKeyValueChange(
	const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(Blackboard.GetBrainComponent());
	AAIController* OwnerController = OwnerComp ? OwnerComp->GetAIOwner() : nullptr;
	if (OwnerController == nullptr)
	{
		return EBlackboardNotificationResult::ContinueObserving;
	}

	const int32 NodeInstanceIdx = OwnerComp->FindInstanceContainingNode(this);
	FBTSetFocusMemory* MyMemory = CastInstanceNodeMemory<FBTSetFocusMemory>(OwnerComp->GetNodeMemory(this, NodeInstanceIdx));
	MyMemory->Reset();
	OwnerController->ClearFocus(FocusPriority);

	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = Blackboard.GetValue<UBlackboardKeyType_Object>(ChangedKeyID);
		AActor* TargetActor = Cast<AActor>(KeyValue);
		if (TargetActor)
		{
			OwnerController->SetFocus(TargetActor, FocusPriority);
			MyMemory->FocusActorSet = TargetActor;
			MyMemory->bActorSet = true;
		}
	}
	else
	{
		const FVector FocusLocation = Blackboard.GetValue<UBlackboardKeyType_Vector>(ChangedKeyID);
		OwnerController->SetFocalPoint(FocusLocation, FocusPriority);
		MyMemory->FocusLocationSet = FocusLocation;
	}

	return EBlackboardNotificationResult::ContinueObserving;
}

FString UBTService_SetFocus::GetStaticDescription() const
{
	if (bClearFocus)
	{
		return "Clear focus";
	}

	FString KeyDesc("invalid");
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
		BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Set focus to %s"), *KeyDesc);
}

#if WITH_EDITOR
FName UBTService_SetFocus::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Service.DefaultFocus.Icon");
}
#endif