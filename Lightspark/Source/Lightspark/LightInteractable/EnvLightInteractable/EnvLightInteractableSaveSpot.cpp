// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "EnvLightInteractableSaveSpot.h"
#include "TriggeredActor/TriggeredActor.h"
#include "LightsparkGameMode.h"


AEnvLightInteractableSaveSpot::AEnvLightInteractableSaveSpot() {
	PrimaryActorTick.bCanEverTick = true;

	resetState = false;
}

void AEnvLightInteractableSaveSpot::StateChangeLit_Implementation() {
	Super::StateChangeLit_Implementation();

	ALightsparkGameMode* GameModeInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());
	//GameModeInstance->SaveGame();
}

void AEnvLightInteractableSaveSpot::CheckForCharacters() {
	UE_LOG(LogClass, Log, TEXT("ERMAHGERDBERDS!!!! %d"), static_cast<uint8>(this->GetCurrentState()));
	this->ChangeState(EInteractionState::Lit);
}