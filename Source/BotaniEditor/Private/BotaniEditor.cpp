#include "BotaniEditor.h"

#include "AssetToolsModule.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "IContentBrowserSingleton.h"
#include "IPlacementModeModule.h"
#include "KismetCompilerModule.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "AssetActions/BotaniExperience_Actions.h"

#include "AssetActions/BotaniGameData_Actions.h"
#include "AssetActions/BotaniInputConfig_Actions.h"
#include "AssetActions/BotaniPawnData_Actions.h"
#include "Camera/BotaniCameraMode.h"
#include "Details/BotaniCharacterModDetailsViewer.h"
#include "Details/BotaniEquipmentDetailsViewer.h"
#include "Development/BotaniDeveloperSettings.h"
#include "Game/Experience/BotaniExperienceDefinition.h"
#include "GameFeatures/Data/BotaniExperienceActionSet.h"

#include "GameFeatures/Data/BotaniGameData.h"
#include "GameFeatures/Data/BotaniPawnData.h"

#include "Input/BotaniInputConfig.h"
#include "Inventory/Definitions/BotaniInventoryItemDefinition.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Player/BotaniPlayerStart.h"
#include "Settings/BotaniEditorSettings.h"

#include "Slate/BotaniEditorSlateStyle.h"
#include "Slate/SlateBrushAsset.h"
#include "Styling/SlateStyleRegistry.h"
#include "ThumbnailRenderer/BotaniItemThumbnailRenderer.h"

#define LOCTEXT_NAMESPACE "FBotaniEditorModule"

EAssetTypeCategories::Type FBotaniEditorModule::ExperienceAssetsCategory;
EAssetTypeCategories::Type FBotaniEditorModule::InputCategory;
EAssetTypeCategories::Type FBotaniEditorModule::AbilitySystemCategory;

void FBotaniEditorModule::StartupModule()
{
    FSlateStyleSet* Style = (FSlateStyleSet*)&FAppStyle::Get();
    const FSlateBrush* RegularBody = Style->GetBrush("Graph.StateNode.Body");
    FSlateBrush NewBody = *RegularBody;
    NewBody.TintColor = FLinearColor(FColor::FromHex("8FBFF5FF"));
    
    Style->Set("Graph.StateNode.Body", new FSlateBrush(NewBody));
    
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    InputCategory = AssetTools.FindAdvancedAssetCategory("Input");
    
    ExperienceAssetsCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Experience")), LOCTEXT("ExperienceAssetCategory", "Experience"));
    {
        // Game data assets
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FBotaniInputConfig_Actions()));
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FBotaniPawnData_Actions()));
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FBotaniGameData_Actions()));

        // Experience assets
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FBotaniExperienceDefinition_Actions()));
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FBotaniExperienceActionSet_Actions()));
    }

    AbilitySystemCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Ability System")), LOCTEXT("AbilitySystemAssetCategory", "Ability System"));
    {
        // Ability data assets
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FBotaniAbilitySet_Actions()));

        // Gameplay ability classes
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FBotaniGameplayAbility_Actions()));
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FBotaniGameplayEffect_Actions()));
    }

    IContentBrowserSingleton& ContentBrowserSingleton = IContentBrowserSingleton::Get();
    // ContentBrowserSingleton.Set

    // Register custom class layouts
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomPropertyTypeLayout("BotaniEquipmentActorToSpawn",
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBotaniEquipmentDetailsViewer::MakeInstance));

   // PropertyModule.RegisterCustomPropertyTypeLayout("BotaniCharacterModifier",
    //    FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBotaniCharacterModDetailsViewer::MakeInstance));
    PropertyModule.NotifyCustomizationModuleChanged();

    // Make a new style set for Botani Editor which will manage custom icons
    StyleSet = MakeShared<FBotaniEditorSlateStyle>();
    FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());

    // Register the thumbnail renderer
    if (GIsEditor)
    {
        UThumbnailManager::Get().RegisterCustomRenderer(UBotaniInventoryItemDefinition::StaticClass(), UBotaniItemThumbnailRenderer::StaticClass());
    }

    // Register a new placement mode category for Botani
    constexpr bool bHasCustomPlacementMode = false;
    if (GIsEditor && bHasCustomPlacementMode)
    {
        IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
        //PlacementModeModule.UnregisterPlacementCategory(FBuiltInPlacementCategories::Basic());
        //PlacementModeModule.UnregisterPlacementCategory(FBuiltInPlacementCategories::AllClasses());
        //PlacementModeModule.UnregisterPlacementCategory(FBuiltInPlacementCategories::RecentlyPlaced());
        //PlacementModeModule.UnregisterPlacementCategory("Cinematic");
        //PlacementModeModule.UnregisterPlacementCategory("PMCinematic");
        
        int32 SortOrder = 0;
        const FName CategoryNameHandle = FName(TEXT("Botani"));
        const FText CategoryDisplayName = NSLOCTEXT("PlacementMode", "PMBotani", "Botani Assets");
        
        
        FPlacementCategoryInfo Info(
            CategoryDisplayName,
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.Object"),
            CategoryNameHandle,
            TEXT("PMBotani"),
            TNumericLimits<int32>::Lowest()
        );
        
        PlacementModeModule.RegisterPlacementCategory(Info);

        // Load the default player start class from the content browser
        FSoftClassPath Path(TEXT("/BioDevices/Devices/BioPlayerStart/Device_BioPlayerStart.Device_BioPlayerStart_C"));
        UClass* PlayerStartClass = Path.TryLoadClass<ABotaniPlayerStart>();
        FAssetData AssetData(PlayerStartClass);
        //@TODO: Let each GFP add custom assets to this list
        
        UActorFactory* ActorFactory = GEditor->FindActorFactoryByClass(ABotaniPlayerStart::StaticClass());
        
        PlacementModeModule.RegisterPlaceableItem(CategoryNameHandle, MakeShareable(new FPlaceableItem(ActorFactory, AssetData, SortOrder += 10)));
    }
}

