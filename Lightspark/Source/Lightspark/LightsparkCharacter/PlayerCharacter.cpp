// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerCharacter.h"
#include "LightsparkGameMode.h"
#include "LightInteractable/LightInteractable.h"
#include "LightInteractable/EnvLightInteractable/EnvLightInteractable.h"
#include "LightInteractable/EnvLightInteractable/EnvLightInteractableSaveSpot.h"
#include "TriggeredActor/TriggeredActorSegmentDoor.h"
#include "LightInteractable/PlayerLightInteractable/PlayerLightInteractableFlower.h"
#include "LightsparkCharacter/AI/EnemyAiCharacter.h"
#include "LightsparkCharacter/AI/FriendlyAiCharacter.h"
#include "LightsparkSaveGame.h"


APlayerCharacter::APlayerCharacter() {
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Player"));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	
	currentMaxEnergy = 10.0f;
	energyNeededForRune = 50.0f;
	lightEnergyGain = 2.0f;
	hitEnergyDamage = 2.0f;
	shadowEnergyDamage = 1.0f;
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
	dashSpeed = 20.0f;
	dashEnabledTime = 3.0f;

	sneakLightRangeOffset = 5.0f;
	sneakEnergy = 0.0f;
	sneakOffset = 0.0f;
	lightFlashRange = 5000.0f;
	lightFlashTime = 0.0f;
	initialAttenuationRadius = 0.0f;
	lightFlashActive = false;

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
	canDash = false;
	isInShadow = true;

	minLightRange = 600.0f;
	maxLightRange = 2000.0f;
	minLightTemp = 1000.0f;
	maxLightTemp = 12000.0f;
	minLightIntensity = 10.0f;
	maxLightIntensity = 50.0f;
	interactionRadiusFac = 0.5f;
	lifeLightDecTime = 1.0f;
	lightColorFade = 0.0f;
	lightColorOffsetFac = 0.4f;
	lightColorOffset = 2500.0f;
	lightColorFadeTime = 0.1f;
	lightUpdateTime = 0.0f;

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

	ULightsparkSaveGame* PlayerLoadInstance = ALightsparkGameMode::LoadGame();
	
	if (PlayerLoadInstance) {
		SetActorLocation(PlayerLoadInstance->Player.CharacterLocation);
		SetActorRotation(PlayerLoadInstance->Player.CharacterRotation);

		CameraBoom->SetRelativeRotation(PlayerLoadInstance->Player.CameraBoomRotation);
		FollowCamera->SetRelativeLocation(PlayerLoadInstance->Player.CameraLocation);
		FollowCamera->SetRelativeRotation(PlayerLoadInstance->Player.CameraRotation);

		currentMaxEnergy = PlayerLoadInstance->Player.currentMaxEnergy;
		characterEnergy = PlayerLoadInstance->Player.characterEnergy;

		for (int i = 0; i < 4; ++i) {
			this->SetSprintEmpowermentActive(i, PlayerLoadInstance->Player.SprintEmpowermentActive[i]);
			this->SetJumpEmpowermentActive(i, PlayerLoadInstance->Player.JumpEmpowermentActive[i]);

			if (i == SEmp_FasterSprint && this->GetSprintEmpowermentActive(SEmp_FasterSprint)) maxSprintSpeed = ((fasterSprintSpeedFactor / 100.0f) * baseWalkSpeed) + baseWalkSpeed;
			if (i == JEmp_HigherJump && this->GetJumpEmpowermentActive(JEmp_HigherJump)) CharacterMovement->JumpZVelocity += addedJumpHeight;
		}
	} else {
		if (currentMaxEnergy > maxEnergy) currentMaxEnergy = maxEnergy;
		if (characterEnergy > currentMaxEnergy) characterEnergy = currentMaxEnergy;
	}
	
	lightEnergy = characterEnergy;

	characterRunes = characterEnergy / energyNeededForRune;

	this->SetCurrentMovementState(EMovementState::Default);
	CurrentLightUpdateState = ELightUpdateState::NoUpdate;

	CharacterMovement->MaxWalkSpeed = baseWalkSpeed;
	maxSprintSpeed = ((sprintSpeedFactor / 100.0f) * baseWalkSpeed) + baseWalkSpeed;

	lightRangeFactor = (maxLightRange - minLightRange) / maxEnergy;
	lightIntensityFactor = (maxLightIntensity - minLightIntensity) / maxEnergy;

	lightFlashFadeInTime  = LightFlashFadeCurve->FloatCurve.Keys[1].Time - LightFlashFadeCurve->FloatCurve.Keys[0].Time;
	lightFlashFadeOutTime = LightFlashFadeCurve->FloatCurve.Keys[2].Time - LightFlashFadeCurve->FloatCurve.Keys[1].Time;

	this->InitLight();
	
	if (!OnReachedJumpApex.IsAlreadyBound(this, &APlayerCharacter::JumpApex)) {
		OnReachedJumpApex.AddDynamic(this, &APlayerCharacter::JumpApex);
	}
	if (!LandedDelegate.IsAlreadyBound(this, &APlayerCharacter::JumpLanded)) {
		LandedDelegate.AddDynamic(this, &APlayerCharacter::JumpLanded);
	}
	

	if (!GetCapsuleComponent()->OnComponentBeginOverlap.IsAlreadyBound(this, &APlayerCharacter::CheckInLight)) {
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::CheckInLight);
	}
	if (!GetCapsuleComponent()->OnComponentEndOverlap.IsAlreadyBound(this, &APlayerCharacter::CheckInShadow)) {
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::CheckInShadow);
	}

	GetWorld()->GetTimerManager().SetTimer(DisplayTimerHandle, this, &APlayerCharacter::DisplayCurrentStates, 0.2f, true);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (OnReachedJumpApex.IsAlreadyBound(this, &APlayerCharacter::JumpApex)) {
		OnReachedJumpApex.RemoveDynamic(this, &APlayerCharacter::JumpApex);
	}
	if (LandedDelegate.IsAlreadyBound(this, &APlayerCharacter::JumpLanded)) {
		LandedDelegate.RemoveDynamic(this, &APlayerCharacter::JumpLanded);
	}

	if (GetCapsuleComponent()->OnComponentBeginOverlap.IsAlreadyBound(this, &APlayerCharacter::CheckInLight)) {
		GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &APlayerCharacter::CheckInLight);
	}
	if (GetCapsuleComponent()->OnComponentEndOverlap.IsAlreadyBound(this, &APlayerCharacter::CheckInShadow)) {
		GetCapsuleComponent()->OnComponentEndOverlap.RemoveDynamic(this, &APlayerCharacter::CheckInShadow);
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	this->CheckMovementInput(deltaTime);
	this->EvaluateMovementState(deltaTime);

	if (!lightFlashActive) {
		if (!bIsCrouched) {
			if (isInShadow) {
				this->UseEnergy(shadowEnergyDamage * deltaTime);
			}
			else  if (characterEnergy < maxEnergy) {
				this->UseEnergy(-lightEnergyGain * deltaTime);
			}
		}

		this->UpdateLight(deltaTime);
	}
}

