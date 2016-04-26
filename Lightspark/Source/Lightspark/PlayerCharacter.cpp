// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerCharacter.h"
#include "LightInteractable.h"


APlayerCharacter::APlayerCharacter() {
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Player"));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	initialEnergy = 50.0f;
	energyNeededForRune = 50.0f;
	characterEnergy = initialEnergy;
	characterRunes = characterEnergy / energyNeededForRune;

	baseWalkSpeed = 600.0f;
	maxSprintSpeed = 1200.0f;
	decelerationFactor = 1500.0f;
	sprintDuration = 4.0f;
	exhaustedDuration = 3.0f;
	maxSprintEnergy = 100.0f;
	sprintEnergy = maxSprintEnergy;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	isSprinting = false;
	isExhausted = false;
	JumpKeyHoldTime = 0.0f;
	exhaustedTime = 0.0f;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetInteractionSphere()->ComponentTags.Add(TEXT("Light"));
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	sprintEnergyConsume = maxSprintEnergy / sprintDuration;

	GetInteractionSphere()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::EvaluateLightInteraction);
}

void APlayerCharacter::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	CheckSprintInput(deltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprinting);
	InputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprinting);

	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &APlayerCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &APlayerCharacter::TouchStopped);
}


void APlayerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void APlayerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::StartSprinting() {
	isSprinting = true;
	UE_LOG(LogClass, Log, TEXT("Started sprinting"));
	sprintKeyHoldTime = 0.0f;
}

void APlayerCharacter::StopSprinting() {
	isSprinting = false;
	UE_LOG(LogClass, Log, TEXT("Stopped sprinting. Hold time: %f"), sprintKeyHoldTime);
	sprintKeyHoldTime = 0.0f;
}

void APlayerCharacter::CheckSprintInput(float deltaTime) {
	float* maxWalkSpeed = &GetCharacterMovement()->MaxWalkSpeed;

	const bool startedSprinting = sprintKeyHoldTime == 0.0f &&  isSprinting;
	const bool ranOutOfEnergy = sprintEnergy == 0.0f && isSprinting;
	const bool stoppedSprinting = (*maxWalkSpeed > baseWalkSpeed || sprintEnergy == 0.0f) && !isSprinting;

	if (!isExhausted) {
		if (isSprinting && sprintEnergy > 0.0f) {
			sprintKeyHoldTime += deltaTime;

			sprintEnergy = maxSprintEnergy - (sprintKeyHoldTime * sprintEnergyConsume);
			if (sprintEnergy < 0.0f) { sprintEnergy = 0.0f; isExhausted = true; }

			UE_LOG(LogClass, Log, TEXT("SprintEnergy: %f"), sprintEnergy);

			if (startedSprinting) *maxWalkSpeed = maxSprintSpeed;
		}
		else if (stoppedSprinting || ranOutOfEnergy) {
			Decelerate(deltaTime, maxWalkSpeed);

			if (*maxWalkSpeed == baseWalkSpeed && stoppedSprinting) sprintEnergy = maxSprintEnergy;
		}
	} else {
		exhaustedTime += deltaTime;

		if (*maxWalkSpeed > baseWalkSpeed) Decelerate(deltaTime, maxWalkSpeed);

		UE_LOG(LogClass, Log, TEXT("ExhaustedTime: %f"), exhaustedTime);

		if (exhaustedTime >= exhaustedDuration) { exhaustedTime = 0.0f; isExhausted = false; }
	}
}

void APlayerCharacter::Decelerate(float deltaTime, float* maxWalkSpeed) {
	*maxWalkSpeed = (*maxWalkSpeed < baseWalkSpeed) ? baseWalkSpeed : *maxWalkSpeed - (deltaTime * decelerationFactor);
}

void APlayerCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	Super::EvaluateLightInteraction(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ALightInteractable* const TestInteractable = Cast<ALightInteractable>(OtherActor);

	if (TestInteractable && !TestInteractable->IsPendingKill() && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Log, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		TestInteractable->ChangeState(EInteractionState::Lit);
	}
}