// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Feedback/NumberPops/Components/BotaniNumPopComponent_NiagaraText.h"

#include "BotaniLogChannels.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Feedback/NumberPops/Data/BotaniNumPopStyle_Niagara.h"
#include "Kismet/KismetSystemLibrary.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniNumPopComponent_NiagaraText)

UBotaniNumPopComponent_NiagaraText::UBotaniNumPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UBotaniNumPopComponent_NiagaraText* UBotaniNumPopComponent_NiagaraText::FindNumPopComponent(const AActor* Actor)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UBotaniNumPopComponent_NiagaraText>();
}

void UBotaniNumPopComponent_NiagaraText::AddNumberPop(const FBotaniNumPopRequest& NewRequest)
{
	int32 LocalDamage = NewRequest.NumberToDisplay;

	//Add a NiagaraComponent if we don't already have one
	if (!NiagaraComp)
	{
		NiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		if (Style != nullptr)
		{
			NiagaraComp->SetAsset(Style->TextNiagara);
			NiagaraComp->bAutoActivate = false;
			
		}
		NiagaraComp->SetupAttachment(nullptr);
		check(NiagaraComp);
		NiagaraComp->RegisterComponent();
	}


	NiagaraComp->Activate(false);
	NiagaraComp->SetWorldLocation(NewRequest.WorldLocation);

	UE_LOG(LogBotani, Log, TEXT("DamageHit location : %s"), *(NewRequest.WorldLocation.ToString()));
	//Add Damage information to the current Niagara list - Damage informations are packed inside a FVector4 where XYZ = Position, W = Damage
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(NiagaraComp, Style->NiagaraArrayName);
	DamageList.Add(FVector4(NewRequest.WorldLocation.X, NewRequest.WorldLocation.Y, NewRequest.WorldLocation.Z, LocalDamage));
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, Style->NiagaraArrayName, DamageList);
}
