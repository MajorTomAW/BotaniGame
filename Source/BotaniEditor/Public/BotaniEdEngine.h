// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "BotaniEdEngine.generated.h"


UCLASS()
class BOTANIEDITOR_API UBotaniEdEngine : public UUnrealEdEngine
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UEngine Interface
	virtual void Init(IEngineLoop* InEngineLoop) override;
	virtual void Start() override;
	virtual void Tick(float DeltaSeconds, bool bIdleMode) override;

	virtual FGameInstancePIEResult PreCreatePIEInstances(const bool bAnyBlueprintErrors, const bool bStartInSpectatorMode, const float PIEStartTime, const bool bSupportsOnlinePIE, int32& InNumOnlinePIEInstances) override;
	//~ End UEngine Interface

private:
	void FirstTickSetup();
	bool bFirstTickSetup;
};
