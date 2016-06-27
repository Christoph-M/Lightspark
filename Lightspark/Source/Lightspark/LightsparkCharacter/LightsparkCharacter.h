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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Returns current Character Energy */
	UFUNCTION(BlueprintPure, Category = "Energy")
	virtual float GetCurrentCharacterEnergy() const { return characterEnergy; }
	
protected:
	UFUNCTION()
	virtual void EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

public:
	FORCEINLINE class USphereComponent* GetInteractionSphere() const { return InteractionSphere; }

protected:
	/**
	* Initial Energy (float)
	* How much energy the character has at start
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float initialEnergy;

	/**
	* Max Energy (float)
	* Maximum amount of energy the character can have
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float maxEnergy;

	/**
	* Character Energy (float)
	* How much energy the character currently has
	*/
	UPROPERTY(VisibleAnywhere, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float characterEnergy;

	/*
	* Interaction Radius (float)
	* The radius the character can interact with other objects in
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (BlueprintProtected = "true"))
	float interactionRadius;
};