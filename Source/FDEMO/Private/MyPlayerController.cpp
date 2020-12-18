// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "FDEMO/FDEMOCharacter.h"
#include "FDEMO/FDEMOGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AMyPlayerController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);
    if(InPawn)
    {
        auto PossessedCharacter=Cast<AFDEMOCharacter>(InPawn);
        if(!PossessedCharacter)return;
        // OnDeath.AddUniqueDynamic(this,&AMyPlayerController::OnControlledCharacterDeath);
     }
}
void AMyPlayerController::OnControlledCharacterDeath()
{
    MyCharacter=GetControlledCharacter();
    GetWorldTimerManager().SetTimer(
    CharacterspawnHandle,
    this,
    &AMyPlayerController::CharacterSpawn,
    RespawnTime,
    false
    );
}

AFDEMOCharacter* AMyPlayerController::GetControlledCharacter()
{
    return  Cast<AFDEMOCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
}

void AMyPlayerController::OnKill()
{

}

void AMyPlayerController::CharacterSpawn_Implementation()
{
 
    // ServerRestartPlayer();
    AFDEMOGameMode* tempMode=GetWorld()->GetAuthGameMode<AFDEMOGameMode>();
}


