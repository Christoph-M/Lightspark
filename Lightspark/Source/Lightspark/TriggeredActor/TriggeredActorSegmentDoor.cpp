// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "TriggeredActorSegmentDoor.h"
#include "LightInteractable/EnvLightInteractable/EnvLightInteractableSaveSpot.h"
#include "LightsparkGameMode.h"
#include "LightsparkSaveGame.h"


ATriggeredActorSegmentDoor::ATriggeredActorSegmentDoor() {
	doorCanBeOpened = false;
	doorOpen = false;
}

void ATriggeredActorSegmentDoor::BeginPlay() {
	Super::BeginPlay();

	ULightsparkSaveGame* LoadInstance = ALightsparkGameMode::LoadGame();

	if (LoadInstance) {
		for (FLevelSegmentData Entry : LoadInstance->LevelSegments) {
			if (Entry.segment == this->segment) {
				doorOpen = Entry.doorOpen;
			}
		}
	}

	for (int i = 0; i < SaveSpots.Num(); ++i) {
		PlatesLitUp.Add(false);
	}

	this->Trigger(this);
	if (doorOpen) this->ActivateDoor();
}

void ATriggeredActorSegmentDoor::Trigger_Implementation(class AActor* OtherActor) {
	Super::Trigger_Implementation(OtherActor);

	AEnvLightInteractableSaveSpot* TestSaveSpot = Cast<AEnvLightInteractableSaveSpot>(OtherActor);

	if (TestSaveSpot && !TestSaveSpot->IsPendingKill()) {
		bool activateDoor = true;

		for (int i = 0; i < SaveSpots.Num(); ++i) {
			if (SaveSpots[i]->GetCurrentState() != EInteractionState::Lit) activateDoor = false;
			else PlatesLitUp[i] = true;
		}

		doorCanBeOpened = activateDoor;
	}
}

void ATriggeredActorSegmentDoor::OpenDoor() {
	if (doorCanBeOpened) {
		this->ActivateDoor();
	}
}

void ATriggeredActorSegmentDoor::ActivateDoor_Implementation() {
	UE_LOG(LogClass, Log, TEXT("Door has been opened"));

	if (!doorOpen) {
		doorOpen = true;

		ALightsparkGameMode* GameModeInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());

		GameModeInstance->SaveGame();
	}

	OnDoorOpened.Broadcast(segment);
}