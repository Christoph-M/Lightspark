// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "EnemyAiCharacter.h"
#include "LightInteractable/EnvLightInteractable/EnvLightInteractable.h"
#include "LightsparkCharacter/PlayerCharacter.h"
#include "TriggeredActor/TriggeredActorSegmentDoor.h"
#include "LightsparkGameMode.h"
#include "IndexList.h"
#include "LightsparkSaveGame.h"


AEnemyAiCharacter::AEnemyAiCharacter() {
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Enemy"));

	GetInteractionSphere()->ComponentTags.Add(TEXT("Dark"));
}

void AEnemyAiCharacter::BeginPlay() {
	Super::BeginPlay();

	for (TActorIterator<ATriggeredActorSegmentDoor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ATriggeredActorSegmentDoor* Door = *ActorItr;

		if (Door && !Door->IsPendingKill() && Door->segment == this->segment) {
			SegmentDoor = Door;

			if (!SegmentDoor->OnDoorOpened.IsAlreadyBound(this, &AEnemyAiCharacter::DoorOpened)) {
				SegmentDoor->OnDoorOpened.AddDynamic(this, &AEnemyAiCharacter::DoorOpened);
			}
		}
	}

	if (!GetCapsuleComponent()->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::CheckPlayer)) {
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAiCharacter::CheckPlayer);
	}
}

void AEnemyAiCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (GetCapsuleComponent()->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::CheckPlayer)) {
		GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyAiCharacter::CheckPlayer);
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyAiCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	Super::EvaluateLightInteraction(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AEnvLightInteractable* const TestInteractable = Cast<AEnvLightInteractable>(OtherActor);
	
	if (TestInteractable && !TestInteractable->IsPendingKill() && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Log, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		TestInteractable->CheckForCharacters();
	}
}

void AEnemyAiCharacter::CheckPlayer(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	APlayerCharacter* const TestPlayer = Cast<APlayerCharacter>(OtherActor);

	if (TestPlayer && !TestPlayer->IsPendingKill()) {
		TestPlayer->MyTakeDamage();
		this->Disable();
	}

	AEnvLightInteractable* const TestActor = Cast<AEnvLightInteractable>(OtherActor);

	if (TestActor && !TestActor->IsPendingKill()) {
		this->Disable();
	}
}

void AEnemyAiCharacter::SetID() {
	UIndexList* IndexListInstance = ALightsparkGameMode::LoadIndexList();

	if (IndexListInstance) {
		for (FIndexListData Entry : IndexListInstance->EnemyIndexList) {
			if (this->GetActorLocation() == Entry.ActorLocation) {
				id = Entry.id;
			}
		}
	} else {
		UE_LOG(LogClass, Error, TEXT("Index List was not found!"));
	}

	UE_LOG(LogClass, Log, TEXT("Enemy ID: %d"), id);
}

void AEnemyAiCharacter::DoorOpened(int32 segmentt) {
	this->Disable();

	if (SegmentDoor->OnDoorOpened.IsAlreadyBound(this, &AEnemyAiCharacter::DoorOpened)) {
		SegmentDoor->OnDoorOpened.RemoveDynamic(this, &AEnemyAiCharacter::DoorOpened);
	}
}