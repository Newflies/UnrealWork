// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDEMO/FDEMOCharacter.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FDEMO_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
    public:
    virtual  void SetPawn(APawn* InPawn) override;
    UFUNCTION(BlueprintCallable)
    void OnControlledCharacterDeath();

    UFUNCTION(BlueprintCallable)
    AFDEMOCharacter* GetControlledCharacter();
    UPROPERTY(EditAnywhere)
    float RespawnTime=5.0f;

    FTimerHandle CharacterspawnHandle;

    AFDEMOCharacter* MyCharacter=nullptr;
    UFUNCTION(BlueprintNativeEvent)
    void CharacterSpawn();

    void OnKill();

};
