#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "IAssetTools.h"
#include "Factories/BlueprintFactory.h"
#include "Modules/ModuleManager.h"
#include "BotaniEditor.generated.h"

class FBotaniEditorModule : public IModuleInterface
{
public:
    //~ Begin IModuleInterface Interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    //~ End IModuleInterface Interface

    static EAssetTypeCategories::Type GetExperienceAssetsCategory();
    static EAssetTypeCategories::Type GetInputCategory();
    static EAssetTypeCategories::Type GetAbilitySystemCategory();

protected:
    void RegisterAssetTypeActions(IAssetTools& AssetTools, const TSharedRef<IAssetTypeActions>& Action);

    static EAssetTypeCategories::Type ExperienceAssetsCategory;
    static EAssetTypeCategories::Type InputCategory;
    static EAssetTypeCategories::Type AbilitySystemCategory;

    TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
    TSharedPtr<FSlateStyleSet> StyleSet;
};

////////////////////////////////////////////////////////////////////
/// Data Asset factories
////////////////////////////////////////////////////////////////////

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniInputConfig_Factory : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY(EditAnywhere, Category = InputConfig)
    TSubclassOf<class UBotaniInputConfig> InputConfigClass;
    
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniPawnData_Factory : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY(EditAnywhere, Category = PawnData)
    TSubclassOf<class UBotaniPawnData> PawnDataClass;

    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniGameData_Factory : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY(EditAnywhere, Category = GameData)
    TSubclassOf<class UBotaniGameData> GameDataClass;

    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniExperienceActionSet_Factory : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniAbilitySet_Factory : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

////////////////////////////////////////////////////////////////////
/// Blueprint Factories
////////////////////////////////////////////////////////////////////

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniCameraMode_Factory : public UBlueprintFactory
{
    GENERATED_UCLASS_BODY()

public:
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniExperienceDefinition_Factory : public UBlueprintFactory
{
    GENERATED_UCLASS_BODY()

public:
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniGameplayAbility_Factory : public UBlueprintFactory
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY(EditAnywhere, Category = GameplayAbility)
    TSubclassOf<class UBotaniGameplayAbility> GameplayAbilityClass;

    virtual bool ConfigureProperties() override;
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};

UCLASS(NotBlueprintable, NotBlueprintType)
class UBotaniGameplayEffect_Factory : public UBlueprintFactory
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY(EditAnywhere, Category = GameplayEffect)
    TSubclassOf<class UGameplayEffect> GameplayEffectClass;

    virtual bool ConfigureProperties() override;
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};
