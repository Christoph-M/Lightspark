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

	ULightsparkSaveGame* ActorLoadInstance = ALightsparkGameMode::LoadGame(Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode()));

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
	UIndexList* IndexListInstance = ALightsparkGameMode::LoadIndexList(Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode()));

	if (!IndexListInstance) {
		UE_LOG(LogClass, Error, TEXT("Index List was not found!"));
		Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode())->CreateIndexLists();
		UE_LOG(LogClass, Log, TEXT("IndexList created."));
		IndexListInstance = ALightsparkGameMode::LoadIndexList(Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode()));
	}

	for (FIndexListData Entry : IndexListInstance->NPCIndexList) {
		if (this->GetActorLocation() == Entry.ActorLocation) {
			id = Entry.id;
		}
	}

	UE_LOG(LogClass, Log, TEXT("NPC ID: %d"), id);
}