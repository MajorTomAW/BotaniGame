// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"

#include "BTService_SetFocus.generated.h"

class AActor;
class UBlackboardComponent;

struct FBTSetFocusMemory
{
	AActor* FocusActorSet;
	FVector FocusLocationSet;
	bool bActorSet;

	void Reset()
	{
		FocusActorSet = nullptr;
		FocusLocationSet = FAISystem::InvalidLocation;
		bActorSet = false;
	}
};

/**
 * Sets the focus of the AI controller to the specified actor or location.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "[BIO] Set Focus"), HideCategories = ("Service"))
class UBTService_SetFocus : public UBTService_BlackboardBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The priority of the focus. */
	UPROPERTY()
	uint8 FocusPriority;
	
	/** Determines whether we want to clear the focus. */
	UPROPERTY(EditAnywhere, Category = "Focus")
	uint8 bClearFocus : 1 = false;
	
	/** Determines whether the focus should be cleared when the service becomes irrelevant. */
	UPROPERTY(EditAnywhere, Category = "Focus", meta = (DisplayName = "Clear Focus on Irrelevant", EditCondition = "!bClearFocus", EditConditionHides))
	uint8 bClearFocusOnCeaseRelevant : 1 = false;

protected:
	//~ Begin UBTService_DefaultFocus Interface
	COMMONAI_API virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTSetFocusMemory); }
	COMMONAI_API virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	COMMONAI_API virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
	COMMONAI_API virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	COMMONAI_API virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	COMMONAI_API EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID);
	COMMONAI_API virtual FString GetStaticDescription() const override;
	//~ End UBTService_DefaultFocus Interface

#if WITH_EDITOR
	COMMONAI_API virtual FName GetNodeIconName() const override;
#endif
};
