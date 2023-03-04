// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SGameplayInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USGameplayInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Any object that implements this interface has the ability to be interacted by a pawn.
 */
class ACTIONROGUELIKE_API ISGameplayInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Call this function on the object that is being interacted with e.g. chest item, lever, etc.
	// The Instigator is the pawn that initiates the interaction.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(APawn* Instigator);
};
