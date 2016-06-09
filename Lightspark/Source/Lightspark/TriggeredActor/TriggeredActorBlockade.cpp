// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "TriggeredActorBlockade.h"


ATriggeredActorBlockade::ATriggeredActorBlockade() {
	this->Tags.Add(TEXT("Blockade"));
}

void ATriggeredActorBlockade::Trigger(AActor* OtherActor) {
	Super::Trigger(OtherActor);

	this->SetActorHiddenInGame(true);
	this->SetActorTickEnabled(false);
	this->SetActorEnableCollision(false);
}