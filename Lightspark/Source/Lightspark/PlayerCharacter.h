// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightsparkCharacter.h"
#include "PlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class EMovementState {
	Default,
	Moving,
	Sprint,
	SprintDash,
	StopSprint,
	Jump,
	Jumping,
	DoubleJump,
	JumpGlide
};


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

	UFUNCTION(BlueprintPure, Category = "Movement")
	EMovementState GetCurrentMovementState() { return CurrentMovementState; }

	void SetCurrentMovementState(EMovementState NewState) { CurrentMovementState = NewState; }

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	virtual void Jump() override;

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	virtual void StopJumping() override;

protected:
	UFUNCTION(BlueprintPure, Category = "Empowerment")
	bool GetSprintEmpowermentActive(int i) { return SprintEmpowermentActive[i]; }
	UFUNCTION(BlueprintPure, Category = "Empowerment")
	bool GetJumpEmpowermentActive(int i) { return JumpEmpowermentActive[i]; }

	void SetSprintEmpowermentActive(int i, bool active) { SprintEmpowermentActive[i] = active; }
	void SetJumpEmpowermentActive(int i, bool active) { JumpEmpowermentActive[i] = active; }


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

	void CheckMovementInput(float deltaTime);

	void EvaluateMovementState(float deltaTime);

	void Jump(float deltaTime);

	void Sprint(float deltaTime);

	//void Dash();

	void Decelerate(float deltaTime, float* maxWalkSpeed, float baseSpeed);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION()
	virtual void EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

private:
	UFUNCTION()
	void JumpApex();

	UFUNCTION()
	void JumpLanded(const FHitResult& Hit);

	void DisplayCurrentStates();

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
	* Jump Energy Consume (float)
	* Energy consumption per jump
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump", Meta = (BlueprintProtected = "true"))
	float jumpEnergyConsume;


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
	* Sprint Energy Consume (float)
	* Energy consumption per second
	*/
	UPROPERTY(VisibleAnywhere, Category = "Movement|Sprint|Energy")
	float sprintEnergyConsume;

	/**
	* Deceleration Factor (float)
	* How fast the character decelerates down to the base walk speed
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float decelerationFactor;

	/**
	* Dash Enabled Time (float)
	* For how long the dash ability is active after sprint button has been released
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float dashEnabledTime;

private:
	/**
	* Character Runes (int32)
	* How many runes the character currently has
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy", meta = (AllowPrivateAccess = "true"))
	int32 characterRunes;


	EMovementState CurrentMovementState;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Empowerment", meta = (AllowPrivateAccess = "true"))
	TArray<bool> SprintEmpowermentActive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Empowerment", meta = (AllowPrivateAccess = "true"))
	TArray<bool> JumpEmpowermentActive;


	/**
	* Is Jumping (bool)
	* Is the character currently jumping
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (AllowPrivateAccess = "true"))
	bool isJumping;

	/**
	* Jump Time (float)
	* How long the character has been in the air
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (AllowPrivateAccess = "true"))
	float jumpTime;


	/**
	* Max Sprint Speed (float)
	* The maximum sprint speed the character can achieve
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	float maxSprintSpeed;

	/**
	* Is Sprinting (bool)
	* Is the character currently sprinting
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	bool isSprinting;

	/**
	* Sprint Key Hold Time (float)
	* How long the sprint key has been held
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	float sprintKeyHoldTime;

	/**
	* Dash Enabled (bool)
	* Can the character dash
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	bool dashEnabled;

	/**
	* Is Dashing (bool)
	* Is the character currently dashing
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	bool isDashing;

	float* maxWalkSpeed;

	FTimerHandle DashTimerHandle, DisplayTimerHandle;

	FVector measureStart, measureEnd;
};
