// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "EnvLightInteractablePlant.h"


AEnvLightInteractablePlant::AEnvLightInteractablePlant() {

}

void AEnvLightInteractablePlant::BeginPlay()
{
	Super::BeginPlay();
}

void AEnvLightInteractablePlant::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

void AEnvLightInteractablePlant::CheckForCharacters() {
	Super::CheckForCharacters();
}