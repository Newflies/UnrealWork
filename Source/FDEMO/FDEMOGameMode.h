// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FDEMOGameMode.generated.h"

UCLASS(minimalapi)
class AFDEMOGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFDEMOGameMode();
	UPROPERTY(VisibleAnywhere)
	uint16 Score;
};



