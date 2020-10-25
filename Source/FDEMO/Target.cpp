// Fill out your copyright notice in the Description page of Project Settings.


#include "Target.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "FDEMOCharacter.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATarget::ATarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/MyMesh/mark"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector(0, 0, 0));
		StaticMesh->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));
		StaticMesh->SetRelativeRotation(FQuat::MakeFromEuler(FVector(90, 0, 90)));
		StaticMesh->OnComponentBeginOverlap.AddDynamic(this,&ATarget::OnHit);
	}
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();
	localScore = 0;
	timer = 100;
	startPos = GetActorLocation();
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation();
	float RunningTime = GetGameTimeSinceCreation();
	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	NewLocation.Y += DeltaHeight * 50.0f;       //Scale our height by a factor of 20
	SetActorLocationAndRotation(NewLocation, GetActorRotation());

	timer = timer > 0 ? timer - 0.1 : timer;
	ReportAndDestory(timer);

}
void ATarget::OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	UWorld* world = GetWorld();
	localScore += 1;
//if (!world->IsServer()) HitEvent();
}
void ATarget::ReportAndDestory(float _time)
{
	if (_time > 0) return;
	GEngine->AddOnScreenDebugMessage
	(
		-1,
		10, 			//	显示的时间/秒
		FColor::Blue, 	//	显示的颜色
		"Score:" + FString::SanitizeFloat(localScore)	//	显示的信息
	);
	Destroy();
}
void ATarget::MoveArond(float DeltaTime)
{
	float Deltadisplacement = (FMath::Sin(GetGameTimeSinceCreation() + DeltaTime) - FMath::Sin(GetGameTimeSinceCreation()));
	FVector newLocation=startPos;
	newLocation.Y += Deltadisplacement * 20.0f;
	SetActorLocation(newLocation);
}
void ATarget::HitServer_Implementation(int32 _inint)
{
	AFDEMOCharacter* myCharacter = GetWorld()->SpawnActor<AFDEMOCharacter>(AFDEMOCharacter::StaticClass(), GetActorTransform());
	myCharacter->AddScore(1);
}

void ATarget::HitEvent()
{
	if (!GetWorld()->IsServer()) HitServer(1);
}

bool ATarget::HitServer_Validate(int _num)
{
	if (_num >= 0) return true;
	return false;
}