// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBased.h"
#include "SProjectileTeleport.generated.h"

/**
 * A projectile-based ability in which player shoots a projectile and teleports at wherever the projectile is.
 */
UCLASS(ABSTRACT)
class ACTIONROGUELIKE_API ASProjectileTeleport : public ASProjectileBased
{
	GENERATED_BODY()
	
public:
	ASProjectileTeleport();

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(EditAnywhere, Category="Projectile Ability")
	float TimeUntilExplode;

	UPROPERTY(EditAnywhere, Category="Projectile Ability")
	float TimeUntilTeleport;

	FTimerHandle DetonateDelayTimer;

	virtual void Dissipate_Implementation() override;
	
	void Teleport();
};
