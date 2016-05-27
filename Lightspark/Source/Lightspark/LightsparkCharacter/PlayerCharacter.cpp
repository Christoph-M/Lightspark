// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerCharacter.h"
#include "LightsparkGameMode.h"
#include "LightInteractable/LightInteractable.h"
#include "LightInteractable/EnvLightInteractable/EnvLightInteractable.h"
#include "LightInteractable/PlayerLightInteractable/PlayerLightInteractableFlower.h"
#include "LightsparkCharacter/AI/EnemyAiCharacter.h"
#include "LightsparkCharacter/AI/FriendlyAiCharacter.h"


APlayerCharacter::APlayerCharacter() {
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Player"));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	
	currentMaxEnergy = 100.0f;
	energyNeededForRune = 50.0f;
	spendEnergyConsume = 25.0f;
	consumeEnergyGain = 50.0f;

	GetCharacterMovement()->bNotifyApex = true;

	for (int i = 0; i < 4; ++i) {
		SprintEmpowermentActive.Add(false);
		JumpEmpowermentActive.Add(false);
	}

	jumpEnergyConsume = 5.0f;
	doubleJumpEnergyConsume = 5.0f;
	jumpFallGravity = 2.0f;
	glideSpeed = 250.0f;
	addedJumpHeight = 100.0f;

	maxWalkSpeed = &GetCharacterMovement()->MaxWalkSpeed;
	baseWalkSpeed = 600.0f;
	sprintSpeedFactor = 50.0f;
	fasterSprintSpeedFactor = 100.0f;
	sprintEnergyConsume = 2.0f;
	decelerationFactor = 1500.0f;
	dashEnabledTime = 3.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	jumpTime = 0.0f;

	isJumping = false;
	isFalling = false;
	canDoubleJump = false;
	doubleJumped = false;
	isGliding = false;
	isSprinting = false;
	JumpKeyHoldTime = 0.0f;
	dashEnabled = false;
	isDashing = false;

	minLightRange = 600.0f;
	maxLightRange = 2000.0f;
	minLightTemp = 1000.0f;
	maxLightTemp = 12000.0f;

	isInteracting = false;
	canSpend = false;
	canConsume = false;
	
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

	GetMesh()->SetCastShadow(false);

	LifeLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LifeLight"));
	LifeLight->AttachTo(RootComponent);
	LifeLight->bUseInverseSquaredFalloff = false;
	LifeLight->Intensity = 50.0f;
	LifeLight->bUseTemperature = true;

}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	CharacterMovement = GetCharacterMovement();

	if (currentMaxEnergy > maxEnergy) currentMaxEnergy = maxEnergy;
	if (characterEnergy > currentMaxEnergy) characterEnergy = currentMaxEnergy;

	characterRunes = characterEnergy / energyNeededForRune;

	this->SetCurrentMovementState(EMovementState::Default);

	CharacterMovement->MaxWalkSpeed = baseWalkSpeed;
	maxSprintSpeed = ((sprintSpeedFactor / 100.0f) * baseWalkSpeed) + baseWalkSpeed;

	baseJumpHeight = CharacterMovement->JumpZVelocity;

	if (!GetInteractionSphere()->OnComponentBeginOverlap.IsAlreadyBound(this, &APlayerCharacter::EvaluateLightInteraction)) {
		GetInteractionSphere()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::EvaluateLightInteraction);
	}

	lightRangeFactor = (maxLightRange - minLightRange) / maxEnergy;

	this->UpdateLight();
	
	if (!OnReachedJumpApex.IsAlreadyBound(this, &APlayerCharacter::JumpApex)) {
		OnReachedJumpApex.AddDynamic(this, &APlayerCharacter::JumpApex);
	}
	if (!LandedDelegate.IsAlreadyBound(this, &APlayerCharacter::JumpLanded)) {
		LandedDelegate.AddDynamic(this, &APlayerCharacter::JumpLanded);
	}

	if (!GetWorld()->GetTimerManager().TimerExists(DisplayTimerHandle)) {
		GetWorld()->GetTimerManager().SetTimer(DisplayTimerHandle, this, &APlayerCharacter::DisplayCurrentStates, 0.2f, true);
	}
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (GetInteractionSphere()->OnComponentBeginOverlap.IsAlreadyBound(this, &APlayerCharacter::EvaluateLightInteraction)) {
		GetInteractionSphere()->OnComponentBeginOverlap.RemoveDynamic(this, &APlayerCharacter::EvaluateLightInteraction);
	}

	if (OnReachedJumpApex.IsAlreadyBound(this, &APlayerCharacter::JumpApex)) {
		OnReachedJumpApex.RemoveDynamic(this, &APlayerCharacter::JumpApex);
	}
	if (LandedDelegate.IsAlreadyBound(this, &APlayerCharacter::JumpLanded)) {
		LandedDelegate.RemoveDynamic(this, &APlayerCharacter::JumpLanded);
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Super::EndPlay(EndPlayReason);
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

	InputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
	InputComponent->BindAction("Spend", IE_Pressed, this, &APlayerCharacter::SpendEnergy);
	InputComponent->BindAction("Consume", IE_Pressed, this, &APlayerCharacter::ConsumeEnergy);

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

	InputComponent->BindAction("JumpPower", IE_Pressed, this, &APlayerCharacter::ChangeJumpHeight);
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


void APlayerCharacter::Interact() {
	if (!isInteracting) {
		TArray<AActor*> CollectedActors;
		GetInteractionSphere()->GetOverlappingActors(CollectedActors);

		canSpend   = false;
		canConsume = false;

		for (int i = 0; i < CollectedActors.Num(); ++i) {
			APlayerLightInteractableFlower* const TestFlower = Cast<APlayerLightInteractableFlower>(CollectedActors[i]);

			if (TestFlower && !TestFlower->IsPendingKill() && TestFlower->GetCurrentState() == EInteractionState::Default) {
				interactedActor = TestFlower;

				switch (TestFlower->GetInteractionType()) {
					case EInteractionType::SpendConsume:	canSpend = true;  canConsume = true;  break;
					case EInteractionType::SpendOnly:		canSpend = true;  canConsume = false; break;
					case EInteractionType::ConsumeOnly:		canSpend = false; canConsume = true;  break;
					case EInteractionType::NoInteraction:	canSpend = false; canConsume = false; break;
				}

				isInteracting = true;
				CharacterMovement->DisableMovement();

				return;
			}


			AFriendlyAiCharacter* const TestCharacter = Cast<AFriendlyAiCharacter>(CollectedActors[i]);

			if (TestCharacter && !TestCharacter->IsPendingKill()) {
				interactedActor = TestCharacter;
				
				this->Merge();

				return;
			}
		}
	} else {
		isInteracting = false;
		CharacterMovement->MovementMode = EMovementMode::MOVE_Walking;
	}
}

void APlayerCharacter::SpendEnergy() {
	if (isInteracting && canSpend && interactedActor && !interactedActor->IsPendingKill()) {
		APlayerLightInteractableFlower* const Flower = Cast<APlayerLightInteractableFlower>(interactedActor);
		interactedActor = nullptr;

		Flower->ChangeState(EInteractionState::Lit);

		this->UseEnergy(spendEnergyConsume);

		isInteracting = false;
		CharacterMovement->MovementMode = EMovementMode::MOVE_Walking;
	}
}

void APlayerCharacter::ConsumeEnergy() {
	if (isInteracting && canConsume && interactedActor && !interactedActor->IsPendingKill()) {
		APlayerLightInteractableFlower* const Flower = Cast<APlayerLightInteractableFlower>(interactedActor);
		interactedActor = nullptr;

		Flower->ChangeState(EInteractionState::Destroyed);

		characterEnergy += consumeEnergyGain;
		this->UpdateLight();

		isInteracting = false;
		CharacterMovement->MovementMode = EMovementMode::MOVE_Walking;
	}
}

void APlayerCharacter::Merge() {
	if (interactedActor && !interactedActor->IsPendingKill()) {
		AFriendlyAiCharacter* const Friendly = Cast<AFriendlyAiCharacter>(interactedActor);
		interactedActor = nullptr;

		switch (Friendly->GetSprintEmpowerment()) {
			case ESprintEmpowerment::FasterSprint:
				SetSprintEmpowermentActive(SEmp_FasterSprint, true);
				maxSprintSpeed = ((fasterSprintSpeedFactor / 100.0f) * baseWalkSpeed) + baseWalkSpeed;
				UE_LOG(LogClass, Log, TEXT("Faster sprint activated.")); break;
			case ESprintEmpowerment::SprintReducedCost:
				SetSprintEmpowermentActive(SEmp_SprintReducedCost, true);
				UE_LOG(LogClass, Log, TEXT("Reduced sprint cost activated.")); break;
			case ESprintEmpowerment::Dash:
				SetSprintEmpowermentActive(SEmp_Dash, true);
				UE_LOG(LogClass, Log, TEXT("Dash activated.")); break;
			case ESprintEmpowerment::Thrust:
				SetSprintEmpowermentActive(SEmp_Thrust, true);
				UE_LOG(LogClass, Log, TEXT("Thrust activated.")); break;
		}

		switch (Friendly->GetJumpEmpowerment()) {
			case EJumpEmpowerment::DoubleJump:
				SetJumpEmpowermentActive(JEmp_DoubleJump, true);
				UE_LOG(LogClass, Log, TEXT("Double jump activated.")); break;
			case EJumpEmpowerment::HigherJump:
				SetJumpEmpowermentActive(JEmp_HigherJump, true);
				CharacterMovement->JumpZVelocity += addedJumpHeight;
				UE_LOG(LogClass, Log, TEXT("Higher jump activated.")); break;
			case EJumpEmpowerment::JumpReducedCost:
				SetJumpEmpowermentActive(JEmp_JumpReducedCost, true);
				UE_LOG(LogClass, Log, TEXT("Reduced jump cost activated.")); break;
			case EJumpEmpowerment::Glide:
				SetJumpEmpowermentActive(JEmp_Glide, true);
				UE_LOG(LogClass, Log, TEXT("Glide activated.")); break;
		}
		UE_LOG(LogClass, Log, TEXT("Current Max Energy: %f"), currentMaxEnergy);
		currentMaxEnergy += energyNeededForRune;

		if (currentMaxEnergy >= maxEnergy) currentMaxEnergy = maxEnergy;

		characterEnergy = currentMaxEnergy;

		this->UpdateLight();

		UE_LOG(LogClass, Log, TEXT("Current Max Energy: %f"), currentMaxEnergy);

		Friendly->Merge();
	}
}

void APlayerCharacter::Jump() {
	if (!isInteracting) {
		if (!isJumping) {
			Super::Jump();

			isJumping = true;
			jumpTime = 0.0f;
		} else if (this->GetJumpEmpowermentActive(JEmp_DoubleJump) && !doubleJumped) {
			canDoubleJump = true;
		}
		
		if (this->GetJumpEmpowermentActive(JEmp_Glide)) {
			isGliding = true;
		}
	}
}

void APlayerCharacter::StopJumping() {
	Super::StopJumping();

	isGliding = false;
}

void APlayerCharacter::JumpApex() {
	CharacterMovement->GravityScale = jumpFallGravity;

	isFalling = true;

	measureEnd = GetActorLocation();
	float distance = FVector::Dist(measureStart, measureEnd);
	UE_LOG(LogClass, Log, TEXT("Jump Height: %f"), distance);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Jump Height: %f"), distance));
}

void APlayerCharacter::JumpLanded(const FHitResult& Hit) {
	CharacterMovement->GravityScale = 1.0f;

	isJumping = false;
	isFalling = false;
	doubleJumped = false;
	canDoubleJump = false;
	isGliding = false;
	UE_LOG(LogClass, Log, TEXT("Landed"));
	CharacterMovement->bNotifyApex = true;
	jumpTime = 0.0f;
}


void APlayerCharacter::StartSprinting() {
	if (!isInteracting) {
		/*if (GetSprintEmpowermentActive(SEmp_Dash) && isSprinting) {
			dashEnabled = true;
			GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::Dash, dashEnabledTime);
			GetWorld()->GetTimerManager().PauseTimer(DashTimerHandle);
		}*/
		isSprinting = true;
		UE_LOG(LogClass, Log, TEXT("Started sprinting"));
		sprintKeyHoldTime = 0.0f;
	}
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
	} else if (isJumping && canDoubleJump) {
		this->SetCurrentMovementState(EMovementState::DoubleJump);
	} else if (isJumping && isGliding && isFalling) {
		this->SetCurrentMovementState(EMovementState::JumpGlide);
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
		case EMovementState::DoubleJump:
			this->DoubleJump(); break;
		case EMovementState::Jump:
			this->Jump(deltaTime); break;
		case EMovementState::JumpGlide:
			this->Glide(deltaTime); break;
		case EMovementState::Jumping:
			this->Jumping(deltaTime); break;
		case EMovementState::Moving: break;
		case EMovementState::Sprint:
			this->Sprint(deltaTime); break;
		case EMovementState::SprintDash:
			this->Dash(); break;
		case EMovementState::StopSprint:
			this->Decelerate(deltaTime, maxWalkSpeed, baseWalkSpeed); break;
		default: UE_LOG(LogClass, Warning, TEXT("Movement State not set."));
	}
}


