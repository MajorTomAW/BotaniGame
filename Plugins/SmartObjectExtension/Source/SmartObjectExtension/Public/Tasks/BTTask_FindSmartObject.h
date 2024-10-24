// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "SmartObjectTypes.h"
#include "AI/BTTask_FindAndUseGameplayBehaviorSmartObject.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Runtime/AIModule/Classes/BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindSmartObject.generated.h"

struct FSmartObjectClaimHandle;

/**
 * Tries to find a smart object in the world and sets it as the target.
 */
UCLASS(MinimalAPI)
class UBTTask_FindSmartObject : public UBTTask_BlackboardBase
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UBTTask_BlackboardBase Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTUseSOTaskMemory); }
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
	virtual FString GetStaticDescription() const override;
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	//~ End UBTTask_BlackboardBase Interface

	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result);

public:
	/** The request filter to apply to the smart object search. */
	UPROPERTY(EditAnywhere, Category = "Smart Objects")
	FGameplayTagQuery ActivityRequirements;

	/** The claim priority to use when claiming the smart object. */
	UPROPERTY(EditAnywhere, Category = "Smart Objects")
	ESmartObjectClaimPriority ClaimPriority;

	/** The EQS request to use when searching for smart objects. */
	UPROPERTY(EditAnywhere, Category = "Smart Objects")
	FEQSParametrizedQueryExecutionRequest EQSRequest;

	/** Fallback radius to use when EQSRequest is not configured. */
	UPROPERTY(EditAnywhere, Category = "Smart Objects")
	float FallbackRadius = 1000.f;

protected:
	FQueryFinishedSignature OnQueryFinishedDelegate;

private:
	 void AddSOFilter();
};
