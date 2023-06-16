// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None	UMETA(Hidden),
	CMOVE_Slide UMETA(DisplayName="Slide"),
	CMOVE_Max	UMETA(Hidden)
};

class ACTIONROGUELIKE_API FSavedMove_ZCharacter : public FSavedMove_Character
{
public:
	uint32 Saved_bWantsToSprint:1;
	uint32 Saved_bPrevWantsToCrouch:1;

	// Minimal API to override
	
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	/** Copy values of safe variables to corresponding saved variables. */
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
	/** Copy values of saved variables to safe variables. */
	virtual void PrepMoveFor(ACharacter* Character) override;
	
	// Minimal API ends
};

class UZCharacterMovementComponent;

class ACTIONROGUELIKE_API FNetPredictionData_Client_ZCharacter : public FNetworkPredictionData_Client_Character
{
public:
	FNetPredictionData_Client_ZCharacter(const UZCharacterMovementComponent& ZCharacterMovement);

	/** Allocate FSavedMove_ZCharacter object on heap. */
	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class ACTIONROGUELIKE_API UZCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_ZCharacter;

	bool Safe_bWantsToSprint;
	bool Safe_bPrevWantsToCrouch;

protected:
#pragma region Sprint
	UPROPERTY(EditDefaultsOnly)
	float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly)
	float Walk_MaxWalkSpeed;
#pragma endregion

#pragma region Slide
	UPROPERTY(EditDefaultsOnly)
	float Slide_MinSpeed;
	UPROPERTY(EditDefaultsOnly)
	float Slide_EnterImpulse;
	UPROPERTY(EditDefaultsOnly)
	float Slide_GravityForce;
	UPROPERTY(EditDefaultsOnly)
	float Slide_Friction;
#pragma endregion 
	
public:
	// Sets default values for this component's properties
	UZCharacterMovementComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;

	// Sprint movement
	UFUNCTION(BlueprintCallable)
	void SprintPressed();
	UFUNCTION(BlueprintCallable)
	void SprintReleased();

	UFUNCTION(BlueprintPure)
	bool IsCustomMovementMode(ECustomMovementMode QueryMovementMode) const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override; // Called by StartNewPhysics.
	
	// Slide interface
	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float DeltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& OutHit) const;
};
