// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "EnemyAiCharacter.h"
#include "LightInteractable.h"


AEnemyAiCharacter::AEnemyAiCharacter() {
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Enemy"));

	GetInteractionSphere()->ComponentTags.Add(TEXT("Dark"));
}

void AEnemyAiCharacter::BeginPlay() {
	Super::BeginPlay();

	if (!GetInteractionSphere()->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::EvaluateLightInteraction)) {
		GetInteractionSphere()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAiCharacter::EvaluateLightInteraction);
	}
}

void AEnemyAiCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (GetInteractionSphere()->OnComponentBeginOverlap.IsAlreadyBound(this, &AEnemyAiCharacter::EvaluateLightInteraction)) {
		GetInteractionSphere()->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemyAiCharacter::EvaluateLightInteraction);
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyAiCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	Super::EvaluateLightInteraction(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ALightInteractable* const TestInteractable = Cast<ALightInteractable>(OtherActor);

	if (TestInteractable && !TestInteractable->IsPendingKill() && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Log, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		TestInteractable->ChangeState(EInteractionState::Unlit);
	}
}