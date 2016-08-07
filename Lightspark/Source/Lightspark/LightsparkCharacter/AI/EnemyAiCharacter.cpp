// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAiCharacter.h"
#include "LightInteractable/EnvLightInteractable/EnvLightInteractable.h"
#include "LightsparkCharacter/PlayerCharacter.h"
#include "TriggeredActor/TriggeredActorSegmentDoor.h"
#include "LightsparkGameMode.h"
#include "IndexList.h"
#include "LightsparkSaveGame.h"
#include "Perception/PawnSensingComponent.h"
#include "Lightspark/LightsparkCharacter/AI/AI_Controller.h"


AEnemyAiCharacter::AEnemyAiCharacter() {
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Enemy"));

	GetInteractionSphere()->ComponentTags.Add(TEXT("Dark"));

	AIControllerClass = AAI_Controller::StaticClass();
	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensing"));
	PawnSensor->HearingThreshold = 0;
	PawnSensor->LOSHearingThreshold = PawnSensor->HearingThreshold;
	PawnSensor->SightRadius = 2000.0f;
	PawnSensor->SetPeripheralVisionAngle(45.0f);

	SensingRadius = CreateDefaultSubobject<USphereComponent>(TEXT("SensingRadius"));
	SensingRadius->AttachTo(GetCapsuleComponent());
	SensingRadius->SetSphereRadius(1000.0f);

	AttentionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("AttentionRadius"));
	AttentionRadius->AttachTo(GetCapsuleComponent());
	AttentionRadius->SetSphereRadius(2000.0f);

	EnemyInSight = false;	
}

//tick check intereaction sphere player 		GetInteractionSphere()->

void AEnemyAiCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PawnSensor->SensingInterval = 0.5f;
	PawnSensor->OnSeePawn.AddDynamic(this, &AEnemyAiCharacter::OnSeePawn);
}

void AEnemyAiCharacter::BeginPlay() {
	Super::BeginPlay();
	
	AAI_Controller* AICont;
	AICont = Cast<AAI_Controller>(GetController());

	AICont->SetEnemyWaypoints(cEnemyWaypoints);

	this->SetID();

	ULightsparkSaveGame* ActorLoadInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode())->LoadGame();

	if (ActorLoadInstance) {
		for (FNPCSaveData Entry : ActorLoadInstance->Enemies) {
			if (Entry.id == this->GetID()) {
				this->SetActorLocation(Entry.ActorLocation);
				this->SetActorRotation(Entry.ActorRotation);

				if (Entry.destroyed) this->Disable();
			}
		}
	}

	for (TActorIterator<ATriggeredActorSegmentDoor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ATriggeredActorSegmentDoor* Door = *ActorItr;

		if (Door && !Door->IsPendingKill() && Door->segment == this->segment) {
			SegmentDoor = Door;

			if (!SegmentDoor->OnDoorOpened.IsAlreadyBound(this, &AEnemyAiCharacter::DoorOpened)) {
				SegmentDoor->OnDoorOpened.AddDynamic(this, &AEnemyAiCharacter::DoorOpened);
			}
		}
	}

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (!PlayerCharacter->OnSneakToggle.IsAlreadyBound(this, &AEnemyAiCharacter::PlayerSneakToggle)) {
		PlayerCharacter->OnSneakToggle.AddDynamic(this, &AEnemyAiCharacter::PlayerSneakToggle);
	}

	if (!GetCapsuleComponent()->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::CheckPlayer)) {
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAiCharacter::CheckPlayer);
	}

	if (!SensingRadius->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::InSensRad)) {
		SensingRadius->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAiCharacter::InSensRad);
	}

	if (!AttentionRadius->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::InAttRad)) {
		AttentionRadius->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAiCharacter::InAttRad);
	}
}

void AEnemyAiCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (PlayerCharacter->OnSneakToggle.IsAlreadyBound(this, &AEnemyAiCharacter::PlayerSneakToggle)) {
		PlayerCharacter->OnSneakToggle.RemoveDynamic(this, &AEnemyAiCharacter::PlayerSneakToggle);
	}

	if (GetCapsuleComponent()->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::CheckPlayer)) {
		GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyAiCharacter::CheckPlayer);
	}

	if(SensingRadius->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::InSensRad)) {
		SensingRadius->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyAiCharacter::InSensRad);
	}

	if (AttentionRadius->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::InAttRad)) {
		AttentionRadius->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyAiCharacter::InAttRad);
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyAiCharacter::OnSeePawn(APawn * OtherCharacter)
{
	AAI_Controller* AICont;
	AICont = Cast<AAI_Controller>(GetController());

	if (OtherCharacter->IsA<ALightsparkCharacter>() && !EnemyInSight)
	{
		AICont->SetEnemy(OtherCharacter);
		NotifyOtherEnemies(OtherCharacter);
		EnemyInSight = true;
	}
}

