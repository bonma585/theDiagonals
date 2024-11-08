// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TopDownTestCharacter.generated.h"

UCLASS(Blueprintable)
class ATopDownTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATopDownTestCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Spotlight for the lantern effect */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lantern Light", meta = (AllowPrivateAccess = "true"))
	class USpotLightComponent* LanternLight;

	/** Point light for surrounding light */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Surrounding Light", meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* SurroundingLight;
};
