// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileBased.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASProjectileBased::ASProjectileBased()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("Sphere Component");
	SetRootComponent(SphereComp);

	FxComp = CreateDefaultSubobject<UParticleSystemComponent>("FX Component");
	FxComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("Movement Component");
	MovementComp->InitialSpeed = 1000.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0.f;

	SphereComp->SetCollisionProfileName("Projectile");
}

void ASProjectileBased::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Default implementation is do nothing; let derived classes decide it.
}

void ASProjectileBased::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Dissipate();
}

void ASProjectileBased::Dissipate_Implementation()
{
	if (ensure(IsValid(this)))
	{
		MovementComp->StopMovementImmediately();
		FxComp->DeactivateSystem();
		UGameplayStatics::SpawnEmitterAtLocation(this, HitFx, GetActorLocation(), GetActorRotation());
		SetActorEnableCollision(false);
		Destroy();
	}
}

void ASProjectileBased::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASProjectileBased::OnProjectileOverlap);
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBased::OnProjectileHit);
}
