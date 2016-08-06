// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "FriendlyAiCharacter.h"
#include "LightsparkGameMode.h"
#include "IndexList.h"
#include "LightsparkSaveGame.h"


AFriendlyAiCharacter::AFriendlyAiCharacter() {
	SprintEmpowerment = ESprintEmpowerment::None;
	JumpEmpowerment = EJumpEmpowerment::None;
}

void AFriendlyAiCharacter::BeginPlay() {
	Super::BeginPlay();

	this->SetID();

	ULightsparkSaveGame* ActorLoadInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode())->LoadGame();

	if (ActorLoadInstance) {
		for (FNPCSaveData Entry : ActorLoadInstance->NPCs) {
			if (Entry.id == this->GetID()) {
				this->SetActorLocation(Entry.ActorLocation);
				this->SetActorRotation(Entry.ActorRotation);

				if (Entry.destroyed) this->Disable();
			}
		}
	}
}

void AFriendlyAiCharacter::Merge_Implementation() {
	this->Disable();
}

void AFriendlyAiCharacter::SetID() {
	ALightsparkGameMode* GameMode = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());
	UIndexList* IndexListInstance = GameMode->LoadIndexList();

	for (FIndexListData Entry : IndexListInstance->NPCIndexList) {
		if (this->GetActorLocation() == Entry.ActorLocation) {
			id = Entry.id;
		}
	}

	UE_LOG(LogClass, Log, TEXT("NPC ID: %d"), id);
}