// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "TriggeredActorSegmentDoor.h"
#include "LightInteractable/EnvLightInteractable/EnvLightInteractableSaveSpot.h"


ATriggeredActorSegmentDoor::ATriggeredActorSegmentDoor() {
	
}

void ATriggeredActorSegmentDoor::Trigger_Implementation(class AActor* OtherActor) {
	Super::Trigger_Implementation(OtherActor);

	AEnvLightInteractableSaveSpot* TestSaveSpot = Cast<AEnvLightInteractableSaveSpot>(OtherActor);

	if (TestSaveSpot && !TestSaveSpot->IsPendingKill()) {
		bool activateDoor = true;

		for (int i = 0; i < SaveSpots.Num(); ++i) {
			if (SaveSpots[i]->GetCurrentState() != EInteractionState::Lit) activateDoor = false;
		}

		if (activateDoor) this->OpenDoor();
	}
}

void ATriggeredActorSegmentDoor::OpenDoor_Implementation() {
	UE_LOG(LogClass, Log, TEXT("Door has been opened"));
	this->Destroy();
}