void APlayerCharacter::Jump(float deltaTime) {
	if (characterEnergy > energyNeededForRune) {
		this->UseEnergy(jumpEnergyConsume);
	}

	jumpTime += deltaTime;

	measureStart = GetActorLocation();
}

void APlayerCharacter::Jumping(float deltaTime) {
	jumpTime += deltaTime;
}

void APlayerCharacter::DoubleJump() {
	CharacterMovement->GravityScale = 1.0f;

	// HAAAAAX!!!!
	CharacterMovement->MovementMode = EMovementMode::MOVE_Walking;
	CharacterMovement->DoJump(false);

	if (characterEnergy > energyNeededForRune) {
		this->UseEnergy(doubleJumpEnergyConsume);
	}

	doubleJumped = true;
	canDoubleJump = false;

	CharacterMovement->bNotifyApex = true;
}

void APlayerCharacter::Glide(float deltaTime) {
	FVector* currentVel = &CharacterMovement->Velocity;
	
	if (currentVel->Z < -glideSpeed) CharacterMovement->Velocity = FVector(currentVel->X, currentVel->Y, -glideSpeed);
}

void APlayerCharacter::Sprint(float deltaTime) {
	if (characterEnergy > energyNeededForRune) {
		this->UseEnergy(sprintEnergyConsume * deltaTime);
	}

	if (sprintKeyHoldTime == 0.0f && isSprinting) {
		*maxWalkSpeed = maxSprintSpeed;
		measureStart = GetActorLocation();
	}
	
	sprintKeyHoldTime += deltaTime;
}