void APlayerCharacter::InitLight() {
	//lightTempFactor = (maxLightTemp - minLightTemp) / currentMaxEnergy;

	LifeLight->AttenuationRadius = minLightRange + characterEnergy * lightRangeFactor;
	LifeLight->Temperature = 8000.0f; //minLightTemp + characterEnergy * lightTempFactor;
	LifeLight->Intensity = lightIntensityFactor * characterEnergy + minLightIntensity;
	LifeLight->UpdateColorAndBrightness();
	LifeLight->UpdateComponentToWorld();

	GetInteractionSphere()->SetSphereRadius(LifeLight->AttenuationRadius * interactionRadiusFac);
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

	InputComponent->BindAction("Sneak", IE_Pressed, this, &APlayerCharacter::StartSneak);
	InputComponent->BindAction("Sneak", IE_Released, this, &APlayerCharacter::StopSneak);

	InputComponent->BindAction("FlashOfLight", IE_Pressed, this, &APlayerCharacter::StartLightFlash);

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


	InputComponent->BindAction("Checkpoint_0", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint0);
	InputComponent->BindAction("Checkpoint_1", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint1);
	InputComponent->BindAction("Checkpoint_2", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint2);
	InputComponent->BindAction("Checkpoint_3", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint3);
	InputComponent->BindAction("Checkpoint_4", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint4);
	InputComponent->BindAction("Checkpoint_5", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint5);
	InputComponent->BindAction("Checkpoint_6", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint6);
	InputComponent->BindAction("Checkpoint_7", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint7);
	InputComponent->BindAction("Checkpoint_8", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint8);
	InputComponent->BindAction("Checkpoint_9", IE_Pressed, this, &APlayerCharacter::TeleportToCheckpoint9);
}

