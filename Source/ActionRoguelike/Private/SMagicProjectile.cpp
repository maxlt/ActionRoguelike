// Fill out your copyright notice in the Description page of Project Settings.

#include "SMagicProjectile.h"
#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	SoundComp = CreateDefaultSubobject<UAudioComponent>("SoundComp");

	DamageSize = 20.f;
	SphereComp->SetSphereRadius(20.f);
}

void ASMagicProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnProjectileOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor && ensure(ImpactSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
	}

	if (OtherActor && OtherActor != GetInstigator<APawn>())
	{
		USAttributeComponent* AttrComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (AttrComp)
		{
			AttrComp->ApplyHealthChange(-DamageSize);
			Dissipate(); // Only dissipate when we hit something valid
		}
	}
}

void ASMagicProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ensure(IsValid(this)) && ensure(ImpactSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
	}

	Super::OnProjectileHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();

	SoundComp->Play();
}

void ASMagicProjectile::Dissipate_Implementation()
{
	Super::Dissipate_Implementation();
}
