// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Tasks/BTTask_FindSmartObject.h"

#include "BlackboardKeyType_SOClaimHandle.h"

UBTTask_FindSmartObject::UBTTask_FindSmartObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Find Smart Object";
	ClaimPriority = ESmartObjectClaimPriority::Normal;

	bNotifyTaskFinished = true;

	EQSRequest.RunMode = EEnvQueryRunMode::AllMatching;
	OnQueryFinishedDelegate = FQueryFinishedSignature::CreateUObject(this, &ThisClass::OnQueryFinished);

	AddSOFilter();
}

EBTNodeResult::Type UBTTask_FindSmartObject::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

EBTNodeResult::Type UBTTask_FindSmartObject::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_FindSmartObject::OnTaskFinished(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_FindSmartObject::InitializeMemory(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FBTUseSOTaskMemory>(NodeMemory, InitType);
}

void UBTTask_FindSmartObject::CleanupMemory(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTUseSOTaskMemory>(NodeMemory, CleanupType);
}

FString UBTTask_FindSmartObject::GetStaticDescription() const
{
	FString Result;
	if (!ActivityRequirements.IsEmpty())
	{
		Result += FString::Printf(TEXT("Object requirements: %s"), *ActivityRequirements.GetDescription());
	}

	return Result.Len() > 0 ? Result : Super::GetStaticDescription();
}

void UBTTask_FindSmartObject::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	EQSRequest.InitForOwnerAndBlackboard(*this, GetBlackboardAsset());
}

void UBTTask_FindSmartObject::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
}

void UBTTask_FindSmartObject::AddSOFilter()
{
	UBlackboardKeyType_SOClaimHandle* ClaimHandleType = NewObject<UBlackboardKeyType_SOClaimHandle>();
	BlackboardKey.AllowedTypes.Add(ClaimHandleType);
}
