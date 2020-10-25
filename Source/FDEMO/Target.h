// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.generated.h"

UCLASS()
class FDEMO_API ATarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATarget();
	UPROPERTY(VisibleAnywhere, Category = "MyVar")
		float localScore;
	UPROPERTY(VisibleAnywhere, Category = "MyVar")
		float timer;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "MyVar")
	FVector startPos;
	
	void HitEvent();

	UFUNCTION(Server, Reliable, WithValidation)
	void HitServer(int32 _inint);

	void HitServer_Implementation(int32 _inint);

	bool HitServer_Validate(int32 _num);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* StaticMesh;
	UFUNCTION(BlueprintCallable, Category = "MyFunc")
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "MyFunc")
		void ReportAndDestory(float _time);
	UFUNCTION(BlueprintCallable, Category = "MyFunc")
		void MoveArond(float DeltaTime);
};
