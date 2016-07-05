// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "EnvLightInteractable.h"
#include "LightsparkCharacter/PlayerCharacter.h"
#include "LightsparkCharacter/AI/EnemyAiCharacter.h"


AEnvLightInteractable::AEnvLightInteractable() {
	PrimaryActorTick.bCanEverTick = true;

	resetState = true;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	InteractionSphere->AttachTo(RootComponent);
}

void AEnvLightInteractable::BeginPlay() {
	Super::BeginPlay();

	this->CheckForCharacters();

	if (!GetSphere()->OnComponentEndOverlap.IsAlreadyBound(this, &AEnvLightInteractable::UpdateState) && resetState) {
		GetSphere()->OnComponentEndOverlap.AddDynamic(this, &AEnvLightInteractable::UpdateState);
	}
}

void AEnvLightInteractable::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (GetSphere()->OnComponentEndOverlap.IsAlreadyBound(this, &AEnvLightInteractable::UpdateState)) {
		GetSphere()->OnComponentEndOverlap.RemoveDynamic(this, &AEnvLightInteractable::UpdateState);
	}

	Super::EndPlay(EndPlayReason);
}

void AEnvLightInteractable::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AEnvLightInteractable::CheckForCharacters() {
	TArray<AActor*> CollectedActors;
	GetSphere()->GetOverlappingActors(CollectedActors);
	
	APlayerCharacter* Player = nullptr;
	AEnemyAiCharacter* Enemy = nullptr;

	int playerEnergy = -1;
	int enemyEnergy  = -1;

	for (int i = 0; i < CollectedActors.Num(); ++i) {
		APlayerCharacter* const TestPlayer = Cast<APlayerCharacter>(CollectedActors[i]);

		if (TestPlayer && !TestPlayer->IsPendingKill()) {
			Player = TestPlayer;
			break;
		}
	}

	for (int i = 0; i < CollectedActors.Num(); ++i) {
		AEnemyAiCharacter* const TestEnemy = Cast<AEnemyAiCharacter>(CollectedActors[i]);

		if (TestEnemy && !TestEnemy->IsPendingKill()) {
			Enemy = TestEnemy;
			break;
		}
	}

	if (!Player && !Enemy) { this->ChangeState(EInteractionState::Default); return; }

	if (Player && !Player->IsPendingKill()) playerEnergy = Player->GetCurrentCharacterEnergy();
	if (Enemy  && !Enemy->IsPendingKill())  enemyEnergy  = Enemy->GetCurrentCharacterEnergy();

	if (playerEnergy > enemyEnergy) {
		this->ChangeState(EInteractionState::Lit);
	} else {
		this->ChangeState(EInteractionState::Unlit);
	}
}

void AEnvLightInteractable::UpdateState(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	this->CheckForCharacters();
}