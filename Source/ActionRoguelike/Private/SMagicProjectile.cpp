// Fill out your copyright notice in the Description page of Project Settings.

#include "SMagicProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("Sphere Component");
	SetRootComponent(SphereComp);

	FxComp = CreateDefaultSubobject<UParticleSystemComponent>("Particle FX Component");
	FxComp->SetupAttachment(SphereComp);

	ProjectileMovComp = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement Component");

	// Unexplained by Tom how these settings affect the magic projectiles
	ProjectileMovComp->InitialSpeed = 1000.f;
	ProjectileMovComp->bRotationFollowsVelocity = true;
	ProjectileMovComp->bInitialVelocityInLocalSpace = true;
}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

