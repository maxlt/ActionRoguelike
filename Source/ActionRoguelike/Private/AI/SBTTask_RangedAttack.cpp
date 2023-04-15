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

		UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
		AActor* TargetActor = Cast<AActor>(bb->GetValueAsObject("TargetActor"));
		const FVector AICharacterAimDir = TargetActor->GetActorLocation() - MuzzleLoc;
		const FRotator AimRot = AICharacterAimDir.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* SpawnedProjectile = nullptr;
		if (ensure(ProjectileClass))
		{
			SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLoc, AimRot, SpawnParams);
		}

		return IsValid(SpawnedProjectile) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
