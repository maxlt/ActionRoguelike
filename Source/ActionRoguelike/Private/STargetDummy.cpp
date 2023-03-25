// Fill out your copyright notice in the Description page of Project Settings.

#include "STargetDummy.h"
#include "SAttributeComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASTargetDummy::ASTargetDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(MeshComp);

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("Attribute");
}

void ASTargetDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASTargetDummy::OnHealthChanged);
}

void ASTargetDummy::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta)
{
	if (Delta < 0.f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}
