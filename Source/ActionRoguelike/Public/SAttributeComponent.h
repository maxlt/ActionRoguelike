// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Health;

	// TODO Attributes for max health, stamina, strength, etc.
public:	
	// Sets default values for this component's properties
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(float Delta);		
};
