// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractComp = CreateDefaultSubobject<USInteractionComponent>("Interaction Component");

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = ControlRot.Roll = 0.f;

	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = ControlRot.Roll = 0.f;

	AddMovementInput(FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y), Value);
}

void ASCharacter::PrimaryAttack()
{
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);

	PlayAnimMontage(AttackAnim);

	//GetWorldTimerManager()->ClearTimer(TimerHandle_PrimaryAttack);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	const FTransform SpawnTM {
		GetControlRotation(), // We spawn the magic projectile at the camera/view's direction
		HandLocation
	};

	FActorSpawnParameters SpawnParams{};
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}

void ASCharacter::PrimaryInteract()
{
	verify(InteractComp);
	InteractComp->PrimaryInteract();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	{
		FVector LineStart = GetActorLocation();
		LineStart += GetActorRightVector() * 100.f; // Offset to the right of the pawn

		FVector ActorDir_LineEnd = LineStart + GetActorForwardVector() * 100.f;
		DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDir_LineEnd, 100.f, FColor::Yellow, false, 0.f, 0u, 5.f);

		FVector ControllerDir_LineEnd = LineStart + GetControlRotation().Vector() * 100.f;
		DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDir_LineEnd, 100.f, FColor::Green, false, 0.f, 0u, 5.f);
	}

	{
		FVector LineStart = GetActorLocation();
		LineStart += GetActorRightVector() * -100.f;

		DrawDebugCamera(GetWorld(), LineStart, CameraComp->GetComponentRotation(), CameraComp->FieldOfView, 50.f, FColor::Blue, false, 0.f, 0u);

		const FVector SpringArmLineStart = SpringArmComp->GetComponentLocation();

		DrawDebugLine(GetWorld(), SpringArmLineStart, SpringArmLineStart + SpringArmComp->GetComponentRotation().Vector() * 100.f, FColor::Red, false, 0.f, 0u, 1.f);
	}
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
}

