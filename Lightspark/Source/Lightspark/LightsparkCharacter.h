// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "LightsparkCharacter.generated.h"


UCLASS()
class ALightsparkCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* InteractionSphere;

public:
	ALightsparkCharacter();

	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
protected:
	UFUNCTION()
	virtual void EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

public:
	FORCEINLINE class USphereComponent* GetInteractionSphere() const { return InteractionSphere; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (BlueprintProtected = "true"))
	float interactionRadius;
};