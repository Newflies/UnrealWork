// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

UCLASS()
class FDEMO_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class USkeletalMeshComponent* MeshComp;
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintNativeEvent,Category="aaa")
	void HandFire() ;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Sound")
	USoundBase* ShellSound;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Sound")
	USoundBase* HeadSound;
	//如果拥有投射物发射投射物
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Projectile")
	TSubclassOf<AActor>ProjectileClass;
	//该属性仅仅对射线武器有效，伤害类型
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Damage")
	TSubclassOf<class UDamageType> DamageType;
	//射线武器造成的伤害
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Damage")
	float Damage;

	void OnUnEquip();
};
