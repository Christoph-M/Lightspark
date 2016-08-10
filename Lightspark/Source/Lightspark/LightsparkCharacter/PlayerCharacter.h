// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightsparkCharacter.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerSneakDelegate, bool, isSneaking);


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
	JumpGlide,
	LightFlash,
	Sneak
};

UENUM(BlueprintType)
enum class ELightUpdateState {
	NoUpdate,
	UpdateStart,
	UpdateDecFadeIn,
	UpdateDecFadeOut,
	UpdateIncFadeIn,
	UpdateIncFadeOut,
	UpdateEnd
};

UENUM(BlueprintType)
enum ESprintEmpowerments {
	SEmp_FasterSprint,
	SEmp_SprintReducedCost,
	SEmp_Dash,
	SEmp_Thrust
};

UENUM(BlueprintType)
enum EJumpEmpowerments {
	JEmp_DoubleJump,
	JEmp_HigherJump,
	JEmp_JumpReducedCost,
	JEmp_Glide
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lightspark", meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* LifeLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lightspark", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* audioSprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lightspark", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* audioRecharge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lightspark", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* audioAmbient;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lightspark", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* audioMusic;

	UCharacterMovementComponent* CharacterMovement;
	
public:
	APlayerCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float deltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE class UPointLightComponent* GetLifeLight() const { return LifeLight; }

	float GetCurrentMaxEnergy() { return currentMaxEnergy; }
	void MyTakeDamage(float);

	int32 GetCurrentSegment() { return curSegment; }

	UFUNCTION(BlueprintCallable, Category = "Segment")
	void SetCurrentSegment(int32 segment);

	int32 GetLightFlashUses() { return lightFlashUses; }

	UFUNCTION(BlueprintCallable, Category = "Light")
	void SetLightFlashUses(int32 charges) { lightFlashUses += charges; if (lightFlashUses > maxLightFlashUses) lightFlashUses = maxLightFlashUses; }

	int32 GetMaxLightFlashUses() { return maxLightFlashUses; }

	UFUNCTION(BlueprintCallable, Category = "Light")
	void SetMaxLightFlashUses(int32 maxCharges) { maxLightFlashUses = maxCharges; if (lightFlashUses > maxLightFlashUses) lightFlashUses = maxLightFlashUses; }

	UFUNCTION(BlueprintCallable, Category = "Light")
	void Kill() { characterEnergy = -0.1f; }

	UFUNCTION(BlueprintPure, Category = "Movement")
	EMovementState GetCurrentMovementState() { return CurrentMovementState; }

	UFUNCTION(BlueprintPure, Category = "Light")
	ELightUpdateState GetCurrentLightUpdateState() { return CurrentLightUpdateState; }

	void SetCurrentMovementState(EMovementState NewState) { CurrentMovementState = NewState; }

	UFUNCTION(BlueprintPure, Category = "Empowerment")
	bool GetSprintEmpowermentActive(int i) { return SprintEmpowermentActive[i]; }
	UFUNCTION(BlueprintPure, Category = "Empowerment")
	bool GetJumpEmpowermentActive(int i) { return JumpEmpowermentActive[i]; }

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	virtual void Jump() override;

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	virtual void StopJumping() override;

	UPROPERTY(BlueprintAssignable)
	FPlayerSneakDelegate OnSneakToggle;

protected:
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

	void Interact();
	void SpendEnergy();
	void ConsumeEnergy();
	void Merge();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character", meta = (BlueprintProtected = "true"))
	void StartSprinting();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character", meta = (BlueprintProtected = "true"))
	void StopSprinting();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character", meta = (BlueprintProtected = "true"))
	void StartSneak();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character", meta = (BlueprintProtected = "true"))
	void StopSneak();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character", meta = (BlueprintProtected = "true"))
	void StartLightFlash();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pawn|Character", meta = (BlueprintProtected = "true"))
	void LightFlash();

	void CheckMovementInput(float deltaTime);

	void EvaluateMovementState(float deltaTime);

	void Jump(float deltaTime);

	void Jumping(float deltaTime);

	void DoubleJump(float deltaTime);

	void Glide(float deltaTime);

	void Sprint(float deltaTime);

	void Dash();

	void DisableDash();

	void Decelerate(float deltaTime, float* maxWalkSpeed, float baseSpeed);

	void LightFlash(float deltaTime);

	void UseEnergy(float amount);

	void PlayLoopSound(class USoundWave*);

