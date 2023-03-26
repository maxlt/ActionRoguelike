// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileTeleport.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ASProjectileTeleport::ASProjectileTeleport()
{
	TeleportDestination = FVector::ZeroVector;
	TimeUntilExplode = 0.2f;
	TimeUntilTeleport = 0.2f;

	MovementComp->InitialSpeed = 5000.f;
}

void ASProjectileTeleport::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(ExplodeTimer, this, &ASProjectileTeleport::Explode, TimeUntilExplode);
}

void ASProjectileTeleport::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileTeleport::OnProjectileHit);
}

void ASProjectileTeleport::Explode()
{
	UE_LOG(LogTemp, Log, TEXT("Explode"));

	TeleportDestination = GetActorLocation();
	OnExploded();

	GetWorldTimerManager().SetTimer(TeleportTimer, this, &ASProjectileTeleport::Teleport, TimeUntilTeleport);
	MovementComp->StopMovementImmediately();
}

void ASProjectileTeleport::Teleport()
{
	APawn* InstigatorPawn = GetInstigator<APawn>();
	ensureAlways(InstigatorPawn);

	UE_LOG(LogTemp, Log, TEXT("Teleport at %s"), *TeleportDestination.ToString());

	ensureAlways(!TeleportDestination.IsZero());
	InstigatorPawn->TeleportTo(TeleportDestination, InstigatorPawn->GetActorRotation());
	Destroy();
}

void ASProjectileTeleport::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GetWorldTimerManager().ClearTimer(ExplodeTimer);
	Explode();
}
