// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "PhysXInterfaceWrapperCore.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent=MeshComp;
	MuzzleCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("MuzzleCamera"));
	MuzzleCamera->SetupAttachment(RootComponent,"Muzzle");
	static ConstructorHelpers::FObjectFinder<USoundBase> DefaultSound(TEXT("/Game/HumanVocalizations/HumanMaleA/Cues/voice_male_grunt_pain_01_Cue.voice_male_grunt_pain_01_Cue"));
	if(DefaultSound.Succeeded())
	{
		ShellSound=DefaultSound.Object;
	}
	static  ConstructorHelpers::FObjectFinder<USoundBase> DefaultHeadSound(TEXT("/Game/HumanVocalizations/HumanMaleA/Cues/voice_male_grunt_pain_02_Cue.voice_male_grunt_pain_02_Cue"));
	if(DefaultHeadSound.Succeeded())
	{
		HeadSound=DefaultHeadSound.Object;
	}
	DamageType=UDamageType::StaticClass();
	Damage=10.0f;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWeapon::OnUnEquip()
{
	MeshComp->DestroyComponent();
	Destroy();
}

void ASWeapon::HandFire_Implementation() 
{
	
	AActor* MyOwner=GetOwner();
	if(MyOwner)
	{
			FVector EyeLocation;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation,EyeRotation);

			if(!ProjectileClass)
			{
				FVector TraceEnd=EyeLocation+(EyeRotation.Vector()*10000);
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);
				QueryParams.AddIgnoredActor(MyOwner);
				QueryParams.bTraceComplex=true;
				FHitResult HitResult;
				AController* Mycontroller= Cast<APawn>(GetOwner())->Controller;

				if(GetWorld()->LineTraceSingleByChannel(HitResult,EyeLocation,TraceEnd,ECC_Camera,QueryParams))
				{
					if (HitResult.BoneName.ToString()!="None")
					{
						if(HitResult.BoneName.ToString()!="neck_01")
						{
							UGameplayStatics::PlaySound2D(GetWorld(),ShellSound);
							UGameplayStatics::ApplyPointDamage(HitResult.GetActor(),Damage,GetActorLocation(),HitResult,Mycontroller,GetOwner(),DamageType);
						}
						else
						{
							UGameplayStatics::PlaySound2D(GetWorld(),HeadSound);
							UGameplayStatics::ApplyPointDamage(HitResult.GetActor(),2*Damage,GetActorLocation(),HitResult,Mycontroller,GetOwner(),DamageType);
						}
					}

				}
			}
		else if(ProjectileClass)
		{
			FVector MuzzleLocation = MeshComp->GetSocketLocation("Muzzle");

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			//在枪口发射子弹
			GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation,EyeRotation,SpawnParams);
		}
	}
	
}



