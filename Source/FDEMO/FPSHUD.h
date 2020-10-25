// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSHUD.generated.h"

/**
 * 
 */
UCLASS()
class FDEMO_API AFPSHUD : public AHUD
{
	GENERATED_BODY()
protected:
    // ������Ļ������ƴ��
    UPROPERTY(EditDefaultsOnly)
        UTexture2D* CrosshairTexture;
public:
    // HUD�������Ƶ��á�
    virtual void DrawHUD() override;
};
