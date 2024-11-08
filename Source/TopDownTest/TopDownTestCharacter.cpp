// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownTestCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h" // Include point light component

ATopDownTestCharacter::ATopDownTestCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 10.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create the spotlight for the lantern (directional light)
	LanternLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("LanternLight"));
	LanternLight->SetupAttachment(RootComponent);  // Attach to the root component (or attach to another component if needed)
	LanternLight->SetIntensity(50000.f);  // Set the intensity of the light
	LanternLight->SetLightColor(FLinearColor(1.f, 0.9f, 0.7f));  // Slightly warm light color
	LanternLight->SetInnerConeAngle(45.f);  // Set inner cone angle
	LanternLight->SetOuterConeAngle(60.f);  // Set outer cone angle for softer falloff
	LanternLight->SetRelativeLocation(FVector(0.f, 0.f, 500.f));  // Adjust the position relative to the character

	// Create the point light for surrounding light (ambient light)
	SurroundingLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("SurroundingLight"));
	SurroundingLight->SetupAttachment(RootComponent);  // Attach to the root component
	SurroundingLight->SetIntensity(10000.f);  // Set the intensity lower than the spotlight
	SurroundingLight->SetAttenuationRadius(2000.f);  // Set the radius of the light
	SurroundingLight->SetLightColor(FLinearColor(1.f, 0.95f, 0.85f));  // Slightly warm light color
	SurroundingLight->SetRelativeLocation(FVector(0.f, 0.f, 100.f));  // Adjust the position relative to the character

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATopDownTestCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Get the direction the character is moving in
	FVector MoveDirection = GetVelocity();
	MoveDirection.Z = 0; // Ignore vertical movement for top-down rotation

	if (!MoveDirection.IsNearlyZero())
	{
		// Find the desired rotation (facing the movement direction)
		FRotator TargetRotation = MoveDirection.Rotation();

		// Smoothly interpolate to the desired rotation
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, 1000.f); // Adjust interpolation speed (5.0f) as needed
		SetActorRotation(NewRotation);
	}
}
