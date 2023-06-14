// Fill out your copyright notice in the Description page of Project Settings.

#include "ZPlayerCameraManager.h"
#include "SCharacter.h"
#include "ZCharacterMovementComponent.h"

// Sets default values
AZPlayerCameraManager::AZPlayerCameraManager()
	: CrouchBlendDuration{0.1f}
{
}

void AZPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (const ASCharacter* const OwningCharacter = GetOwningPlayerController()->GetPawn<ASCharacter>())
	{
		const UZCharacterMovementComponent* const ZCMC = OwningCharacter->GetCharacterMovement<UZCharacterMovementComponent>();
		// TargetCrouchOffset is a displacement with a negative z-component.
		const FVector TargetCrouchOffset = FVector(0.f, 0.f, ZCMC->GetCrouchedHalfHeight() - OwningCharacter->GetDefaultHalfHeight());
		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendElaspedTime / CrouchBlendDuration, 0.f, 1.f));

		if (ZCMC->IsCrouching())
		{
			CrouchBlendElaspedTime = FMath::Clamp(CrouchBlendElaspedTime + DeltaTime, 0.f, CrouchBlendDuration);
			// Equivalent to Offset = Offset - TargetCrouchOffset. This means that TargetCrouchOffset is also a displacement.
			Offset -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendElaspedTime = FMath::Clamp(CrouchBlendElaspedTime - DeltaTime, 0.f, CrouchBlendDuration);
		}

		if (ZCMC->IsMovingOnGround())
		{
			// Since Location is a point, then Offset is a displacement (vector).
			OutVT.POV.Location += Offset;
		}
	}
}
