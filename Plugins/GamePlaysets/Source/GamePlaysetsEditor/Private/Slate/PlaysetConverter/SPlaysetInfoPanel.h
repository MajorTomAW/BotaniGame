// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "PlaysetTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "SPlaysetInfoPanel.generated.h"

class GAMEPLAYSETSEDITOR_API SPlaysetInfoPanel final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPlaysetInfoPanel)
	{
	}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void GetPlaysetInfo(FPlaysetDisplayInfo& OutDisplayInfo, FPlaysetDataList& OutData, EPlaysetOffsetType& OffsetType) const;

private:
	TSharedPtr<IDetailsView> PlaysetDetailsView;
	TObjectPtr<class UPlaysetInfoObject> PlaysetInfoObject;
};

/**
 * UPlaysetInfoObject
 *
 * Contains all the information about a new playset.
 */
UCLASS()
class UPlaysetInfoObject : public UObject
{
	GENERATED_BODY()

public:
	/** The playset display info. */
	UPROPERTY(EditAnywhere, Category = "Playset", meta = (DisplayName = "Display Info"))
	FPlaysetDisplayInfo PlaysetDisplayInfo;

	/** The playset offset type. */
	UPROPERTY(EditAnywhere, Category = "Playset", meta = (DisplayName = "Offset Type"))
	EPlaysetOffsetType OffsetType = EPlaysetOffsetType::Center;
	
	/** The playset data. */
	UPROPERTY(EditAnywhere, Category = "Playset", meta = (DisplayName = "Data"))
	FPlaysetDataList PlaysetData;
};




