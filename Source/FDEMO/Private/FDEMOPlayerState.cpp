// Fill out your copyright notice in the Description page of Project Settings.


#include "FDEMOPlayerState.h"
#include "Net/UnrealNetwork.h"




AFDEMOPlayerState::AFDEMOPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    NumKills=0;
    NumDeaths=0;
}

void AFDEMOPlayerState::ScoreKill(AFDEMOPlayerState* Victim, int32 Points)
{
    NumKills++;
    
}



void AFDEMOPlayerState::ScoreDeath(AFDEMOPlayerState* KilledBy, int32 Points)
{
    NumDeaths++;
}

void AFDEMOPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME( AFDEMOPlayerState, NumKills );
    DOREPLIFETIME( AFDEMOPlayerState, NumDeaths );
}





int AFDEMOPlayerState::GetKills()
{
    return  NumKills;
}

int AFDEMOPlayerState::GetDeath()
{
    return  NumDeaths;
}


