// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileTeleport.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ASProjectileTeleport::ASProjectileTeleport()
{
	TimeUntilExplode = 0.2f;
	TimeUntilTeleport = 0.2f;

	MovementComp->InitialSpeed = 5000.f;
}

void ASProjectileTeleport::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(DetonateDelayTimer, this, &ASProjectileTeleport::Dissipate, TimeUntilExplode);
}

void ASProjectileTeleport::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileTeleport::OnProjectileHit);
}

void ASProjectileTeleport::Dissipate_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Explode"));

	GetWorldTimerManager().ClearTimer(DetonateDelayTimer);

	UGameplayStatics::SpawnEmitterAtLocation(this, HitFx, GetActorLocation(), GetActorRotation());
	FxComp->DeactivateSystem();
	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	FTimerHandle TeleportDelayTimer;
	GetWorldTimerManager().SetTimer(TeleportDelayTimer, this, &ASProjectileTeleport::Teleport, TimeUntilTeleport);
}

void ASProjectileTeleport::Teleport()
{
	APawn* InstigatorPawn = GetInstigator<APawn>();
	if (ensureAlways(InstigatorPawn))
	{
		FVector Destination = GetActorLocation();
		UE_LOG(LogTemp, Log, TEXT("Teleport at %s"), *Destination.ToString());

		InstigatorPawn->TeleportTo(Destination, InstigatorPawn->GetActorRotation());

		Destroy();
	}
}
