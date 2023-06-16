// Fill out your copyright notice in the Description page of Project Settings.

#include "ZCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#pragma region FSavedMove_ZCharacter

bool FSavedMove_ZCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_ZCharacter* NewSavedMove_ZCharacter = static_cast<FSavedMove_ZCharacter*>(NewMove.Get());
	if (NewSavedMove_ZCharacter->Saved_bWantsToSprint != Saved_bWantsToSprint)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_ZCharacter::Clear()
{
	Saved_bWantsToSprint = 0;
	FSavedMove_Character::Clear();
}

uint8 FSavedMove_ZCharacter::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();
	if (Saved_bWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}
	return Result;
}

void FSavedMove_ZCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
	
	const UZCharacterMovementComponent* const CharacterMovement = Character->GetCharacterMovement<UZCharacterMovementComponent>();
	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bPrevWantsToCrouch = CharacterMovement->Safe_bPrevWantsToCrouch;
}

void FSavedMove_ZCharacter::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);
	
	UZCharacterMovementComponent* const CharacterMovement = Character->GetCharacterMovement<UZCharacterMovementComponent>();
	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
}

#pragma endregion

#pragma region FNetPredictionData_ClientZCharacter

FNetPredictionData_Client_ZCharacter::FNetPredictionData_Client_ZCharacter(const UZCharacterMovementComponent& ZCharacterMovement)
	: FNetworkPredictionData_Client_Character(ZCharacterMovement)
{
}

FSavedMovePtr FNetPredictionData_Client_ZCharacter::AllocateNewMove()
{
	return MakeShared<FSavedMove_ZCharacter>();
}

#pragma endregion

#pragma region UZCharacterMovementComponent

namespace
{
	FORCEINLINE bool operator<(const FVector& V, float M)
	{
		return V.SquaredLength() < FMath::Square(M);
	}
	
	FORCEINLINE bool operator>(const FVector& V, float M)
	{
		return V.SquaredLength() > FMath::Square(M);
	}
}

// Sets default values for this component's properties
UZCharacterMovementComponent::UZCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true; // We want the character to be able to crouch.
}

// Called when the game starts
void UZCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UZCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		MaxWalkSpeed = Safe_bWantsToSprint ? Sprint_MaxWalkSpeed : Walk_MaxWalkSpeed;
	}

	Safe_bPrevWantsToCrouch = bWantsToCrouch;
}

void UZCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (MovementMode == MOVE_Walking && !bWantsToCrouch && Safe_bPrevWantsToCrouch)
	{
		FHitResult PotentialSlideSurface;
		if (Velocity > Slide_MinSpeed && GetSlideSurface(PotentialSlideSurface))
		{
			EnterSlide();
		}
	}

	if (IsCustomMovementMode(ECustomMovementMode::CMOVE_Slide) && !bWantsToCrouch)
	{
		ExitSlide();
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UZCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

bool UZCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UZCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

void UZCharacterMovementComponent::EnterSlide()
{
	bWantsToCrouch = true;
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	SetMovementMode(MOVE_Custom, CMOVE_Slide);
}

void UZCharacterMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;
	const FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(MOVE_Walking);
}

void UZCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// Useful when incorporating root motion into a movement.
	// However, sliding has no use for root motion, so this function was added for completeness.
	RestorePreAdditiveRootMotionVelocity();
	
	FHitResult PreMovementSurfaceHit;
	if (!GetSlideSurface(PreMovementSurfaceHit) || Velocity < Slide_MinSpeed)
	{
		ExitSlide();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	// Surface gravity. v += a * dt
	Velocity += Slide_GravityForce * FVector::DownVector * DeltaTime;

	// Strafe while sliding
	if (FMath::Abs(Acceleration.GetSafeNormal() | UpdatedComponent->GetRightVector()) > 0.5f)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	// Calculate velocity
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(DeltaTime, Slide_Friction,	true, GetMaxBrakingDeceleration());
	}

	ApplyRootMotionToVelocity(DeltaTime);

	// Perform move
	Iterations++;
	bJustTeleported = false;

	const FVector OldPosition = UpdatedComponent->GetComponentLocation();
	const FQuat OldRotation = UpdatedComponent->GetComponentQuat();

	const FVector IntendedPositionDelta = Velocity * DeltaTime;
	const FVector VelocityPlaneDirection = FVector::VectorPlaneProject(Velocity, PreMovementSurfaceHit.Normal).GetSafeNormal();
	const FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelocityPlaneDirection, PreMovementSurfaceHit.Normal).ToQuat();
	
	FHitResult Hit{};
	SafeMoveUpdatedComponent(IntendedPositionDelta, NewRotation, true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, DeltaTime, IntendedPositionDelta);
		SlideAlongSurface(IntendedPositionDelta, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	FHitResult PostMovementSurfaceHit;
	if (!GetSlideSurface(PostMovementSurfaceHit) || Velocity < Slide_MinSpeed)
	{
		ExitSlide();
	}

	// Update outgoing velocity
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldPosition) / DeltaTime;
	}
}

bool UZCharacterMovementComponent::GetSlideSurface(FHitResult& OutHit) const
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	const FName ProfileName = TEXT("BlockAll");

	// Setting up collision query parameters
	FCollisionQueryParams CollQueryParams;
	TArray<AActor*> CharacterChildren; // Line tracing should ignore the owner and its child actors.
	GetCharacterOwner()->GetAllChildActors(CharacterChildren);
	CollQueryParams.AddIgnoredActors(CharacterChildren);
	CollQueryParams.AddIgnoredActor(GetOwner());
	
	return GetWorld()->LineTraceSingleByProfile(OutHit, Start, End, ProfileName, CollQueryParams);
}

// Called every frame
void UZCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FNetworkPredictionData_Client* UZCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner);
	if (!ClientPredictionData)
	{
		// Force removing const-ness
		UZCharacterMovementComponent* MutableThis = const_cast<UZCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetPredictionData_Client_ZCharacter(*this);
		// Not sure if the followings are necessary
		// MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.0f;
		// MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.0f;
	}
	return ClientPredictionData;
}

void UZCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
}

void UZCharacterMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void UZCharacterMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

bool UZCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode QueryMovementMode) const
{
	return (MovementMode == MOVE_Custom) && (CustomMovementMode == QueryMovementMode);
}

#pragma endregion
