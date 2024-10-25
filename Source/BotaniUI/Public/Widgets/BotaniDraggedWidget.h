// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniUserWidget.h"
#include "GameplayTagContainer.h"
#include "Blueprint/DragDropOperation.h"
#include "BotaniDraggedWidget.generated.h"

/**
 * EBotaniDragDropResponse
 *
 * The possible responses to a drag and drop operation.
 */
UENUM(BlueprintType)
enum class EBotaniDragDropResponse : uint8
{
	/** The drop is pending. */
	DropPending UMETA(DisplayName = "Pending"),
	
	/** The drop was rejected. */
	DropRejected UMETA(DisplayName = "Rejected"),

	/** The drop was accepted. */
	DropAccepted UMETA(DisplayName = "Accepted"),

	/** The drop was accepted and will swap with the current item. */
	DropAcceptedSwap UMETA(DisplayName = "Accepted (Swap)"),
};

/**
 * UBotaniDragDropOperation
 *
 * An operation that is created when a drag and drop operation is started.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BOTANIUI_API UBotaniDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	/** Returns the dragged widget used to represent the drag and drop operation. */
	UFUNCTION(BlueprintCallable, Category = "Drag and Drop")
	class UBotaniDraggedWidget* GetDraggedWidget() const;

	/** Updates if the dragged widget can be dropped at the current location. */
	UFUNCTION(BlueprintCallable, Category = "Drag and Drop")
	void UpdateDragDropResponse(EBotaniDragDropResponse Response, const FText& Message);

	/** Returns the payload object as the given type. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Drag and Drop", meta = (DeterminesOutputType = "PayloadType"))
	UObject* GetTypedPayload(const TSubclassOf<UObject> PayloadType);

	/** Sets the tag of the drag operation. */
	UFUNCTION(BlueprintCallable, Category = "Drag and Drop")
	void SetDragOperationTag(FGameplayTag InTag);

	/** Returns the tag of the drag operation. */
	UFUNCTION(BlueprintCallable, Category = "Drag and Drop")
	FGameplayTag GetDragOperationTag() const;

public:
	/** The widget that "instigated" this drag drop operation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta=( ExposeOnSpawn="true"))
	TObjectPtr<UUserWidget> HostingWidget = nullptr;

	/** The tags that specify the drag operation. */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true"))
	FGameplayTagContainer DragOperationTags;
};

/**
 * UBotaniDraggedWidget
 *
 * A widget that gets created during a drag and drop operation.
 */
UCLASS(Abstract)
class BOTANIUI_API UBotaniDraggedWidget : public UBotaniUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** Called when the dragged widget gets created. */
	virtual void NativeOnDraggedWidgetCreated(UBotaniDragDropOperation* InOperation);

	/** Called when the dragged widget gets created. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Drag and Drop")
	void OnDraggedWidgetCreated(UBotaniDragDropOperation* InOperation);

	/** Updates if the dragged widget can be dropped at the current location. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Drag and Drop")
	void UpdateDragDropResponse(EBotaniDragDropResponse Response, const FText& Message);
};
