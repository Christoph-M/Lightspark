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

void AFriendlyAiCharacter::Merge_Implementation() {
	this->Disable();
}

void AFriendlyAiCharacter::SetID() {
	UIndexList* IndexListInstance = ALightsparkGameMode::LoadIndexList();

	if (IndexListInstance) {
		for (FIndexListData Entry : IndexListInstance->NPCIndexList) {
			if (this->GetActorLocation() == Entry.ActorLocation) {
				id = Entry.id;
			}
		}
	} else {
		UE_LOG(LogClass, Error, TEXT("Index List was not found!"));
	}

	UE_LOG(LogClass, Log, TEXT("NPC ID: %d"), id);
}