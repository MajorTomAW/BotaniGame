// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Weapons/Templates/BotaniExplosionTemplate.h"

#include "NiagaraFunctionLibrary.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniExplosionTemplate)

ABotaniExplosionTemplate::ABotaniExplosionTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AActor::SetLifeSpan(1.f);
}

void ABotaniExplosionTemplate::BeginPlay()
{
	Super::BeginPlay();

	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation(), GetActorRotation());
	}
}

void ABotaniExplosionTemplate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
