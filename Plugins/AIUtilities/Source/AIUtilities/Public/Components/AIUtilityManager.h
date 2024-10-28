// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "AIUtilityManager.generated.h"

/**
 * UAIUtilityManager
 *
 * An actor component class.
 */
UCLASS(MinimalAPI, ClassGroup = (AI), meta = (BlueprintSpawnableComponent, DisplayName = "AI Utility Manager"))
class UAIUtilityManager : public UBrainComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UBrainComponent Interface
	AIUTILITIES_API virtual void StartLogic() override;
	AIUTILITIES_API virtual void RestartLogic() override;
	AIUTILITIES_API virtual void StopLogic(const FString& Reason) override;
	AIUTILITIES_API virtual void PauseLogic(const FString& Reason) override;
	AIUTILITIES_API virtual EAILogicResuming::Type ResumeLogic(const FString& Reason) override;
	//~ End UBrainComponent Interface
	
protected:
private:
};
