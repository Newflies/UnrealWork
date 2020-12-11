// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SWeapon.h"
#include "GameFramework/Character.h"
#include "FDEMOCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDelegate);  //��F��ͷ�Ĵ�������



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

	virtual  void Tick(float DeltaSeconds) override;
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Weapon)
	USkeletalMeshComponent* WeaponMesh;
	bool bIsDying=false;
protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	virtual  void OnDeath();


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
	UFUNCTION(Server,Reliable)
        void StartFireServer();
	UFUNCTION(NetMulticast,Reliable)
        void StartFireMulticast();


	/** ���ڽ�����������ĺ�����һ��������δ��룬��ҿ��ٴ�ʹ��StartFire��*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
        void StopFire();
	UFUNCTION(Server,Reliable)
        void StopFireServer();
	UFUNCTION(NetMulticast,Reliable)
        void StopFireMulticast();

	/** ��������Ͷ����ķ�����������*/
	UFUNCTION(Server, Reliable)
        void HandleFire();

	/** ��ʱ������������ṩ���ɼ��ʱ���ڵ������ӳ١�*/
	FTimerHandle FiringTimer;

	/** ������ݻٵľ��*/
	FTimerHandle DeathTimer;

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
	UFUNCTION(BlueprintCallable, Category = "Health")
        float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	float PlayerScore;

	UFUNCTION(BlueprintCallable, Category = "Health")
        void AddScore(int32 _num);
	UFUNCTION(BlueprintCallable, Category = "Health")
        void Minus_1_health();
	UFUNCTION(BlueprintCallable, Category = "Health")
        void ReLoadBullet();
	//��Ծ����ͬ��
	UFUNCTION(BlueprintCallable, Category = "Action")
        void CharacterJump();
	UFUNCTION(Server,Reliable)
        void CharacterJumpServer();
	UFUNCTION(NetMulticast,Reliable)
        void CharacterJumpMulticast();
	
	//ֹͣ��Ծ����ͬ��
	UFUNCTION(BlueprintCallable,Category="Action")
        void CharacterStopJumping();
	UFUNCTION(Server,Reliable)
        void CharacterStopJumpingServer();
	UFUNCTION(NetMulticast,Reliable)
        void CharacterStopJumpingMulticast();
	
	//���º���ͬ��
	UFUNCTION(BlueprintCallable,Category="Action")
        void CharacterCrouching();
	UFUNCTION(Server,Reliable)
        void CharacterCrouchingServer();
	UFUNCTION(NetMulticast,Reliable)
        void CharacterCrouchingMulticast();
	
	//ֹͣ���º���ͬ��
	UFUNCTION(BlueprintCallable,Category="Action")
        void CharacterStopCrouching();
	UFUNCTION(Server,Reliable)
        void CharacterStopCrouchingServer();
	UFUNCTION(NetMulticast,Reliable)
        void CharacterStopCrouchingMulticast();
	//��ս����ͬ��
	UFUNCTION(BlueprintCallable,Category="Action")
        void CharacterPunching();
	UFUNCTION(Server,Reliable)
        void CharacterPunchingServer();
	UFUNCTION(NetMulticast,Reliable)
        void CharacterPunchingMulticast();
	//ֹͣ��ս����ͬ��
	UFUNCTION(BlueprintCallable,Category="Action")
        void CharacterStopPunching();
	UFUNCTION(Server,Reliable)
        void CharacterStopPunchingServer();
	UFUNCTION(NetMulticast,Reliable)
        void CharacterStopPunchingMulticast();
	UFUNCTION(BlueprintCallable,Category="Action")
		void CharacterDeath();
	UFUNCTION(Server,Reliable)
		void CharacterDeathServer();
	UFUNCTION(NetMulticast,Reliable)
		void CharacterDeathMulticast();
	
	/**��Ϊtrue����������Ծ��*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool JumpButtonDown;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool CrouchButtonDown;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	bool PunchButtonDown;

	void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player")
	float ZoomedFov;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = player,meta=(ClampMin=0.0,ClampMax=100))
	float ZoomInterSpeed;

	/** Returns True if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;
	/**
	* Kills pawn.  Server/authority only.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	float DefaultFOV;
	void BeginZoom();
	void EndZoom();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	float C_Pitch;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Action")
	float C_Yaw;

	void DeathDestroy();

	void OwnDestroy();

};
	 