void FBotaniEditorModule::ShutdownModule()
{
    // Unregister the asset type actions
    if (const FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
    {
        for (TSharedPtr<IAssetTypeActions>& AssetAction : CreatedAssetTypeActions)
        {
            AssetToolsModule->Get().UnregisterAssetTypeActions(AssetAction.ToSharedRef());
        }
    }
    CreatedAssetTypeActions.Empty();

    // Unregister slate styling
    if (StyleSet.IsValid())
    {
        FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
    }
}

EAssetTypeCategories::Type FBotaniEditorModule::GetExperienceAssetsCategory()
{
    return ExperienceAssetsCategory;
}

EAssetTypeCategories::Type FBotaniEditorModule::GetInputCategory()
{
    return InputCategory;
}

EAssetTypeCategories::Type FBotaniEditorModule::GetAbilitySystemCategory()
{
    return AbilitySystemCategory;
}

void FBotaniEditorModule::RegisterAssetTypeActions(IAssetTools& AssetTools, const TSharedRef<IAssetTypeActions>& Action)
{
    AssetTools.RegisterAssetTypeActions(Action);
    CreatedAssetTypeActions.Add(Action);
}

////////////////////////////////////////////////////////////////////
// Class viewer filter

class FBotaniEditorClassFilter : public IClassViewerFilter
{
public:
    FBotaniEditorClassFilter()
        : DisallowedClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown)
    {
    }

    virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
    {
        return !InClass->HasAnyClassFlags(DisallowedClassFlags)
        && InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
    }

    virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
    {
        return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
            && InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
    }

public:
    // All children of these classes are allowed
    TSet<const UClass*> AllowedChildrenOfClasses;

    // Disallowed class flags
    EClassFlags DisallowedClassFlags;
};

////////////////////////////////////////////////////////////////////
// Asset factories

UBotaniInputConfig_Factory::UBotaniInputConfig_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UBotaniInputConfig::StaticClass();
    bCreateNew = true;
    bEditAfterNew = true;
}


UObject* UBotaniInputConfig_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    UBotaniInputConfig* NewInputConfig;

    if (InputConfigClass != nullptr)
    {
        NewInputConfig = NewObject<UBotaniInputConfig>(InParent, InputConfigClass, Name, Flags, Context);
    }
    else
    {
        check(Class->IsChildOf(UBotaniInputConfig::StaticClass()));
        NewInputConfig = NewObject<UBotaniInputConfig>(InParent, Class, Name, Flags, Context);
    }

    return NewInputConfig;
}


UBotaniPawnData_Factory::UBotaniPawnData_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UBotaniPawnData::StaticClass();
    bCreateNew = true;
    bEditAfterNew = true;
}

UObject* UBotaniPawnData_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    UBotaniPawnData* NewPawnData;

    if (PawnDataClass != nullptr)
    {
        NewPawnData = NewObject<UBotaniPawnData>(InParent, PawnDataClass, Name, Flags, Context);
    }
    else
    {
        check(Class->IsChildOf(UBotaniPawnData::StaticClass()));
        NewPawnData = NewObject<UBotaniPawnData>(InParent, Class, Name, Flags, Context);
    }

    return NewPawnData;
}

