// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FDEMOGameState.generated.h"

/**
 * 
 */
UCLASS()
class FDEMO_API AFDEMOGameState : public AGameStateBase
{
	GENERATED_BODY()
	AFDEMOGameState();
	public:
	UPROPERTY(BlueprintReadOnly,Transient, Replicated)
	int32 RemainingTime;

	/** is timer paused? */
	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;
	
};
