// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AIDecisionRuleFactory.h"

#include "AIDecisionRule.h"
#include "KismetCompilerModule.h"
#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "AIDecisionRuleFactory"


UAIDecisionRuleFactory::UAIDecisionRuleFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UAIDecisionRule::StaticClass();
	ParentClass = UAIDecisionRule::StaticClass();
	
	bCreateNew = true;
	bEditAfterNew = true;
	bSkipClassPicker = true;

	BlueprintType = BPTYPE_Normal;
}

UObject* UAIDecisionRuleFactory::FactoryCreateNew(
	UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

UObject* UAIDecisionRuleFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	UClass* ClassToUse =  ParentClass;

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
	return FKismetEditorUtilities::CreateBlueprint(ClassToUse, InParent, InName, BlueprintType, BlueprintClass, BlueprintGeneratedClass, CallingContext);
}

bool UAIDecisionRuleFactory::CanCreateNew() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE