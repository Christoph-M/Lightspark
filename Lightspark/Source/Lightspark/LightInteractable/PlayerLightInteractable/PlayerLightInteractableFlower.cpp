// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerLightInteractableFlower.h"
#include "TriggeredActor/TriggeredActor.h"
#include "LightsparkCharacter/LightsparkCharacter.h"



APlayerLightInteractableFlower::APlayerLightInteractableFlower() {

}

void APlayerLightInteractableFlower::StateChangeDefault_Implementation() {
	Super::StateChangeDefault_Implementation();

	if (this->GetTriggerActorOnStateChange() == EInteractionState::Default) this->ActivateTriggerActor();
	if (this->GetTriggerCharacterOnStateChange() == EInteractionState::Default) this->ActivateTriggerCharacter();
}

void APlayerLightInteractableFlower::StateChangeLit_Implementation() {
	if (GetInteractionType() == EInteractionType::SpendOnly || GetInteractionType() == EInteractionType::SpendConsume) {
		Super::StateChangeLit_Implementation();

		if (this->GetTriggerActorOnStateChange() == EInteractionState::Lit) this->ActivateTriggerActor();
		if (this->GetTriggerCharacterOnStateChange() == EInteractionState::Lit) this->ActivateTriggerCharacter();
	}
}

void APlayerLightInteractableFlower::StateChangeUnlit_Implementation() {
	Super::StateChangeUnlit_Implementation();

	if (this->GetTriggerActorOnStateChange() == EInteractionState::Unlit) this->ActivateTriggerActor();
	if (this->GetTriggerCharacterOnStateChange() == EInteractionState::Unlit) this->ActivateTriggerCharacter();
}

void APlayerLightInteractableFlower::StateChangeDestroyed_Implementation() {
	if (GetInteractionType() == EInteractionType::ConsumeOnly || GetInteractionType() == EInteractionType::SpendConsume) {
		Super::StateChangeUnlit_Implementation();

		if (this->GetTriggerActorOnStateChange() == EInteractionState::Destroyed) this->ActivateTriggerActor();
		if (this->GetTriggerCharacterOnStateChange() == EInteractionState::Destroyed) this->ActivateTriggerCharacter();
	}
}

void APlayerLightInteractableFlower::StateChangeUnknown_Implementation() {
	Super::StateChangeUnknown_Implementation();
}

void APlayerLightInteractableFlower::ActivateTriggerActor() {
	Super::ActivateTriggerActor();

	ATriggeredActor* const TriggeredActor = this->GetTriggeredActor();

	if (TriggeredActor && !TriggeredActor->IsPendingKill()) {
		TriggeredActor->SetActorHiddenInGame(true);
		TriggeredActor->SetActorTickEnabled(false);
		TriggeredActor->SetActorEnableCollision(false);
	}
}

void APlayerLightInteractableFlower::ActivateTriggerCharacter() {
	Super::ActivateTriggerCharacter();

	ALightsparkCharacter* const TriggeredCharacter = this->GetTriggeredCharacter();

	if (TriggeredCharacter && !TriggeredCharacter->IsPendingKill()) {
		// Lure enemy.
	}
}