void APlayerCharacter::Dash() {
	dashEnabled = false;
	UE_LOG(LogClass, Log, TEXT("Dash disabled"));
}

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

void APlayerCharacter::ChangeJumpHeight() {
	if (CharacterMovement->JumpZVelocity < baseJumpHeight + addedJumpHeight) {
		CharacterMovement->JumpZVelocity += addedJumpHeight;
	} else {
		CharacterMovement->JumpZVelocity = baseJumpHeight;
	}

	UE_LOG(LogClass, Log, TEXT("Jump Vel: %f"), CharacterMovement->JumpZVelocity);
}

void APlayerCharacter::UseEnergy(float amount) {
	characterEnergy -= amount;

	this->UpdateLight();
}

void APlayerCharacter::UpdateLight() {
	lightTempFactor = (maxLightTemp - minLightTemp) / currentMaxEnergy;

	LifeLight->AttenuationRadius = minLightRange + characterEnergy * lightRangeFactor;
	LifeLight->Temperature = minLightTemp + characterEnergy * lightTempFactor;
	LifeLight->UpdateColorAndBrightness();

	UE_LOG(LogClass, Log, TEXT("Attenuation Radius: %f"), LifeLight->AttenuationRadius);

	GetInteractionSphere()->SetSphereRadius(LifeLight->AttenuationRadius * 0.5f);
}

void APlayerCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	Super::EvaluateLightInteraction(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AEnvLightInteractable* const TestInteractable = Cast<AEnvLightInteractable>(OtherActor);

	if (TestInteractable && !TestInteractable->IsPendingKill() && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Log, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		TArray<AActor*> CollectedActors;
		TestInteractable->GetMesh()->GetOverlappingActors(CollectedActors);

		bool strongerEnemyInRange = false;

		for (int i = 0; i < CollectedActors.Num(); ++i) {
			const AEnemyAiCharacter* TestEnemy = Cast<AEnemyAiCharacter>(CollectedActors[i]);

			if (TestEnemy && !TestEnemy->IsPendingKill()) {
				// If enemy is stronger, strongerEnemyInrange = true; break;
			}
		}

		if (!strongerEnemyInRange) TestInteractable->ChangeState(EInteractionState::Lit);
	}
}

void APlayerCharacter::DisplayCurrentStates() {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, FString::Printf(TEXT("Current Energy: %f"), characterEnergy));
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Orange, FString::Printf(TEXT("Movement State: %d"), static_cast<uint8>(this->GetCurrentMovementState())));
	if (isInteracting && canSpend) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Cyan, FString::Printf(TEXT("Press Shift to spend.")));
	if (isInteracting && canConsume) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Cyan, FString::Printf(TEXT("Press Space to consume.")));
}