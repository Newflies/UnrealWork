// Copyright Epic Games, Inc. All Rights Reserved.

#include "FDEMOCharacter.h"

#include <tuple>


#include "DrawDebugHelpers.h"
#include "FDEMOGameMode.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "ThirdPersonMPProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// AFDEMOCharacter

AFDEMOCharacter::AFDEMOCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	WeaponMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaopnMesh"));
	WeaponMesh->SetupAttachment(GetMesh(),WeaponAttachSocketName);
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	//初始化玩家生命值
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	//初始化投射物类
	ProjectileClass = AThirdPersonMPProjectile::StaticClass();
	//初始化射速
	FireRate = 0.25f;
	FireButtonDown = false;
	//玩家分数
	PlayerScore = 0;
	//

	//
	WeaponAttachSocketName="WeaponSocket";
	DefaultFOV=100;
	ZoomedFov=50;
	bReplicates=true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFDEMOCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFDEMOCharacter::CharacterJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFDEMOCharacter::CharacterStopJumping);
	PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&AFDEMOCharacter::CharacterCrouching);
	PlayerInputComponent->BindAction("Crouch",IE_Released,this,&AFDEMOCharacter::CharacterStopCrouching);

	
	PlayerInputComponent->BindAxis("MoveForward", this, &AFDEMOCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFDEMOCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFDEMOCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFDEMOCharacter::LookUpAtRate);

	// handle touch devices
	// PlayerInputComponent->BindTouch(IE_Pressed, this, &AFDEMOCharacter::TouchStarted);
	// PlayerInputComponent->BindTouch(IE_Released, this, &AFDEMOCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFDEMOCharacter::OnResetVR);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFDEMOCharacter::StartFire);
	// PlayerInputComponent->BindAction("Fire",IE_Released,this,&AFDEMOCharacter::StopFire);
	PlayerInputComponent->BindAction("ReLoad",IE_Pressed,this,&AFDEMOCharacter::ReLoadBullet);
	PlayerInputComponent->BindAction("Punch",IE_Pressed,this,&AFDEMOCharacter::CharacterPunching);
	PlayerInputComponent->BindAction("Punch",IE_Released,this,&AFDEMOCharacter::CharacterStopPunching);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AFDEMOCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AFDEMOCharacter::EndZoom);
}


void AFDEMOCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AFDEMOCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AFDEMOCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AFDEMOCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFDEMOCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFDEMOCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFDEMOCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AFDEMOCharacter::OnDeath()
{

}




//////////////////////////////////////////////////////////////////////////
// 复制的属性

void AFDEMOCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//复制当前生命值。
	DOREPLIFETIME(AFDEMOCharacter, CurrentHealth);
}

void AFDEMOCharacter::OnHealthUpdate()
{
	//客户端特定的功能
	 if (IsLocallyControlled())
	 {
	 	FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
	 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	
	 	if (CurrentHealth <= 0)
	 	{
	 		FString deathMessage = FString::Printf(TEXT("You have been killed."));
	 		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
	 	}
	 }

	//服务器特定的功能
	 if (GetLocalRole() == ROLE_Authority)
	 {
	 	FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
	 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	 }

	//在所有机器上都执行的函数。 
	/*
		因任何因伤害或死亡而产生的特殊功能都应放在这里。
	*/
	if(CurrentHealth<=0)
	{
		CharacterDeath();
		// OnDeath.Broadcast();
	
	}

}

void AFDEMOCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AFDEMOCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

void AFDEMOCharacter::StartFire()
{
StartFireServer();
}

void AFDEMOCharacter::StartFireServer_Implementation()
{
	StartFireMulticast();
}

void AFDEMOCharacter::StartFireMulticast_Implementation()
{
	if (!FireButtonDown&&GetCurrentHealth()>0)
	{
		FireButtonDown = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &AFDEMOCharacter::StopFire, FireRate, false);	
		if(CurrentWeapon)
		{
			CurrentWeapon->HandFire();


		}
	}
}

void AFDEMOCharacter::StopFire()
{
	StopFireServer();
}

void AFDEMOCharacter::StopFireServer_Implementation()
{
	StopFireMulticast();
}

void AFDEMOCharacter::StopFireMulticast_Implementation()
{
	FireButtonDown = false;

}



void AFDEMOCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f)+FVector(0,40,0);
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;
		// AThirdPersonMPProjectile* spawnedProjectile = GetWorld()->SpawnActor<AThirdPersonMPProjectile>(spawnLocation, spawnRotation, spawnParameters);
	// UClass* MyProjectile=	LoadClass<AThirdPersonMPProjectile>(nullptr,TEXT("BluePrint'/Game/BluePrints/MyThirdPersonMPProjectile.MyThirdPersonMPProjectile_C'"));
	// GetWorld()->SpawnActor(MyProjectile);

}
	void AFDEMOCharacter::AddScore(int32 _num)
	{
		PlayerScore += _num;
		// GEngine->AddOnScreenDebugMessage
		// (
		// 	-1,
		// 	10, 			//	显示的时间/秒
		// 	FColor::Blue, 	//	显示的颜色
		// 	"Score:" + FString::SanitizeFloat(PlayerScore)	//	显示的信息
		// );
	}
	void AFDEMOCharacter::Minus_1_health()
	{
		CurrentHealth -= 1;
	}
	void AFDEMOCharacter::ReLoadBullet()
	{
		CurrentHealth=MaxHealth;
	}
	void AFDEMOCharacter::CharacterJump()
	{
	CharacterJumpServer();	
	}

