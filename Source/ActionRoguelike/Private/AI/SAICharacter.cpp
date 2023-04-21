// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensor->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* ThisController = GetController<AAIController>();
	if (ThisController)
	{
		UBlackboardComponent* bb = ThisController->GetBlackboardComponent();
		check(bb);
		bb->SetValueAsObject("TargetActor", Pawn);

		DrawDebugString(GetWorld(), Pawn->GetActorLocation(), "Player Spotted!", nullptr, FColor::White, 4.0f, true);
	}
}

// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensor");
}
