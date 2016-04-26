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

	float GetCurrentEnergy() { return characterEnergy; }

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float initialEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float energyNeededForRune;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float baseWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float maxSprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float decelerationFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float sprintDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float exhaustedDuration;

private:
	UPROPERTY(VisibleAnywhere, Category = "Energy")
	float characterEnergy;

	UPROPERTY(VisibleAnywhere, Category = "Energy")
	int32 characterRunes;


	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	bool isSprinting;

	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	bool isExhausted;

	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	float sprintKeyHoldTime;

	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint")
	float exhaustedTime;

	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint|Energy")
	float maxSprintEnergy;

	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint|Energy")
	float sprintEnergy;

	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint|Energy")
	float sprintEnergyConsume;
};
