// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightsparkCharacter.h"
#include "PlayerCharacter.generated.h"


UCLASS(config = Game)
class LIGHTSPARK_API APlayerCharacter : public ALightsparkCharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
public:
	APlayerCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character", meta = (BlueprintProtected = "true"))
	void StartSprinting();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character", meta = (BlueprintProtected = "true"))
	void StopSprinting();

	void CheckSprintInput(float deltaTime);

	void Decelerate(float deltaTime, float* maxWalkSpeed);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION()
	virtual void EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:
	/**
	* Energy Needed For Rune (float)
	* How much energy is needed to fill a rune
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float energyNeededForRune;


	/**
	* Base Walk Speed (float)
	* How fast the character walks
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float baseWalkSpeed;

	/**
	* Sprint Speed Factor (float)
	* Speed increase (in percent) for sprint
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float sprintSpeedFactor;

	/**
	* Deceleration Factor (float)
	* How fast the character decelerates down to the base walk speed
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float decelerationFactor;

	/**
	* Sprint Duration (float)
	* For how long the character can sprint
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float sprintDuration;

	/**
	* Exhausted Duration (float)
	* For how long the character can't sprint after sprint energy is fully depleted
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float exhaustedDuration;

private:
	/**
	* Character Runes (int32)
	* How many runes the character currently has
	*/
	UPROPERTY(VisibleAnywhere, Category = "Energy")
	int32 characterRunes;

	/**
	* Max Sprint Speed (float)
	* The maximum sprint speed the character can achieve
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	float maxSprintSpeed;

	/**
	* Is Sprinting (bool)
	* Is the character currently sprinting
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	bool isSprinting;

	/**
	* Is Exhausted (bool)
	* Is the character currently exhausted
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	bool isExhausted;

	/**
	* Sprint Key Hold Time (float)
	* How long the sprint key has been held
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	float sprintKeyHoldTime;

	/**
	* Exhausted Time (float)
	* For how long the character has been exhausted
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	float exhaustedTime;

	/**
	* Max Sprint Energy (float)
	* Maximum sprint energy
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint|Energy")
	float maxSprintEnergy;

	/**
	* Sprint Energy (float)
	* How much sprint energy the character currently has
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint|Energy")
	float sprintEnergy;

	/**
	* Sprint Energy Consume (float)
	* Sprint energy consumption calculated from Sprint Duration
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint|Energy")
	float sprintEnergyConsume;

	FVector sprintStart, sprintEnd;
};
