// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SWeapon.h"
#include "GameFramework/Character.h"
#include "FDEMOCharacter.generated.h"

UCLASS(config=Game)
class AFDEMOCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	

	
public:
	AFDEMOCharacter();
	/** 属性复制 */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Weapon)
	ASWeapon* CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly,Category="Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;
	
	UPROPERTY(EditDefaultsOnly,Category="Player")
	FName WeaponAttachSocketName;
	
protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/*---------------------------------------------------------------------------------------*/
	/** 玩家的最大生命值。这是玩家的最高生命值，也是出生时的生命值。*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;

	/** 玩家的当前生命值。降到0就表示死亡。*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
		float CurrentHealth;

	/** RepNotify，用于同步对当前生命值所做的更改。*/
	UFUNCTION()
		void OnRep_CurrentHealth();
	/** 响应要更新的生命值。修改后，立即在服务器上调用，并在客户端上调用以响应RepNotify*/
	void OnHealthUpdate();
	/*---------------------------------------------------------------------------------------*/

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	/*Fire*/

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
		TSubclassOf<class AThirdPersonMPProjectile> ProjectileClass;

	/** 射击之间的延迟，单位为秒。用于控制测试发射物的射击速度，还可防止服务器函数的溢出导致将SpawnProjectile直接绑定至输入。*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	/** 若为true，则正在发射投射物。*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool FireButtonDown;

	/** 用于启动武器射击的函数。*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire();

	/** 用于结束武器射击的函数。一旦调用这段代码，玩家可再次使用StartFire。*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();

	/** 用于生成投射物的服务器函数。*/
	UFUNCTION(Server, Reliable)
		void HandleFire();

	/** 定时器句柄，用于提供生成间隔时间内的射速延迟。*/
	FTimerHandle FiringTimer;
		

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


////////////////////////////////////ADD////////////////////////////////////////////////
	/** 最大生命值的取值函数。*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** 当前生命值的取值函数。*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** 当前生命值的存值函数。将此值的范围限定在0到MaxHealth之间，并调用OnHealthUpdate。仅在服务器上调用。*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float healthValue);
	UFUNCTION(BlueprintPure,Category="Action")
        FORCEINLINE bool GetJumpButtonDown()  const  {return JumpButtonDown;}
	/** 承受伤害的事件。从APawn覆盖。*/
	//UFUNCTION(BlueprintCallable, Category = "Health")
	//	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	float PlayerScore;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void AddScore(int32 _num);
	UFUNCTION(BlueprintCallable, Category = "Health")
		void Minus_1_health();
	UFUNCTION(BlueprintCallable, Category = "Health")
		void ReLoadBullet();
	UFUNCTION(BlueprintCallable, Category = "Action")
		void CharacterJump();
	UFUNCTION(BlueprintCallable,Category="Action")
		void CharacterStopJumping();
	UFUNCTION(BlueprintCallable,Category="Action")
		void CharacterCrouching();
	UFUNCTION(BlueprintCallable,Category="Action")
		void CharacterStopCrouching();
	UFUNCTION(BlueprintCallable,Category="Action")
		void CharacterPunching();
	UFUNCTION(BlueprintCallable,Category="Action")
		void CharacterStopPunching();
	
	/**若为true，则正在跳跃。*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool JumpButtonDown;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
    bool CrouchButtonDown;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool PunchButtonDown;

	void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
};