UBotaniGameData_Factory::UBotaniGameData_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UBotaniGameData::StaticClass();
    bCreateNew = true;
    bEditAfterNew = true;
}

UObject* UBotaniGameData_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    UBotaniGameData* NewGameData;

    if (GameDataClass != nullptr)
    {
        NewGameData = NewObject<UBotaniGameData>(InParent, GameDataClass, Name, Flags, Context);
    }
    else
    {
        check(Class->IsChildOf(UBotaniGameData::StaticClass()));
        NewGameData = NewObject<UBotaniGameData>(InParent, Class, Name, Flags, Context);
    }

    return NewGameData;
}

UBotaniExperienceActionSet_Factory::UBotaniExperienceActionSet_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UBotaniExperienceActionSet::StaticClass();
    
    bCreateNew = true;
    bEditAfterNew = true;
}

UObject* UBotaniExperienceActionSet_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    check(Class->IsChildOf(UBotaniExperienceActionSet::StaticClass()));
    return NewObject<UBotaniExperienceActionSet>(InParent, Class, Name, Flags, Context);
}

UBotaniAbilitySet_Factory::UBotaniAbilitySet_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UBotaniAbilitySet::StaticClass();
    
    bCreateNew = true;
    bEditAfterNew = true;
}

UObject* UBotaniAbilitySet_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    check(Class->IsChildOf(UBotaniAbilitySet::StaticClass()));
    return NewObject<UBotaniAbilitySet>(InParent, Class, Name, Flags, Context);
}


UBotaniCameraMode_Factory::UBotaniCameraMode_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UBotaniCameraMode::StaticClass();
    ParentClass = UBotaniCameraMode::StaticClass();
    
    bCreateNew = true;
    bEditAfterNew = true;

    BlueprintType = BPTYPE_Normal;
    bSkipClassPicker = true;
}

UObject* UBotaniCameraMode_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
    if (ParentClass == nullptr && !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("ClassName"), (ParentClass != nullptr) ? FText::FromString(ParentClass->GetName()) : LOCTEXT("Null", "null"));
        
        FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateBlueprint", "Cannot create a blueprint based on class '{ClassName}'."), Args));
        return nullptr;
    }

    UClass* BlueprintClass = nullptr;
    UClass* BlueprintGeneratedClass = nullptr;

    IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
    KismetCompilerModule.GetBlueprintTypesForClass(ParentClass, BlueprintClass, BlueprintGeneratedClass);
    return FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BlueprintType, BlueprintClass, BlueprintGeneratedClass, CallingContext);
}

UBotaniExperienceDefinition_Factory::UBotaniExperienceDefinition_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UBotaniExperienceDefinition::StaticClass();
    ParentClass = UBotaniExperienceDefinition::StaticClass();
    
    bCreateNew = true;
    bEditAfterNew = true;

    BlueprintType = BPTYPE_Normal;
    bSkipClassPicker = true;
}

UObject* UBotaniExperienceDefinition_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
    if (ParentClass == nullptr && !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("ClassName"), (ParentClass != nullptr) ? FText::FromString(ParentClass->GetName()) : LOCTEXT("Null", "null"));
        
        FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateBlueprint", "Cannot create a blueprint based on class '{ClassName}'."), Args));
        return nullptr;
    }

    UClass* BlueprintClass = nullptr;
    UClass* BlueprintGeneratedClass = nullptr;

    IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
    KismetCompilerModule.GetBlueprintTypesForClass(ParentClass, BlueprintClass, BlueprintGeneratedClass);
    return FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BlueprintType, BlueprintClass, BlueprintGeneratedClass, CallingContext);
}

UBotaniGameplayAbility_Factory::UBotaniGameplayAbility_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UBotaniGameplayAbility::StaticClass();
    ParentClass = UBotaniGameplayAbility::StaticClass();
    
    bCreateNew = true;
    bEditAfterNew = true;

    BlueprintType = BPTYPE_Normal;
    bSkipClassPicker = false;
}

