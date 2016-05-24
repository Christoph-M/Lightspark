// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerLightInteractable.h"
#include "TriggeredActor.h"



APlayerLightInteractable::APlayerLightInteractable() {
	InteractionType = EInteractionType::SpendConsume;
}

void APlayerLightInteractable::ActivateTriggerActor() {
	Super::ActivateTriggerActor();
}