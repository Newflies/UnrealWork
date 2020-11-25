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
	/** ���Ը��� */
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
	/** ��ҵ��������ֵ��������ҵ��������ֵ��Ҳ�ǳ���ʱ������ֵ��*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;

	/** ��ҵĵ�ǰ����ֵ������0�ͱ�ʾ������*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
		float CurrentHealth;

	/** RepNotify������ͬ���Ե�ǰ����ֵ�����ĸ��ġ�*/
	UFUNCTION()
		void OnRep_CurrentHealth();
	/** ��ӦҪ���µ�����ֵ���޸ĺ������ڷ������ϵ��ã����ڿͻ����ϵ�������ӦRepNotify*/
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

	/** ���֮����ӳ٣���λΪ�롣���ڿ��Ʋ��Է����������ٶȣ����ɷ�ֹ������������������½�SpawnProjectileֱ�Ӱ������롣*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	/** ��Ϊtrue�������ڷ���Ͷ���*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool FireButtonDown;

	/** ����������������ĺ�����*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire();

	/** ���ڽ�����������ĺ�����һ��������δ��룬��ҿ��ٴ�ʹ��StartFire��*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();

	/** ��������Ͷ����ķ�����������*/
	UFUNCTION(Server, Reliable)
		void HandleFire();

	/** ��ʱ������������ṩ���ɼ��ʱ���ڵ������ӳ١�*/
	FTimerHandle FiringTimer;
		

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


////////////////////////////////////ADD////////////////////////////////////////////////
	/** �������ֵ��ȡֵ������*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** ��ǰ����ֵ��ȡֵ������*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** ��ǰ����ֵ�Ĵ�ֵ����������ֵ�ķ�Χ�޶���0��MaxHealth֮�䣬������OnHealthUpdate�����ڷ������ϵ��á�*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float healthValue);
	UFUNCTION(BlueprintPure,Category="Action")
        FORCEINLINE bool GetJumpButtonDown()  const  {return JumpButtonDown;}
	/** �����˺����¼�����APawn���ǡ�*/
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
	
	/**��Ϊtrue����������Ծ��*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool JumpButtonDown;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
    bool CrouchButtonDown;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool PunchButtonDown;

	void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
};

