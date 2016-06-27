// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerLightInteractable.h"
#include "TriggeredActor/TriggeredActor.h"
#include "LightsparkGameMode.h"
#include "IndexList.h"



APlayerLightInteractable::APlayerLightInteractable() {
	InteractionType = EInteractionType::SpendConsume;
}

void APlayerLightInteractable::ActivateTriggerActor() {
	Super::ActivateTriggerActor();
}

void APlayerLightInteractable::SetID() {
	UIndexList* IndexListInstance = ALightsparkGameMode::LoadIndexList();

	if (IndexListInstance) {
		for (FIndexListData Entry : IndexListInstance->PlayerInteractableIndexList) {
			if (this->GetActorLocation() == Entry.ActorLocation) {
				id = Entry.id;
			}
		}
	} else {
		UE_LOG(LogClass, Error, TEXT("Index List was not found!"));
	}

	UE_LOG(LogClass, Log, TEXT("PlayerInteractable ID: %d"), id);
}