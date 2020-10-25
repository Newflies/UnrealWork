// Copyright Epic Games, Inc. All Rights Reserved.

#include "FDEMOGameMode.h"
#include "FDEMOCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFDEMOGameMode::AFDEMOGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		
	}
	Score = 0;
}
