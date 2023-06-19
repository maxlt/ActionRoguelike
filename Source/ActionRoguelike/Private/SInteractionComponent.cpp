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
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// bAutoActivate is disabled by default; you can either manually activate it later or force auto-activation now.
	bAutoActivate = false;
	// Note:
	// By default, bAutoActivate is disabled and this component isn't active. Also, bCanEverTick is disabled by default.
	// Activating this component will enable ticking i.e. call SetComponentTickEnabled with true argument, provided bCanEverTick is set.
	// You can tick this component even while it's not active, i.e. Active Component => Ticking Component.
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
	DrawDebugString(GetWorld(), GetOwner()->GetActorLocation(), "Ticking...", nullptr, FColor::Red, 0.f, true);
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
				// break;
				DrawDebugSphere(GetWorld(), Hit.Location, SweepRadius, 32, LineColor, false, 5.f);
				DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Blue, false, 5.0f, 0);
				DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.f, FColor::Orange, false, 5.0f, 0);
				break;
			}
		}

	}

	DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0u, 2.0f);
}

