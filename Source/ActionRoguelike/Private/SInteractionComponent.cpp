// Fill out your copyright notice in the Description page of Project Settings.

#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	// We want to do a line trace starting from the controller's "eye" and query whatever object collided with the line.
	//FHitResult Hit;
	FCollisionObjectQueryParams ObjTypeParams;
	ObjTypeParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	AActor* PawnOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	PawnOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * 10'000);
	//const bool bIsHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjTypeParams);

	const float SweepRadius = 30.f;

	FCollisionShape SweepingShape;
	SweepingShape.SetSphere(SweepRadius);

	TArray<FHitResult> HitResults;
	const bool bIsHit = GetWorld()->SweepMultiByObjectType(HitResults, EyeLocation, End, FQuat::Identity, ObjTypeParams, SweepingShape);

	const FColor LineColor = bIsHit ? FColor::Green : FColor::Red;

	for (const FHitResult Hit : HitResults)
	{
		// Once we've queried the object, we test whether it implements the SGameplayInterface, and if it does, we call
		// the Interact on it.
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				ISGameplayInterface::Execute_Interact(HitActor, GetOwner<APawn>());
			}
		}

		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, SweepRadius, 32, LineColor, false, 2.f);
		break;
	}

	DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0u, 2.0f);

}

