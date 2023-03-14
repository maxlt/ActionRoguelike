// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BarrelMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Barrel Mesh");
	SetRootComponent(BarrelMeshComp);

	ExplosiveForceComp = CreateDefaultSubobject<URadialForceComponent>("Explosive Force");
	ExplosiveForceComp->SetupAttachment(BarrelMeshComp);

	BarrelMeshComp->SetSimulatePhysics(true);

	ExplosiveForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
	ExplosiveForceComp->SetAutoActivate(false);
	ExplosiveForceComp->ImpulseStrength = 1000.f;
	ExplosiveForceComp->bImpulseVelChange = true;
	ExplosiveForceComp->Radius = 750.f;
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BarrelMeshComp->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);
}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplosiveForceComp->FireImpulse();
}

