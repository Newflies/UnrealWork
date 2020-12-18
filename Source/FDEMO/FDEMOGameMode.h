// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FDEMOPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "FDEMOGameMode.generated.h"

UCLASS(minimalapi)
class AFDEMOGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// float Timer;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool IsStart;
	
	AFDEMOGameMode();

	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	virtual bool CanDealDamage(AFDEMOPlayerState* DamageInstigator, AFDEMOPlayerState* DamagedPlayer) const;

	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);
	
	UFUNCTION(BlueprintCallable)
	void StartMatch();
	UFUNCTION(BlueprintNativeEvent)
	void EndMatch() const;

	virtual void PreInitializeComponents() override;

	FTimerHandle TimerHandle_Timer;

	virtual void DefaultTimer();

};



