// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	SpringArmComp->SetupAttachment(GetRootComponent());
	CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	CameraComp->SetupAttachment(SpringArmComp);

	// Set following flags to enable Dark Souls-style character control
	bUseControllerRotationYaw = false;
	SpringArmComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::MoveForward(float Value)
{
	// Make this character moves forward/reverses along the Controller's forward axis
	FRotator ControlRot = GetControlRotation();
	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	// Make this character turns to the left/right of the Controller's forward axis
	FRotator ControlRot = GetControlRotation();
	ControlRot.Roll = ControlRot.Pitch = 0.f;
	AddMovementInput(UKismetMathLibrary::GetRightVector(ControlRot), Value);
}

void ASCharacter::PrimaryAttack()
{
	FVector HandSocketLocation{};

	if (GetMesh())
	{
		HandSocketLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	}

	const FTransform SpawnTrMatx
	{
		GetControlRotation(),
		HandSocketLocation
	};

	FActorSpawnParameters SpawnParams{};
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn a magic projectile
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTrMatx, SpawnParams);
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Render a yellow arrow that indicates the direction this character is facing i.e. direction of character's x-axis
	// in world space
	const FVector LineStart = GetActorLocation() + GetActorRightVector() * 100.f;
	const FVector LineEnd = GetActorForwardVector() * 100.f + LineStart;
	DrawDebugDirectionalArrow(GetWorld(), LineStart, LineEnd, 100.f, FColor::Yellow, false, 0.f, 0u, 5.f);

	// Render an RGB axis-triplet that indicates the rotation of the Controller
	DrawDebugCoordinateSystem(GetWorld(), LineStart, GetControlRotation(), 50.f, false, 0.f, 0u, 1.f);
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
}

