// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "EnemyAiCharacter.h"
#include "LightInteractable.h"


AEnemyAiCharacter::AEnemyAiCharacter() {
	GetCapsuleComponent()->ComponentTags.Add(TEXT("Enemy"));

	interactionRadius = 200.0f;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRange"));
	InteractionSphere->AttachTo(RootComponent);
	InteractionSphere->SetSphereRadius(interactionRadius);
	InteractionSphere->ComponentTags.Add(TEXT("Dark"));
}

void AEnemyAiCharacter::BeginPlay() {
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAiCharacter::EvaluateLightInteraction);
}

void AEnemyAiCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	ALightInteractable* const TestInteractable = Cast<ALightInteractable>(OtherActor);

	UE_LOG(LogClass, Log, TEXT("Actor: %s; Component: %s"), *OtherActor->GetName(), *OtherComp->GetName());

	if (TestInteractable && !TestInteractable->IsPendingKill() && TestInteractable->GetCurrentState() != EInteractionState::Destroyed) {
		UE_LOG(LogClass, Log, TEXT("Interactable Name: %s"), *TestInteractable->GetName());

		TestInteractable->ChangeState(EInteractionState::Unlit);
	}
}