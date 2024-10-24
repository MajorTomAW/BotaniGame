// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Teams/Data/BotaniTeamDisplayAsset.h"

#include "NiagaraComponent.h"
#include "Teams/Subsystem/BotaniTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniTeamDisplayAsset)


UBotaniTeamDisplayAsset::UBotaniTeamDisplayAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniTeamDisplayAsset::ApplyToMaterial(UMaterialInstanceDynamic* Material)
{
	if (Material == nullptr)
	{
		return;
	}

	// Apply scalar parameters
	for (const auto& KVP : ScalarParameters)
	{
		Material->SetScalarParameterValue(KVP.Key, KVP.Value);
	}

	// Apply color parameters
	for (const auto& KVP : ColorParameters)
	{
		Material->SetVectorParameterValue(KVP.Key, FVector(KVP.Value));
	}

	// Apply texture parameters
	for (const auto& KVP : TextureParameters)
	{
		Material->SetTextureParameterValue(KVP.Key, KVP.Value.LoadSynchronous());
	}
}

void UBotaniTeamDisplayAsset::ApplyToMeshComponent(UMeshComponent* MeshComponent)
{
	if (MeshComponent == nullptr)
	{
		return;
	}

	// Apply scalar parameters
	for (const auto& KVP : ScalarParameters)
	{
		MeshComponent->SetScalarParameterValueOnMaterials(KVP.Key, KVP.Value);
	}

	// Apply color parameters
	for (const auto& KVP : ColorParameters)
	{
		MeshComponent->SetVectorParameterValueOnMaterials(KVP.Key, FVector(KVP.Value));
	}

	// Apply texture parameters
	const TArray<UMaterialInterface*> MaterialInterfaces = MeshComponent->GetMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialInterfaces.Num(); ++MaterialIndex)
	{
		if (UMaterialInterface* MaterialInterface = MaterialInterfaces[MaterialIndex])
		{
			UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MaterialInterface);
			if (!DynamicMaterial)
			{
				DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(MaterialIndex);
			}

			for (const auto& KVP : TextureParameters)
			{
				DynamicMaterial->SetTextureParameterValue(KVP.Key, KVP.Value.LoadSynchronous());
			}
		}
	}
}

void UBotaniTeamDisplayAsset::ApplyToNiagaraComponent(UNiagaraComponent* NiagaraComponent)
{
	if (NiagaraComponent == nullptr)
	{
		return;
	}

	// Apply scalar parameters
	for (const auto& KVP : ScalarParameters)
	{
		NiagaraComponent->SetFloatParameter(KVP.Key, KVP.Value);
	}

	// Apply color parameters
	for (const auto& KVP : ColorParameters)
	{
		NiagaraComponent->SetColorParameter(KVP.Key, KVP.Value);
	}

	// Apply texture parameters
	for (const auto& KVP : TextureParameters)
	{
		NiagaraComponent->SetVariableTexture(KVP.Key, KVP.Value.LoadSynchronous());
	}
}

void UBotaniTeamDisplayAsset::ApplyToActor(AActor* TargetActor, bool bIncludeChildActors)
{
	if (TargetActor == nullptr)
	{
		return;
	}

	TargetActor->ForEachComponent(bIncludeChildActors, [&](UActorComponent* ActorComponent)
	{
		if (UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComponent))
		{
			ApplyToMeshComponent(MeshComponent);
		}
		else if (UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(ActorComponent))
		{
			ApplyToNiagaraComponent(NiagaraComponent);
		}
	});
}

#if WITH_EDITOR
void UBotaniTeamDisplayAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (UBotaniTeamSubsystem* TeamSubsystem : TObjectRange<UBotaniTeamSubsystem>())
	{
		TeamSubsystem->NotifyTeamDisplayAssetChanged(this);
	}
}
#endif
