// Fill out your copyright notice in the Description page of Project Settings.

#include "SItemChest.h"

// Sets default values
ASItemChest::ASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	SetRootComponent(BaseMesh);

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lid Mesh"));
	LidMesh->SetupAttachment(BaseMesh);

	PitchTarget = 110.0;
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Log, TEXT("Interact called on an SItemChest object"));
	LidMesh->SetRelativeRotation(FRotator{PitchTarget, 0, 0});
}

// Called when the game starts or when spawned
void ASItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

