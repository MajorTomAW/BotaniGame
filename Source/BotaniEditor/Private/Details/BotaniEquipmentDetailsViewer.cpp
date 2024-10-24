// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotaniEquipmentDetailsViewer.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorStyleSet.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "PropertyCustomizationHelpers.h"
#include "Definitions/GameplayEquipmentDefinition.h"
#include "Editor/PropertyEditor/Private/SSingleProperty.h"
#include "Inventory/Definitions/BotaniEquipmentDefinition.h"

#define LOCTEXT_NAMESPACE "FBotaniEquipmentDetailsViewer"


TSharedRef<IPropertyTypeCustomization> FBotaniEquipmentDetailsViewer::MakeInstance()
{
	return MakeShared<FBotaniEquipmentDetailsViewer>();
}

void FBotaniEquipmentDetailsViewer::CustomizeHeader(
	TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	MyStructProperty = PropertyHandle;

	HeaderRow
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	];
}

void FBotaniEquipmentDetailsViewer::CustomizeChildren(
	TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// Get the property handle for the struct
	TSharedPtr<IPropertyHandle> StructProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBotaniEquipmentActorToSpawn, bUseAutonomousProxyComponent));
	TSharedPtr<IPropertyHandle> ClassProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBotaniEquipmentActorToSpawn, ActorToSpawn));
	check(StructProperty.IsValid());

	TSharedPtr<IPropertyHandle> Sim_Transform = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBotaniEquipmentActorToSpawn, AttachTransform));
	TSharedPtr<IPropertyHandle> Sim_SocketName = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBotaniEquipmentActorToSpawn, AttachSocket));
	TSharedPtr<IPropertyHandle> Sim_ComponentName = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBotaniEquipmentActorToSpawn, AttachComponent));
	
	TSharedPtr<IPropertyHandle> Aut_Transform = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBotaniEquipmentActorToSpawn, AutonomousProxyAttachTransform));
	TSharedPtr<IPropertyHandle> Aut_SocketName = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBotaniEquipmentActorToSpawn, AutonomousProxyAttachSocket));
	TSharedPtr<IPropertyHandle> Aut_ComponentName = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FBotaniEquipmentActorToSpawn, AutonomousProxyAttachComponent));

	ChildBuilder.AddProperty(ClassProperty.ToSharedRef()).DisplayName(LOCTEXT("EquipmentClass", "Actor to Spawn"));
	ChildBuilder.AddProperty(StructProperty.ToSharedRef()).DisplayName(LOCTEXT("EquipmentAutoChange", "Net-Role relevant equipment?"));

	bool bUseAutonomousProxy;
	StructProperty->GetValue(bUseAutonomousProxy);
	StructProperty->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([&] ()
	{
		ChildBuilder.GetParentCategory().GetParentLayout().ForceRefreshDetails();
	}));

	TAttribute<EVisibility> SimVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateLambda([=]()
	{
		return bUseAutonomousProxy ? EVisibility::Visible : EVisibility::Collapsed;
	}));

	// Autonomous proxies
	IDetailGroup& AutGroup = ChildBuilder.AddGroup("AutonomousGroup", LOCTEXT("AutonomousGroup", "Autonomous Proxy"));
	AutGroup
	.HeaderRow()
	.Visibility(SimVisibility)
	.NameContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SImage)
			.Image_Lambda([=]() -> const FSlateBrush*
			{
				return FAppStyle::GetBrush("ClassIcon.Character");
			})
		]
		+ SHorizontalBox::Slot()
		.Padding(16.f, 0.f, 0.f ,0.f)
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("AutonomousGroup", "Autonomous Proxy (First Person)"))
		]
	];

	AutGroup.AddPropertyRow(Aut_Transform.ToSharedRef()).DisplayName(LOCTEXT("AutonomousTransform", "Attach Transform (FP)"));
	AutGroup.AddPropertyRow(Aut_SocketName.ToSharedRef()).DisplayName(LOCTEXT("AutonomousSocket", "Attach Socket (FP)"));
	AutGroup.AddPropertyRow(Aut_ComponentName.ToSharedRef()).DisplayName(LOCTEXT("AutonomousComponent", "Attach Component (FP)"));

	// Simulated proxies
	IDetailGroup& SimGroup = ChildBuilder.AddGroup("SimulatedGroup", LOCTEXT("SimulatedGroup", "Simulated Proxy"));
	SimGroup.HeaderRow().NameContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SImage)
			.Image_Lambda([=]() -> const FSlateBrush*
			{
				return FAppStyle::GetBrush("ClassIcon.BlendSpace");
			})
		]
		+ SHorizontalBox::Slot()
		.Padding(16.f, 0.f, 0.f, 0.f)
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(bUseAutonomousProxy ? LOCTEXT("SimulatedGroup", "Simulated Proxy (Third Person)") : LOCTEXT("SimulatedGroup", "Character") )
		]
	];

	SimGroup.AddPropertyRow(Sim_Transform.ToSharedRef()).DisplayName(LOCTEXT("SimulatedTransform", "Attach Transform (TP)"));
	SimGroup.AddPropertyRow(Sim_SocketName.ToSharedRef()).DisplayName(LOCTEXT("SimulatedSocket", "Attach Socket (TP)"));
	SimGroup.AddPropertyRow(Sim_ComponentName.ToSharedRef()).DisplayName(LOCTEXT("SimulatedComponent", "Attach Component (TP)"));
	

	// Add the property to the builder
	// ChildBuilder.AddProperty(StructProperty.ToSharedRef());

	/*IDetailPropertyRow& PropRow = ChildBuilder.AddProperty(StructProperty.ToSharedRef());
	IDetailPropertyRow& AutTrans_Row = ChildBuilder.AddProperty(Aut_Transform.ToSharedRef());
	FDetailWidgetRow& Row = ChildBuilder.AddCustomRow(LOCTEXT("EquipmentSearchString", "Equipment"))
	.IsEnabled(true)
	[
		/*SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("EquipmentSearchString", "Equipment"))
		]
		+ SVerticalBox::Slot()
		[
			PropRow.GetPropertyHandle()->CreatePropertyValueWidget()
		]#1#

		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(16.f, 4.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock).Text(LOCTEXT("AutonomousProxy", "Autonomous Proxy"))
			]
			+ SVerticalBox::Slot()
			.Padding(4.f)
			[
				StructProperty->CreatePropertyValueWidget()
			]
		]
		+ SHorizontalBox::Slot()
		.Padding(16.f, 4.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock).Text(LOCTEXT("SimulatedProxies", "Simulated Proxies"))
			]
			+ SVerticalBox::Slot()
			.Padding(4.f)
			[
				Aut_Transform->CreatePropertyValueWidget()
			]
		]
	];*/
	

	//ChildBuilder.AddProperty(Aut_Transform.ToSharedRef());
}


#undef LOCTEXT_NAMESPACE