void AFDEMOCharacter::CharacterJumpServer_Implementation()
{
	CharacterJumpMulticast();
}

void AFDEMOCharacter::CharacterJumpMulticast_Implementation()
{
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;
	JumpButtonDown=true;	
}

void  AFDEMOCharacter::CharacterStopJumping ()
	{
	CharacterStopJumpingServer();
	}

void AFDEMOCharacter::CharacterStopJumpingServer_Implementation()
{
	CharacterStopJumpingMulticast();
}

void AFDEMOCharacter::CharacterStopJumpingMulticast_Implementation()
{		bPressedJump = false;
	JumpButtonDown=false;
	ResetJumpState();
}

void AFDEMOCharacter::CharacterCrouching()
	{
	CharacterCrouchingServer();
	}

void AFDEMOCharacter::CharacterCrouchingServer_Implementation()
{
	CharacterCrouchingMulticast();
}

void AFDEMOCharacter::CharacterCrouchingMulticast_Implementation()
{
	CrouchButtonDown=true;

}

void AFDEMOCharacter::CharacterStopCrouching()
	{
	CharacterStopCrouchingServer();
	}

void AFDEMOCharacter::CharacterStopCrouchingServer_Implementation()
{
	CharacterStopCrouchingMulticast();
}

void AFDEMOCharacter::CharacterStopCrouchingMulticast_Implementation()
{
	CrouchButtonDown=false;

}

void AFDEMOCharacter::CharacterPunching()
	{
	CharacterPunchingServer();
	}

void AFDEMOCharacter::CharacterPunchingServer_Implementation()
{
	CharacterPunchingMulticast();
}

void AFDEMOCharacter::CharacterPunchingMulticast_Implementation()
{
	PunchButtonDown=true;
}

void AFDEMOCharacter::CharacterStopPunching()
	{
	CharacterStopPunchingServer();
	}



void AFDEMOCharacter::CharacterDeath()
{
	CharacterDeathServer();
}

void AFDEMOCharacter::CharacterDeathMulticast_Implementation()
{
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetAllBodiesPhysicsBlendWeight(1.0f);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AFDEMOCharacter::CharacterDeathServer_Implementation()
{
	CharacterDeathMulticast();
}




void AFDEMOCharacter::CharacterStopPunchingServer_Implementation()
{
	CharacterStopPunchingMulticast();
}



void AFDEMOCharacter::CharacterStopPunchingMulticast_Implementation()
{
	PunchButtonDown=false;
}



void AFDEMOCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if(FollowCamera)
	{
		OutLocation=FollowCamera->GetComponentLocation();
		OutRotation=FollowCamera->GetComponentRotation();
		return;
	}
	Super::GetActorEyesViewPoint(OutLocation, OutRotation);
}

bool AFDEMOCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer,
	AActor* DamageCauser) const
{
	if(bIsDying
		||IsPendingKill()
		||GetLocalRole()!=ROLE_Authority
		||GetWorld()->GetAuthGameMode<AFDEMOGameMode>()==NULL
		)
	{
		return  false;
	}
	return  true;
}

bool AFDEMOCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer,
                          AActor* DamageCauser)
{
	if(!CanDie(KillingDamage,DamageEvent,Killer,DamageCauser))
	{
		return  false;
	}	
	
		CharacterDeath();
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);
	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
		GetWorld()->GetAuthGameMode<AFDEMOGameMode>()->Killed(Killer,KilledPlayer,this,DamageType);
		DeathDestroy();
	GetWorld()->GetAuthGameMode<AFDEMOGameMode>()->StartMatch();
	return true;
}

void AFDEMOCharacter::BeginZoom()
{

}

void AFDEMOCharacter::EndZoom()
{

}

void AFDEMOCharacter::DeathDestroy()
{
	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &AFDEMOCharacter::OwnDestroy, 2.0f, false);	

}

void AFDEMOCharacter::OwnDestroy()
{
	
	AMyPlayerController* tempController=Cast<AMyPlayerController>(Controller);
	if(tempController!=nullptr)
	{
		tempController->UnPossess();
		AFDEMOGameMode* tempMode=GetWorld()->GetAuthGameMode<AFDEMOGameMode>();
		tempMode->RestartPlayer(tempController);
		if(CurrentWeapon!=NULL)
		CurrentWeapon->SetActorHiddenInGame(true);
		Destroy();
	}

}

void AFDEMOCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	float TargetFOV = bWantsToZoom ? ZoomedFov : DefaultFOV;
	float CurrentFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaSeconds, ZoomInterSpeed);
	FollowCamera->SetFieldOfView(CurrentFOV);
}
float AFDEMOCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	AMyPlayerController* MyPC=Cast<AMyPlayerController>(Controller);
	if(CurrentHealth<=0.f)
	{
		return  0.f;
	}
	AFDEMOGameMode* const Game=GetWorld()->GetAuthGameMode<AFDEMOGameMode>();
	DamageTaken=Game?Game->ModifyDamage(DamageTaken,this
		,DamageEvent,EventInstigator,DamageCauser):0.f;

	const float ActualDamage=Super::TakeDamage(DamageTaken, DamageEvent, EventInstigator, DamageCauser);
	if(ActualDamage>0.f)
	{
		CurrentHealth-=ActualDamage;
		if(CurrentHealth<=0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);	
		}
	}
	return  ActualDamage;
}