void AEnemyAiCharacter::NotifyOtherEnemies(APawn * Pawn)
{
	TArray<AActor*> EnemiesToNotify;
	AttentionRadius->GetOverlappingActors(EnemiesToNotify);

	for (int i = 0; i < EnemiesToNotify.Num(); ++i)
	{
		AEnemyAiCharacter* OtherEnemy = Cast<AEnemyAiCharacter>(EnemiesToNotify[i]);

		if (OtherEnemy && !OtherEnemy->IsPendingKill()) 
		{
			AAI_Controller* OtherAICont = Cast<AAI_Controller>(OtherEnemy->GetController());
			OtherAICont->SetEnemy(Pawn);
		}
	}
}

void AEnemyAiCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	Super::EvaluateLightInteraction(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AEnvLightInteractable* const TestInteractable = Cast<AEnvLightInteractable>(OtherActor);
	
	if (TestInteractable && !TestInteractable->IsPendingKill() && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Log, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		TestInteractable->CheckForCharacters();
	}
}

void AEnemyAiCharacter::CheckPlayer(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	APlayerCharacter* const TestPlayer = Cast<APlayerCharacter>(OtherActor);

	if (TestPlayer && !TestPlayer->IsPendingKill()) {
		TestPlayer->MyTakeDamage();
		this->Destroy();
	}
}

void AEnemyAiCharacter::InAttRad(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APlayerCharacter* TestPlayer = Cast<APlayerCharacter>(OtherActor);

	if (TestPlayer && !TestPlayer->IsPendingKill()) {
		USphereComponent* TestSphere = Cast<USphereComponent>(OtherComp);

		if (TestSphere && !TestSphere->IsPendingKill() && TestSphere->ComponentHasTag("Light")) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("IN ATT RAD"));
		}
	}


	/*UPointLightComponent* LightSphere;

	if (OtherActor->FindComponentByClass<UPointLightComponent>())
	{
		LightSphere = OtherActor->FindComponentByClass<UPointLightComponent>();

		if (LightSphere->ComponentHasTag("LifeLight"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("IN ATTENTION RADIUS"));
		}
	}*/
}

void AEnemyAiCharacter::InSensRad(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APlayerCharacter* TestPlayer = Cast<APlayerCharacter>(OtherActor);

	if (TestPlayer && !TestPlayer->IsPendingKill()) 
	{
		USphereComponent* TestSphere = Cast<USphereComponent>(OtherComp);

		if (TestSphere && !TestSphere->IsPendingKill() && TestSphere->ComponentHasTag("Light") && !EnemyInSight) 
		{
			AAI_Controller* AICont;
			AICont = Cast<AAI_Controller>(GetController());
			ALightsparkCharacter* Character = Cast<ALightsparkCharacter>(OtherActor);

			if (Character && !Character->IsPendingKill()) 
			{
				AICont->SetEnemy(Character);
				NotifyOtherEnemies(Character);
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TOO CLOSE"));
		}
	}

	/*UPointLightComponent* LightSphere;

	if (OtherActor->FindComponentByClass<UPointLightComponent>())
	{
		LightSphere = OtherActor->FindComponentByClass<UPointLightComponent>();

		if (LightSphere->ComponentHasTag("LifeLight"))
		{
			AAI_Controller* AICont;
			AICont = Cast<AAI_Controller>(GetController());
			ALightsparkCharacter* Character = Cast<ALightsparkCharacter>(OtherActor);

			if (OtherActor->IsA<ALightsparkCharacter>())
			{
				AICont->SetEnemy(Character);
				NotifyOtherEnemies(Character);
			}
		}
	}*/
}

void AEnemyAiCharacter::SetID() {
	ALightsparkGameMode* GameMode = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());
	UIndexList* IndexListInstance = GameMode->LoadIndexList();

	for (FIndexListData Entry : IndexListInstance->EnemyIndexList) {
		if (this->GetActorLocation() == Entry.ActorLocation) {
			id = Entry.id;
		}
	}

	UE_LOG(LogClass, Log, TEXT("Enemy ID: %d"), id);
}

void AEnemyAiCharacter::DoorOpened(int32 segmentt) {
	this->Disable();

	if (SegmentDoor->OnDoorOpened.IsAlreadyBound(this, &AEnemyAiCharacter::DoorOpened)) {
		SegmentDoor->OnDoorOpened.RemoveDynamic(this, &AEnemyAiCharacter::DoorOpened);
	}
}

void AEnemyAiCharacter::PlayerSneakToggle(bool isSneaking) {
	UE_LOG(LogClass, Log, TEXT("Player sneak: %d"), isSneaking);
}