	void StopLoopSound();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION()
	virtual void EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
	void CheckInLight(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void CheckInShadow(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void TeleportToCheckpoint0();

	UFUNCTION()
		void TeleportToCheckpoint1();

	UFUNCTION()
		void TeleportToCheckpoint2();

	UFUNCTION()
		void TeleportToCheckpoint3();

	UFUNCTION()
		void TeleportToCheckpoint4();

	UFUNCTION()
		void TeleportToCheckpoint5();

	UFUNCTION()
		void TeleportToCheckpoint6();

	UFUNCTION()
		void TeleportToCheckpoint7();

	UFUNCTION()
		void TeleportToCheckpoint8();

	UFUNCTION()
		void TeleportToCheckpoint9();

	UFUNCTION()
	void ToggleEnergyLoss();

private:
	UFUNCTION()
	void JumpApex();

	UFUNCTION()
	void JumpLanded(const FHitResult& Hit);

	void InitLight();

	void ActivateLightUpdate();

	void UpdateLight(float deltaTime);

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float energyNeededForRune;

	/**
	* Light Energy Gain (float)
	* Energy gained per second while the character is in range of a light source
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float lightEnergyGain;

	/**
	* Hit Energy Damage (flaot)
	* Energy damage taken on enemy contact with life light
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float hitEnergyDamage;
	
	/**
	* Shadow Energy Damage (float)
	* Damage taken per second while the character is not in range of a light source
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float shadowEnergyDamage;

	/**
	* Spend Energy Consume (float)
	* How much energy is taken from character when spending to a flower.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float spendEnergyConsume;

	/**
	* Consume Energy Gain (float)
	* How much energy is gained when consuming from a flower.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Energy", Meta = (BlueprintProtected = "true"))
	float consumeEnergyGain;


	/**
	* Min Light Range (float)
	* Minimum Life Light range
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float minLightRange;

	/**
	* Max Light Range (float)
	* Maximum Life Light range
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float maxLightRange;

	/**
	* Min Light Temp (float)
	* Minimum Life Light temperature
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float minLightTemp;

	/**
	* Max Light Temp (float)
	* Maximum Life Light temperature
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float maxLightTemp;

	/**
	* Min Light Intensity (float)
	* Minimum Life Light Intensity
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float minLightIntensity;

	/**
	* Max Light Intensity (float)
	* Maximum Life Light Intensity
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float maxLightIntensity;

	/**
	* Interaction Radius Factor (float)
	* Factor that determines the interaction radius based on Light Attenuation Radius
	* (<1.0 = smaller than attenuation radius; >1.0 bigger than attenuation radius)
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true", ClampMin = "0.0"))
	float interactionRadiusFac;

	/**
	* Life Light Decrement Time (float)
	* The time (in seconds) it takes the life light to decrease to current character energy
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float lifeLightDecTime;

	/**
	* Light Color Offset Factor (float)
	* Color fade amount (in percent) when character energy changes
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float lightColorOffsetFac;

	/**
	* Light Color Fade Time (float)
	* The time (in seconds) it takes to fade the light color to lightColorOffset
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float lightColorFadeTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	UCurveFloat* FlickerCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	UCurveFloat* IntenstiyOffsetCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	UCurveFloat* LightFlashFadeCurve;


	/**
	* Jump Energy Consume (float)
	* Energy consumption per jump
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Jump", Meta = (BlueprintProtected = "true"))
	float jumpEnergyConsume;

	/**
	* Double Jump Energy Consume (float)
	* Energy consumption on double jump
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Jump", Meta = (BlueprintProtected = "true"))
	float doubleJumpEnergyConsume;

	/**
	* Jump Fall Gravity (float)
	* Gravity change when character reaches jump apex
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Jump", Meta = (BlueprintProtected = "true"))
	float jumpFallGravity;

	/**
	* Glide Factor (float)
	* How fast the character falls down when gliding
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Jump", Meta = (BlueprintProtected = "true"))
	float glideSpeed;

	/**
	* Added Jump Height (float)
	* Jump height to be added to JumpZVelocity when Empowerment is active
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Jump", Meta = (BlueprintProtected = "true"))
	float addedJumpHeight;


	/**
	* Base Walk Speed (float)
	* How fast the character walks
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float baseWalkSpeed;

	/**
	* Sprint Speed Factor (float)
	* Speed increase (in percent) for sprint
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float sprintSpeedFactor;

	/**
	* Faster Sprint Speed Factor (float)
	* Speed increase (in percent) for faster sprint
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float fasterSprintSpeedFactor;

	/**
	* Sprint Energy Consume (float)
	* Energy consumption per second
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float sprintEnergyConsume;

	/**
	* Deceleration Factor (float)
	* How fast the character decelerates down to the base walk speed
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float decelerationFactor;

	/**
	* Dash Speed (float)
	* How fast the character dashes forward
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float dashSpeed;

	/**
	* Dash Enabled Time (float)
	* For how long the dash ability is active after sprint button has been released
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", Meta = (BlueprintProtected = "true"))
	float dashEnabledTime;

	/**
	* Sneak Light Range (float)
	* Range offset applied to the life light when sneak ability is used
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Sneak", Meta = (BlueprintProtected = "true"))
	float sneakLightRangeOffset;

	/**
	* Light Flash Range (float)
	* Range of the life light when flash of light ability is used
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	float lightFlashRange;

	/**
	* Max Light Flash Uses (int32)
	* How many light flash uses the player can have
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	int32 maxLightFlashUses;

	/**
	* Light Flash Uses (int32)
	* How many light flash uses the player has left
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
	int32 lightFlashUses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!AAAaaaAAAAAAAAAaaaAAAaaAAAAAAaaaAAAAAAAAAAAAAaaaaa", Meta = (BlueprintProtected = "true"))
	TArray<class AActor*> Checkpoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", Meta = (BlueprintProtected = "true"))
	class USoundWave* soundDeath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", Meta = (BlueprintProtected = "true"))
	class USoundWave* soundLightFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", Meta = (BlueprintProtected = "true"))
	TArray<class USoundWave*> soundsJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", Meta = (BlueprintProtected = "true"))
	class USoundWave* soundRecharge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", Meta = (BlueprintProtected = "true"))
	class USoundWave* soundSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", Meta = (BlueprintProtected = "true"))
	class USoundWave* soundSneak;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", Meta = (BlueprintProtected = "true"))
	TArray<class USoundWave*> soundsAmbient;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", Meta = (BlueprintProtected = "true"))
	TArray<class USoundWave*> soundsMusic;

private:
	/**
	* Current Max Energy (float)
	* The current maximum character energy
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy", meta = (AllowPrivateAccess = "true"))
	float currentMaxEnergy;

	/**
	* Character Runes (int32)
	* How many runes the character currently has
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy", meta = (AllowPrivateAccess = "true"))
	int32 characterRunes;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Segment", meta = (AllowPrivateAccess = "true"))
	int32 curSegment;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float lightRangeFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float lightTempFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float lightIntensityFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float lightEnergy;

	/**
	* Light Color Offset (float)
	* Color fade amount when character energy changes
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float lightColorOffset;

	float lightUpdateTime;


	EMovementState CurrentMovementState;

	ELightUpdateState CurrentLightUpdateState;

	/**
	* Sprint Empowerment Active (TArray<bool>)
	* Which sprint empowerments are enabled
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Empowerment", meta = (AllowPrivateAccess = "true"))
	TArray<bool> SprintEmpowermentActive;
	/**
	* Jump Empowerment Active (TArray<bool>)
	* Which jump empowerments are enabled
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Empowerment", meta = (AllowPrivateAccess = "true"))
	TArray<bool> JumpEmpowermentActive;


	/**
	* Is Jumping (bool)
	* Is the character currently jumping
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (AllowPrivateAccess = "true"))
	bool isJumping;

	/**
	* Is Falling (bool)
	* Is the character currently falling
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (AllowPrivateAccess = "true"))
	bool isFalling;

	/**
	* Can Double Jump (bool)
	* Can the character currently perform a double jump
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (AllowPrivateAccess = "true"))
	bool canDoubleJump;

	/**
	* Double Jumped (bool)
	* Has the character made a duble jump
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (AllowPrivateAccess = "true"))
	bool doubleJumped;

	/**
	* Is Gliding (float)
	* Is the character currently gliding
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Jump", meta = (AllowPrivateAccess = "true"))
	bool isGliding;

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
	* Can Dash (bool)
	* Can the character currently perform a dash
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	bool canDash;

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


	bool isSneaking;

	float sneakEnergy;

	
	float sneakOffset;


	float lightFlashTime;

	float lightFlashFadeInTime;

	float lightFlashFadeOutTime;

	float initialAttenuationRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool lightFlashActive;


	float* maxWalkSpeed;

	float lightColorFade, initialLightEnergy;

	FTimerHandle DashTimerHandle, DisplayTimerHandle;

	FVector measureStart, measureEnd;

	bool isInteracting, canSpend, canConsume, isInShadow, segmentLit;

	class AActor* interactedActor;
};