void APlayerCharacter::TeleportToCheckpoint0() {
	this->SetActorLocation(Checkpoints[0]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint1() {
	this->SetActorLocation(Checkpoints[1]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint2() {
	this->SetActorLocation(Checkpoints[2]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint3() {
	this->SetActorLocation(Checkpoints[3]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint4() {
	this->SetActorLocation(Checkpoints[4]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint5() {
	this->SetActorLocation(Checkpoints[5]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint6() {
	this->SetActorLocation(Checkpoints[6]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint7() {
	this->SetActorLocation(Checkpoints[7]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint8() {
	this->SetActorLocation(Checkpoints[8]->GetActorLocation());
}

void APlayerCharacter::TeleportToCheckpoint9() {
	this->SetActorLocation(Checkpoints[9]->GetActorLocation());
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


void APlayerCharacter::MyTakeDamage() {
	if (characterEnergy <= 0.0f) {
		/*ALightsparkGameMode* GameMode = (ALightsparkGameMode*)GetWorld()->GetAuthGameMode();
		GameMode->SetCurrentPlayState(ELightsparkPlayState::GameOver);*/
		characterEnergy = -0.1f;
	} else {
		this->UseEnergy(hitEnergyDamage);
	}
}


void APlayerCharacter::Interact() {
	if (!isInteracting) {
		TArray<AActor*> CollectedActors;
		GetInteractionSphere()->GetOverlappingActors(CollectedActors);

		canSpend = false; canConsume = false;

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

				ALightsparkGameMode* GameModeInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());

				GameModeInstance->SaveGame();

				return;
			}


			ATriggeredActorSegmentDoor* TestDoor = Cast<ATriggeredActorSegmentDoor>(CollectedActors[i]);

			if (TestDoor && !TestDoor->IsPendingKill()) {
				TestDoor->OpenDoor();

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

		//this->UseEnergy(spendEnergyConsume);

		isInteracting = false;
		CharacterMovement->MovementMode = EMovementMode::MOVE_Walking;
	}
}

void APlayerCharacter::ConsumeEnergy() {
	if (isInteracting && canConsume && interactedActor && !interactedActor->IsPendingKill()) {
		APlayerLightInteractableFlower* const Flower = Cast<APlayerLightInteractableFlower>(interactedActor);
		interactedActor = nullptr;

		Flower->ChangeState(EInteractionState::Destroyed);

		/*this->ActivateLightUpdate();
		characterEnergy += consumeEnergyGain;*/

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

		currentMaxEnergy += energyNeededForRune;
		if (currentMaxEnergy >= maxEnergy) currentMaxEnergy = maxEnergy;

		this->ActivateLightUpdate();
		characterEnergy = currentMaxEnergy;

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
		if (GetSprintEmpowermentActive(SEmp_Dash)) {
			if (dashEnabled) {
				GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::DisableDash, dashEnabledTime);
				GetWorld()->GetTimerManager().PauseTimer(DashTimerHandle);

				canDash = true;
			}

			dashEnabled = true;
		}

		isSprinting = true;
		UE_LOG(LogClass, Log, TEXT("Started sprinting"));
		sprintKeyHoldTime = 0.0f;
	}
}

void APlayerCharacter::StopSprinting() {
	isSprinting = false;
	GetWorld()->GetTimerManager().UnPauseTimer(DashTimerHandle);
	UE_LOG(LogClass, Log, TEXT("Stopped sprinting. Hold time: %f"), sprintKeyHoldTime);
	sprintKeyHoldTime = 0.0f;
}


void APlayerCharacter::StartSneak() {
	if (!lightFlashActive) {
		Crouch();
		sneakEnergy = characterEnergy;
		this->UseEnergy(characterEnergy);
		sneakOffset = sneakLightRangeOffset;
		OnSneakToggle.Broadcast(true);
	}
}

void APlayerCharacter::StopSneak() {
	UnCrouch();
	this->UseEnergy(-sneakEnergy);
	sneakOffset = 0.0f;
	OnSneakToggle.Broadcast(false);
}


void APlayerCharacter::StartLightFlash() {
	if (!lightFlashActive) {
		lightFlashActive = true;
		this->StopSneak();
		characterEnergy = maxEnergy;
		*maxWalkSpeed = maxSprintSpeed;
		lightFlashTime = 0.0f;
		initialAttenuationRadius = LifeLight->AttenuationRadius;
		LifeLight->Intensity = lightIntensityFactor * characterEnergy + minLightIntensity;
	}
}


void APlayerCharacter::CheckMovementInput(float deltaTime) {
	if (lightFlashActive) {
		if (this->GetCurrentMovementState() != EMovementState::LightFlash)	this->SetCurrentMovementState(EMovementState::LightFlash);
	} else if (GetVelocity().IsZero()) {
		if (this->GetCurrentMovementState() != EMovementState::Default)		this->SetCurrentMovementState(EMovementState::Default);
	} else if (isJumping && jumpTime == 0.0f) {
		if (this->GetCurrentMovementState() != EMovementState::Jump)		this->SetCurrentMovementState(EMovementState::Jump);
	} else if (isJumping && canDoubleJump) {
		if (this->GetCurrentMovementState() != EMovementState::DoubleJump)	this->SetCurrentMovementState(EMovementState::DoubleJump);
	} else if (isJumping && isGliding && isFalling) {
		if (this->GetCurrentMovementState() != EMovementState::JumpGlide)	this->SetCurrentMovementState(EMovementState::JumpGlide);
	} else if (isJumping) {
		if (this->GetCurrentMovementState() != EMovementState::Jumping)		this->SetCurrentMovementState(EMovementState::Jumping);
	} else if (isSprinting && canDash) {
		if (this->GetCurrentMovementState() != EMovementState::SprintDash)	this->SetCurrentMovementState(EMovementState::SprintDash);
	} else if (isSprinting) {
		if (this->GetCurrentMovementState() != EMovementState::Sprint)		this->SetCurrentMovementState(EMovementState::Sprint);
	} else if (*maxWalkSpeed > baseWalkSpeed) {
		if (this->GetCurrentMovementState() != EMovementState::StopSprint)	this->SetCurrentMovementState(EMovementState::StopSprint);
	} else {
		if (this->GetCurrentMovementState() != EMovementState::Moving)		this->SetCurrentMovementState(EMovementState::Moving);
	}
}

void APlayerCharacter::EvaluateMovementState(float deltaTime) {
	switch (this->GetCurrentMovementState()) {
		case EMovementState::Default: break;
		case EMovementState::DoubleJump:
			this->DoubleJump(deltaTime); break;
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
		case EMovementState::LightFlash:
			this->LightFlash(deltaTime); break;
		default: UE_LOG(LogClass, Warning, TEXT("Movement State not set."));
	}
}


void APlayerCharacter::Jump(float deltaTime) {
	this->UseEnergy(jumpEnergyConsume);

	jumpTime += deltaTime;

	measureStart = GetActorLocation();
}

void APlayerCharacter::Jumping(float deltaTime) {
	jumpTime += deltaTime;
}

void APlayerCharacter::DoubleJump(float deltaTime) {
	CharacterMovement->GravityScale = 1.0f;

	// HAAAAAX!!!!
	CharacterMovement->MovementMode = EMovementMode::MOVE_Walking;
	CharacterMovement->DoJump(false);

	this->UseEnergy(doubleJumpEnergyConsume);

	doubleJumped = true;
	canDoubleJump = false;

	CharacterMovement->bNotifyApex = true;

	jumpTime += deltaTime;
}

void APlayerCharacter::Glide(float deltaTime) {
	FVector* currentVel = &CharacterMovement->Velocity;
	
	if (currentVel->Z < -glideSpeed) CharacterMovement->Velocity = FVector(currentVel->X, currentVel->Y, -glideSpeed);
}

void APlayerCharacter::Sprint(float deltaTime) {
	//this->UseEnergy(sprintEnergyConsume * deltaTime);

	if (sprintKeyHoldTime == 0.0f && isSprinting) {
		*maxWalkSpeed = maxSprintSpeed;
		measureStart = GetActorLocation();
	}
	
	sprintKeyHoldTime += deltaTime;
}

void APlayerCharacter::Dash() {
	this->UseEnergy(3.0f);
	CharacterMovement->Velocity *= dashSpeed;

	canDash = false;
}

void APlayerCharacter::DisableDash() {
	dashEnabled = false;
	canDash = false;
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

void APlayerCharacter::LightFlash(float deltaTime) {
	lightFlashTime += deltaTime;

	if (lightFlashTime >= LightFlashFadeCurve->FloatCurve.GetLastKey().Time) {
		lightFlashTime = LightFlashFadeCurve->FloatCurve.GetLastKey().Time;
		lightEnergy = maxEnergy;
		lightFlashActive = false;
	}

	if (lightFlashTime <= lightFlashFadeInTime) {
		LifeLight->AttenuationRadius = initialAttenuationRadius + LightFlashFadeCurve->GetFloatValue(lightFlashTime) * (lightFlashRange - initialAttenuationRadius);
	} else if (lightFlashTime >= lightFlashFadeOutTime) {
		float newRadius = minLightRange + characterEnergy * lightRangeFactor;
		LifeLight->AttenuationRadius = newRadius + LightFlashFadeCurve->GetFloatValue(lightFlashTime) * (lightFlashRange - newRadius);
	}

	LifeLight->UpdateColorAndBrightness();
	LifeLight->UpdateComponentToWorld();

	GetInteractionSphere()->SetSphereRadius(LifeLight->AttenuationRadius * 0.5f);
}

void APlayerCharacter::UseEnergy(float amount) {
	if (characterEnergy - amount > 0.0f || characterEnergy > 0.0f) {
		this->ActivateLightUpdate();
		characterEnergy -= amount;
		if (characterEnergy <= 0.0f) characterEnergy = 0.0f;
		if (characterEnergy >= maxEnergy) characterEnergy = maxEnergy;
	}
}

void APlayerCharacter::ActivateLightUpdate() {
	if (CurrentLightUpdateState == ELightUpdateState::NoUpdate) {
		lightEnergy = characterEnergy;
		CurrentLightUpdateState = ELightUpdateState::UpdateStart;
	}
}

void APlayerCharacter::UpdateLight(float deltaTime) {
	switch (CurrentLightUpdateState) {
		case ELightUpdateState::NoUpdate: {
			float intensityOffset = (sneakOffset > 0.0f) ? IntenstiyOffsetCurve->GetFloatValue(lightUpdateTime - floorf(lightUpdateTime)) * ((maxEnergy - characterEnergy) / maxEnergy) : 1.0f;
			LifeLight->Intensity = (lightIntensityFactor * lightEnergy + minLightIntensity - sneakOffset) * intensityOffset;
			LifeLight->UpdateColorAndBrightness(); break; }
		case ELightUpdateState::UpdateStart:
			if (lightEnergy == characterEnergy) { CurrentLightUpdateState = ELightUpdateState::UpdateEnd; break; }

			lightColorOffset = LifeLight->Temperature * lightColorOffsetFac;
			
			if (lightEnergy > characterEnergy) {
				CurrentLightUpdateState = ELightUpdateState::UpdateDecFadeIn;
			} else if (lightEnergy < characterEnergy) {
				CurrentLightUpdateState = ELightUpdateState::UpdateIncFadeIn;
			}

			initialLightEnergy = lightEnergy; break;
		case ELightUpdateState::UpdateDecFadeIn:
			lightEnergy -= (initialLightEnergy - characterEnergy) / lifeLightDecTime * deltaTime;
			lightColorFade -= lightColorOffset / lightColorFadeTime * deltaTime;

			LifeLight->Intensity = FlickerCurve->GetFloatValue(lightUpdateTime - floorf(lightUpdateTime));

			if (lightColorFade <= -lightColorOffset) lightColorFade = -lightColorOffset;

			if (lightEnergy <= characterEnergy) CurrentLightUpdateState = ELightUpdateState::UpdateDecFadeOut; break;
		case ELightUpdateState::UpdateIncFadeIn:
			lightEnergy += (characterEnergy - initialLightEnergy) / lifeLightDecTime * deltaTime;
			lightColorFade += lightColorOffset / lightColorFadeTime * deltaTime;

			if (lightColorFade >= lightColorOffset) lightColorFade = lightColorOffset;
		
			if (lightEnergy >= characterEnergy) CurrentLightUpdateState = ELightUpdateState::UpdateIncFadeOut; break;
		case ELightUpdateState::UpdateDecFadeOut:
			lightEnergy = characterEnergy;
			lightColorFade += lightColorOffset / lightColorFadeTime * deltaTime;

			LifeLight->Intensity = FlickerCurve->GetFloatValue(lightUpdateTime - floorf(lightUpdateTime));

			if (lightColorFade >= 0.0f) {
				LifeLight->Intensity = 50.0f;
				lightColorFade = 0.0f;
				CurrentLightUpdateState = ELightUpdateState::UpdateEnd;
			} break;
		case ELightUpdateState::UpdateIncFadeOut:
			lightEnergy = characterEnergy;
			lightColorFade -= lightColorOffset / lightColorFadeTime * deltaTime;

			if (lightColorFade <= 0.0f) {
				lightColorFade = 0.0f;
				CurrentLightUpdateState = ELightUpdateState::UpdateEnd;
			} break;
		case ELightUpdateState::UpdateEnd:
			lightUpdateTime = 0.0f;
			this->InitLight(); 
			CurrentLightUpdateState = ELightUpdateState::NoUpdate; break;
	}

	if (CurrentLightUpdateState != ELightUpdateState::UpdateEnd && CurrentLightUpdateState != ELightUpdateState::NoUpdate) {
		//lightTempFactor = (maxLightTemp - minLightTemp) / currentMaxEnergy;
		float intensityOffset = IntenstiyOffsetCurve->GetFloatValue(lightUpdateTime - floorf(lightUpdateTime)) * ((maxEnergy - characterEnergy) / maxEnergy);
		
		LifeLight->AttenuationRadius = minLightRange + lightEnergy * lightRangeFactor;
		//LifeLight->Temperature = 8000.0f + lightColorFade; //minLightTemp + lightEnergy * lightTempFactor + lightColorFade;
		LifeLight->Intensity = (maxLightIntensity * intensityOffset) + (lightIntensityFactor * lightEnergy + minLightIntensity);
		LifeLight->UpdateColorAndBrightness();
		LifeLight->UpdateComponentToWorld();

		GetInteractionSphere()->SetSphereRadius(LifeLight->AttenuationRadius * interactionRadiusFac);
	}

	lightUpdateTime += deltaTime;
}

void APlayerCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	Super::EvaluateLightInteraction(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AEnvLightInteractable* const TestInteractable = Cast<AEnvLightInteractable>(OtherActor);

	UE_LOG(LogClass, Error, TEXT("PLAYER CompName: %s"), *OtherComp->GetName());

	if (TestInteractable && !TestInteractable->IsPendingKill() && OtherComp->GetName() == TEXT("Sphere") && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Log, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		TestInteractable->CheckForCharacters();
	}
}

void APlayerCharacter::CheckInLight(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	AEnvLightInteractableSaveSpot* const TestInteractable = Cast<AEnvLightInteractableSaveSpot>(OtherActor);

	UE_LOG(LogClass, Warning, TEXT("PLAYER CompName: %s"), *OtherComp->GetName());

	if (TestInteractable && !TestInteractable->IsPendingKill() && OtherComp->GetName() == TEXT("Sphere") && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Warning, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		isInShadow = false;
	}
}

void APlayerCharacter::CheckInShadow(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	TArray<AActor*> CollectedActors;
	GetCapsuleComponent()->GetOverlappingActors(CollectedActors);

	if (CollectedActors.Num() == 0) {
		isInShadow = true; return;
	}

	for (int i = 0; i < CollectedActors.Num(); ++i) {
		AEnvLightInteractableSaveSpot* const TestInteractable = Cast<AEnvLightInteractableSaveSpot>(CollectedActors[i]);

		if (TestInteractable && !TestInteractable->IsPendingKill()) {
			return;
		}
	}

	isInShadow = true;
}

void APlayerCharacter::DisplayCurrentStates() {
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, FString::Printf(TEXT("Current Energy: %f"), characterEnergy));
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Orange, FString::Printf(TEXT("Movement State: %d"), static_cast<uint8>(this->GetCurrentMovementState())));
	if (GetSprintEmpowermentActive(0)) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Emerald, FString::Printf(TEXT("Faster Sprint active.")));
	if (GetSprintEmpowermentActive(1)) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Emerald, FString::Printf(TEXT("Reduced Sprint Cost active.")));
	if (GetSprintEmpowermentActive(2)) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Emerald, FString::Printf(TEXT("Dash active.")));
	if (GetSprintEmpowermentActive(3)) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Emerald, FString::Printf(TEXT("Thrust active.")));
	if (GetJumpEmpowermentActive(0)) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Emerald, FString::Printf(TEXT("Double Jump active.")));
	if (GetJumpEmpowermentActive(1)) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Emerald, FString::Printf(TEXT("Higher Jump active.")));
	if (GetJumpEmpowermentActive(2)) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Emerald, FString::Printf(TEXT("Reduced Jump Cost active.")));
	if (GetJumpEmpowermentActive(3)) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Emerald, FString::Printf(TEXT("Glide active.")));
	if (isInteracting && canSpend) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Cyan, FString::Printf(TEXT("Press Shift to spend.")));
	if (isInteracting && canConsume) GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Cyan, FString::Printf(TEXT("Press Space to consume.")));
}