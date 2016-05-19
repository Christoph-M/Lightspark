// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerLightInteractableFlower.h"
#include "TriggeredActor.h"



APlayerLightInteractableFlower::APlayerLightInteractableFlower() {

}

void APlayerLightInteractableFlower::StateChangeDefault_Implementation() {
	Super::StateChangeDefault_Implementation();
}

void APlayerLightInteractableFlower::StateChangeLit_Implementation() {
	if (GetInteractionType() == EInteractionType::SpendOnly || GetInteractionType() == EInteractionType::SpendConsume) {
		Super::StateChangeLit_Implementation();

		this->ActivateTriggerActor();
	}
}

void APlayerLightInteractableFlower::StateChangeUnlit_Implementation() {
	
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

	ATriggeredActor* const _TriggeredActor = Cast<ATriggeredActor>(GetTriggeredActor());
	
	if (_TriggeredActor && !_TriggeredActor->IsPendingKill()) {
		_TriggeredActor->SetActorHiddenInGame(true);
		_TriggeredActor->SetActorTickEnabled(false);
		_TriggeredActor->SetActorEnableCollision(false);
	}
}