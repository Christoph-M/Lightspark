// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerLightInteractableFlower.h"
#include "TriggeredActor/TriggeredActor.h"
#include "LightsparkCharacter/LightsparkCharacter.h"



APlayerLightInteractableFlower::APlayerLightInteractableFlower() {

}

void APlayerLightInteractableFlower::StateChangeDefault_Implementation() {
	Super::StateChangeDefault_Implementation();
}

void APlayerLightInteractableFlower::StateChangeLit_Implementation() {
	if (GetInteractionType() == EInteractionType::SpendOnly || GetInteractionType() == EInteractionType::SpendConsume) {
		Super::StateChangeLit_Implementation();
	}
}

void APlayerLightInteractableFlower::StateChangeUnlit_Implementation() {
	Super::StateChangeUnlit_Implementation();
}

void APlayerLightInteractableFlower::StateChangeDestroyed_Implementation() {
	if (GetInteractionType() == EInteractionType::ConsumeOnly || GetInteractionType() == EInteractionType::SpendConsume) {
		Super::StateChangeUnlit_Implementation();
	}
}

void APlayerLightInteractableFlower::StateChangeUnknown_Implementation() {
	Super::StateChangeUnknown_Implementation();
}

void APlayerLightInteractableFlower::ActivateTriggerActor() {
	Super::ActivateTriggerActor();
}

void APlayerLightInteractableFlower::ActivateTriggerCharacter() {
	Super::ActivateTriggerCharacter();
		// Lure enemy.
}