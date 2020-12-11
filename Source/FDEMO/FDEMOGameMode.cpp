// Copyright Epic Games, Inc. All Rights Reserved.

#include "FDEMOGameMode.h"


#include "FDEMOCharacter.h"
#include "FDEMOGameState.h"
#include "FDEMOPlayerState.h"
#include "GameFramework/GameState.h"
#include "UObject/ConstructorHelpers.h"

AFDEMOGameMode::AFDEMOGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		
	}
}

float AFDEMOGameMode::ModifyDamage(float Damage, AActor* DamagedActor, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage= Damage;
	AFDEMOCharacter* DamagedPawn=Cast<AFDEMOCharacter>(DamagedActor);
	if(DamagedPawn&& EventInstigator)
	{
		AFDEMOPlayerState* DamagedPlayerState=Cast<AFDEMOPlayerState>(DamagedPawn->GetPlayerState());
		AFDEMOPlayerState* InstigatorPlayerState=Cast<AFDEMOPlayerState>(EventInstigator->PlayerState);

		//disable friendly fire
		if(!CanDealDamage(InstigatorPlayerState,DamagedPlayerState))
		{
			ActualDamage=0.f;
		}
		if(InstigatorPlayerState==DamagedPlayerState)
		{
			ActualDamage*=0;
		}
	}
	return  ActualDamage;
}

bool AFDEMOGameMode::CanDealDamage(AFDEMOPlayerState* DamageInstigator, AFDEMOPlayerState* DamagedPlayer) const
{
	return  true;
}

void AFDEMOGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn,
	const UDamageType* DamageType)
{
	AFDEMOPlayerState* KillerPlayerState = Killer ? Cast<AFDEMOPlayerState>(Killer->PlayerState) : NULL;
	AFDEMOPlayerState* VictimPlayerState = KilledPlayer ? Cast<AFDEMOPlayerState>(KilledPlayer->PlayerState) : NULL;
	
	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
		KillerPlayerState->ScoreKill(VictimPlayerState, 0);
	}
	
	if (VictimPlayerState)
	{
		VictimPlayerState->ScoreDeath(KillerPlayerState, 0);
	}
}

void AFDEMOGameMode::StartMatch()
{
	AFDEMOGameState* const MyGameState=Cast<AFDEMOGameState>(GameState);
	if(MyGameState->RemainingTime!=0)
		return;
	MyGameState->RemainingTime=10;
	IsStart=true;
}

void AFDEMOGameMode::EndMatch_Implementation() const
{
}



void AFDEMOGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	 GetWorldTimerManager().SetTimer(TimerHandle_Timer,this,&AFDEMOGameMode::DefaultTimer,GetWorldSettings()->GetEffectiveTimeDilation(),true);
}

void AFDEMOGameMode::DefaultTimer()
{
	if(!IsStart)
		return;
	 AFDEMOGameState* const MyGameState=Cast<AFDEMOGameState>(GameState);
	if(MyGameState && MyGameState->RemainingTime>0&&!MyGameState->bTimerPaused)
	{
		MyGameState->RemainingTime--;
		if(MyGameState->RemainingTime==0)
		{
			EndMatch();
		}
	}
	
}


