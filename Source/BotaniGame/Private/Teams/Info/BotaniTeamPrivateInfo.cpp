// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Teams/Info/BotaniTeamPrivateInfo.h"


// Sets default values
ABotaniTeamPrivateInfo::ABotaniTeamPrivateInfo()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABotaniTeamPrivateInfo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABotaniTeamPrivateInfo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

