// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBased.h"
#include "SMagicProjectile.generated.h"

UCLASS(ABSTRACT)
class ACTIONROGUELIKE_API ASMagicProjectile : public ASProjectileBased
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageSize;

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	class UAudioComponent* SoundComp;

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	class USoundBase* ImpactSound;

protected:
	virtual void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

	virtual void Dissipate_Implementation() override;
};
