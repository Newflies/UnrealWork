// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FDEMOPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FDEMO_API AFDEMOPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()
	AFDEMOPlayerState();
	void ScoreKill(AFDEMOPlayerState* Victim, int32 Points);
	
	/** helper for scoring points */
	
	void ScoreDeath(AFDEMOPlayerState* KilledBy, int32 Points);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Transient,Replicated)
	int32 NumKills=0;
	UPROPERTY(Transient,Replicated)
	/** number of deaths */
	int32 NumDeaths=0;
	public:
	UFUNCTION(BlueprintCallable)
	int32 GetKills();
	UFUNCTION(BlueprintCallable)
	int32 GetDeath();
};