bool UBotaniGameplayAbility_Factory::ConfigureProperties()
{
    // nullptr the class so that the user can select a new one
    GameplayAbilityClass = nullptr;

    // Load the class viewer module to display a class picker
    FClassViewerInitializationOptions Options;
    Options.Mode = EClassViewerMode::ClassPicker;
    Options.ExtraPickerCommonClasses.Empty();
    Options.DisplayMode = EClassViewerDisplayMode::ListView;
    Options.bAllowViewOptions = true;
    Options.bExpandRootNodes = true;

    TArray<UClass*> Favs = UBotaniEditorSettings::Get()->GetFavouriteAbilities();
    Options.ExtraPickerCommonClasses = Favs;

    TSharedPtr<FBotaniEditorClassFilter> Filter = MakeShareable(new FBotaniEditorClassFilter);
    Filter->AllowedChildrenOfClasses.Add(UBotaniGameplayAbility::StaticClass());
    Options.ClassFilters.Add(Filter.ToSharedRef());

    const FText TitleText = LOCTEXT("CreateGameplayAbilityOptions", "Pick Gameplay Ability Class");
    UClass* ChosenClass = nullptr;
    const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UBotaniGameplayAbility::StaticClass());

    if (bPressedOk)
    {
        GameplayAbilityClass = ChosenClass;
    }

    return bPressedOk;
}

UObject* UBotaniGameplayAbility_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
    UClass* ClassToUse = (GameplayAbilityClass != nullptr) ? GameplayAbilityClass : ParentClass;

    if (ClassToUse == nullptr && !FKismetEditorUtilities::CanCreateBlueprintOfClass(ClassToUse))
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("ClassName"), (ClassToUse != nullptr) ? FText::FromString(ClassToUse->GetName()) : LOCTEXT("Null", "null"));
        
        FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateBlueprint", "Cannot create a blueprint based on class '{ClassName}'."), Args));
        return nullptr;
    }

    UClass* BlueprintClass = nullptr;
    UClass* BlueprintGeneratedClass = nullptr;

    IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
    KismetCompilerModule.GetBlueprintTypesForClass(ClassToUse, BlueprintClass, BlueprintGeneratedClass);
    return FKismetEditorUtilities::CreateBlueprint(ClassToUse, InParent, Name, BlueprintType, BlueprintClass, BlueprintGeneratedClass, CallingContext);
}

UBotaniGameplayEffect_Factory::UBotaniGameplayEffect_Factory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UGameplayEffect::StaticClass();
    ParentClass = UGameplayEffect::StaticClass();
    
    bCreateNew = true;
    bEditAfterNew = true;

    BlueprintType = BPTYPE_Normal;
    bSkipClassPicker = false;
}

bool UBotaniGameplayEffect_Factory::ConfigureProperties()
{
    // nullptr the class so that the
    GameplayEffectClass = nullptr;

    // Load the class viewer module to display a class picker
    FClassViewerInitializationOptions Options;
    Options.Mode = EClassViewerMode::ClassPicker;
    Options.ExtraPickerCommonClasses.Empty();
    Options.DisplayMode = EClassViewerDisplayMode::ListView;
    Options.bAllowViewOptions = true;
    Options.bExpandRootNodes = true;

    TArray<UClass*> Favs = UBotaniEditorSettings::Get()->GetFavouriteEffects();
    Options.ExtraPickerCommonClasses = Favs;

    TSharedPtr<FBotaniEditorClassFilter> Filter = MakeShareable(new FBotaniEditorClassFilter);
    Filter->AllowedChildrenOfClasses.Add(UGameplayEffect::StaticClass());
    Options.ClassFilters.Add(Filter.ToSharedRef());

    const FText TitleText = LOCTEXT("CreateGameplayEffectOptions", "Pick Gameplay Effect Class");
    UClass* ChosenClass = nullptr;
    const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UGameplayEffect::StaticClass());

    if (bPressedOk)
    {
        GameplayEffectClass = ChosenClass;
    }

    return bPressedOk;
}

UObject* UBotaniGameplayEffect_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
    UClass* ClassToUse = (GameplayEffectClass != nullptr) ? GameplayEffectClass : ParentClass;

    if (ClassToUse == nullptr && !FKismetEditorUtilities::CanCreateBlueprintOfClass(ClassToUse))
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("ClassName"), (ClassToUse != nullptr) ? FText::FromString(ClassToUse->GetName()) : LOCTEXT("Null", "null"));
        
        FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateBlueprint", "Cannot create a blueprint based on class '{ClassName}'."), Args));
        return nullptr;
    }

    UClass* BlueprintClass = nullptr;
    UClass* BlueprintGeneratedClass = nullptr;

    IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
    KismetCompilerModule.GetBlueprintTypesForClass(ClassToUse, BlueprintClass, BlueprintGeneratedClass);
    return FKismetEditorUtilities::CreateBlueprint(ClassToUse, InParent, Name, BlueprintType, BlueprintClass, BlueprintGeneratedClass, CallingContext);
}

IMPLEMENT_MODULE(FBotaniEditorModule, BotaniEditor)
#undef LOCTEXT_NAMESPACE
