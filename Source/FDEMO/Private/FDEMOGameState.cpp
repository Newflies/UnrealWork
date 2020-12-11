// Fill out your copyright notice in the Description page of Project Settings.


#include "FDEMOGameState.h"

#include "Net/UnrealNetwork.h"


AFDEMOGameState::AFDEMOGameState()
{
    RemainingTime=0;
    bTimerPaused=false;
}
void AFDEMOGameState::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFDEMOGameState,bTimerPaused);
    DOREPLIFETIME(AFDEMOGameState,RemainingTime);
}
