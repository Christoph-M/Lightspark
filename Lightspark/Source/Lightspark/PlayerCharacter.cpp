// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerCharacter.h"
#include "LightsparkGameMode.h"
#include "LightInteractable.h"


APlayerCharacter::APlayerCharacter() {
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Player"));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	energyNeededForRune = 50.0f;

	GetCharacterMovement()->bNotifyApex = true;

	for (int i = 0; i < 4; ++i) {
		SprintEmpowermentActive.Add(false);
		JumpEmpowermentActive.Add(false);
	}

	jumpEnergyConsume = 5.0f;

	maxWalkSpeed = &GetCharacterMovement()->MaxWalkSpeed;
	baseWalkSpeed = 600.0f;
	sprintSpeedFactor = 50.0f;
	sprintEnergyConsume = 2.0f;
	decelerationFactor = 1500.0f;
	dashEnabledTime = 3.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	jumpTime = 0.0f;

	isSprinting = false;
	JumpKeyHoldTime = 0.0f;
	dashEnabled = false;
	isDashing = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
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

	characterRunes = characterEnergy / energyNeededForRune;

	this->SetCurrentMovementState(EMovementState::Default);

	GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
	maxSprintSpeed = ((sprintSpeedFactor / 100.0f) * baseWalkSpeed) + baseWalkSpeed;
	this->SetSprintEmpowermentActive(2, true);

	GetInteractionSphere()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::EvaluateLightInteraction);

	OnReachedJumpApex.AddDynamic(this, &APlayerCharacter::JumpApex);
	LandedDelegate.AddDynamic(this, &APlayerCharacter::JumpLanded);

	GetWorld()->GetTimerManager().SetTimer(DisplayTimerHandle, this, &APlayerCharacter::DisplayCurrentStates, 0.2f, true);
}

void APlayerCharacter::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	this->CheckMovementInput(deltaTime);
	this->EvaluateMovementState(deltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJumping);

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


void APlayerCharacter::Jump() {
	Super::Jump();

	isJumping = true;
	jumpTime = 0.0f;
}

void APlayerCharacter::StopJumping() {
	Super::StopJumping();
}

void APlayerCharacter::JumpApex() {
	measureEnd = GetActorLocation();
	float distance = FVector::Dist(measureStart, measureEnd);
	UE_LOG(LogClass, Log, TEXT("Jump Height: %f"), distance);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Jump Height: %f"), distance));
}

void APlayerCharacter::JumpLanded(const FHitResult& Hit) {
	isJumping = false;
	UE_LOG(LogClass, Log, TEXT("Landed"));
	GetCharacterMovement()->bNotifyApex = true;
	jumpTime = 0.0f;
}


void APlayerCharacter::StartSprinting() {
	/*if (GetSprintEmpowermentActive(2) && isSprinting) {
		dashEnabled = true;
		GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::Dash, dashEnabledTime);
		GetWorld()->GetTimerManager().PauseTimer(DashTimerHandle);
	}*/
	isSprinting = true;
	UE_LOG(LogClass, Log, TEXT("Started sprinting"));
	sprintKeyHoldTime = 0.0f;
}

void APlayerCharacter::StopSprinting() {
	isSprinting = false;
	//GetWorld()->GetTimerManager().UnPauseTimer(DashTimerHandle);
	UE_LOG(LogClass, Log, TEXT("Stopped sprinting. Hold time: %f"), sprintKeyHoldTime);
	sprintKeyHoldTime = 0.0f;
}


void APlayerCharacter::CheckMovementInput(float deltaTime) {
	if (GetVelocity().IsZero()) {
		this->SetCurrentMovementState(EMovementState::Default);
	} else if (isJumping && jumpTime == 0.0f) {
		this->SetCurrentMovementState(EMovementState::Jump);
	} else if (isJumping) {
		this->SetCurrentMovementState(EMovementState::Jumping);
	} else if (isSprinting) {
		this->SetCurrentMovementState(EMovementState::Sprint);
	} else if (*maxWalkSpeed > baseWalkSpeed) {
		this->SetCurrentMovementState(EMovementState::StopSprint);
	} else {
		this->SetCurrentMovementState(EMovementState::Moving);
	}
}

void APlayerCharacter::EvaluateMovementState(float deltaTime) {
	switch (this->GetCurrentMovementState()) {
		case EMovementState::Default: break;
		case EMovementState::DoubleJump: break;
		case EMovementState::Jump:
			this->Jump(deltaTime); break;
		case EMovementState::JumpGlide: break;
		case EMovementState::Jumping: break;
		case EMovementState::Moving: break;
		case EMovementState::Sprint:
			this->Sprint(deltaTime); break;
		case EMovementState::SprintDash: break;
		case EMovementState::StopSprint:
			this->Decelerate(deltaTime, maxWalkSpeed, baseWalkSpeed); break;
		default: break;
	}
}


void APlayerCharacter::Jump(float deltaTime) {
	if (characterEnergy > energyNeededForRune) characterEnergy -= jumpEnergyConsume;

	jumpTime += deltaTime;

	measureStart = GetActorLocation();
}

void APlayerCharacter::Sprint(float deltaTime) {
	if (characterEnergy > energyNeededForRune) characterEnergy -= sprintEnergyConsume * deltaTime;

	if (sprintKeyHoldTime == 0.0f && isSprinting) {
		*maxWalkSpeed = maxSprintSpeed;
		measureStart = GetActorLocation();
	}

	sprintKeyHoldTime += deltaTime;
}

//void APlayerCharacter::Dash() {
//	dashEnabled = false;
//	UE_LOG(LogClass, Log, TEXT("Dash disabled"));
//}

void APlayerCharacter::Decelerate(float deltaTime, float* maxWalkSpeed, float baseSpeed) {
	*maxWalkSpeed = *maxWalkSpeed - (deltaTime * decelerationFactor);

	if (*maxWalkSpeed < baseSpeed) {
		*maxWalkSpeed = baseSpeed;

		// Calculate sprint distance and print it to console and screen
		measureEnd = GetActorLocation();
		float distance = FVector::Dist(measureStart, measureEnd);
		UE_LOG(LogClass, Log, TEXT("Sprint Distance: %f"), distance);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Sprint Distance: %f"), distance));
	}
}

void APlayerCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	Super::EvaluateLightInteraction(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ALightInteractable* const TestInteractable = Cast<ALightInteractable>(OtherActor);

	if (TestInteractable && !TestInteractable->IsPendingKill() && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Log, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		TestInteractable->ChangeState(EInteractionState::Lit);
	}
}

void APlayerCharacter::DisplayCurrentStates() {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, FString::Printf(TEXT("Jump Height: %f"), characterEnergy));
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Orange, FString::Printf(TEXT("Movement State: %d"), static_cast<uint8>(this->GetCurrentMovementState())));
}