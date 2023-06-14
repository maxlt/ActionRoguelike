// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ZPlayerCameraManager.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AZPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

protected:
	/** How long in seconds to blend the camera once crouching begins? */
	UPROPERTY(EditDefaultsOnly)
	float CrouchBlendDuration;

public:
	// Sets default values for this actor's properties
	AZPlayerCameraManager();

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	
private:
	/**
	 * Number of seconds passed since the camera begins blending. Value is always between 0 and CrouchBlendDuration.
	 */
	float CrouchBlendElaspedTime;
};
