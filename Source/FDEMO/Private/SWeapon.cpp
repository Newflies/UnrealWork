// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent=MeshComp;
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
void ASWeapon::HandFire() 
{
	
	AActor* MyOwner=GetOwner();
	if(MyOwner)
	{

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation,EyeRotation);
		FVector TraceEnd=EyeLocation+(EyeRotation.Vector()*1000);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.bTraceComplex=true;
		FHitResult HitResult;
		GEngine->AddOnScreenDebugMessage
(
-1,
10, 			//	显示的时间/秒
FColor::Blue, 	//	显示的颜色
"1"+EyeLocation.ToString()	//	显示的信息
);
		GEngine->AddOnScreenDebugMessage
(
-1,
10, 			//	显示的时间/秒
FColor::Blue, 	//	显示的颜色
EyeRotation.ToString()	//	显示的信息
);		
		if(GetWorld()->LineTraceSingleByChannel(HitResult,EyeLocation,TraceEnd,ECC_Visibility,QueryParams))
		{

		}
		DrawDebugLine(GetWorld(),EyeLocation,TraceEnd,FColor::Red,false,1.0f,1.0f);
	}
	return;
}



