// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTTask_RangedAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (ensure(AIController))
	{
		ACharacter* AICharacter = AIController->GetPawn<ACharacter>();
		if (!AICharacter)
		{
			return EBTNodeResult::Failed;
		}

		const FVector MuzzleLoc = AICharacter->GetMesh()->GetSocketLocation("Muzzle_01");

		const UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
		const AActor* TargetActor = Cast<AActor>(bb->GetValueAsObject("TargetActor"));
		if (!TargetActor) // This character may not have a target yet until it detects the target.
		{
			return EBTNodeResult::Failed;
		}
		const FVector AimDir = TargetActor->GetActorLocation() - MuzzleLoc;
		const FRotator AimRot = AimDir.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const AActor* SpawnedProjectile = nullptr;
		if (ensure(ProjectileClass))
		{
			SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLoc, AimRot, SpawnParams);
		}

		return IsValid(SpawnedProjectile) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
