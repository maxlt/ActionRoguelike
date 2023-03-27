// Fill out your copyright notice in the Description page of Project Settings.

#include "SMagicProjectile.h"
#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	DamageSize = 20.f;
	SphereComp->SetSphereRadius(20.f);
}

void ASMagicProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnProjectileOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

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
