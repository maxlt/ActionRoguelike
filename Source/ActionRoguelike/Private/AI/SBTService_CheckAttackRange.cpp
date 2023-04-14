// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	// During each tick, we check the distance between the AI pawn (e.g. Minion) and the target Actor (i.e. player).

	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	if (ensure(bb))
	{
		AActor* TargetActor = Cast<AActor>(bb->GetValueAsObject("TargetActor"));
		if (TargetActor) // No need to ensure because TargetActor may be null many times.
		{
			AAIController* AIController = OwnerComp.GetAIOwner();
			if (ensure(AIController))
			{
				APawn* AIPawn = AIController->GetPawn();
				if (ensure(AIPawn))
				{
					const double DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
					const bool bWithinRange = DistanceTo < 2000.0; // Is the AI pawn within range of player?

					bool bInSight = false;
					if (bWithinRange)
					{
						bInSight = AIController->LineOfSightTo(TargetActor); // Is the player inside AI pawn's line of sight?
					}

					// Make AI pawn follows player if any of the conditions checks:
					// (1) The AI pawn cannot see the player i.e. line of sight is blocked.
					// (2) The AI pawn is outside the player's range i.e. >= 2000 units.
					// If AttackRangeKey is TRUE, the AI pawn stops following the player.
					// Otherwise, it keeps following the player until the conditions above check.
					bb->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bInSight);
				}
			}
		}
	}
}
