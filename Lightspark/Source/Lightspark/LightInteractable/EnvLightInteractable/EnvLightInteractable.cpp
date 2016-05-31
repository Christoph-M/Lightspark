// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "EnvLightInteractable.h"
#include "LightsparkCharacter/PlayerCharacter.h"
#include "LightsparkCharacter/AI/EnemyAiCharacter.h"


AEnvLightInteractable::AEnvLightInteractable() {
	
}

void AEnvLightInteractable::BeginPlay() {
	this->CheckForCharacters();

	if (!GetMesh()->OnComponentEndOverlap.IsAlreadyBound(this, &AEnvLightInteractable::UpdateState)) {
		GetMesh()->OnComponentEndOverlap.AddDynamic(this, &AEnvLightInteractable::UpdateState);
	}
}

void AEnvLightInteractable::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (GetMesh()->OnComponentEndOverlap.IsAlreadyBound(this, &AEnvLightInteractable::UpdateState)) {
		GetMesh()->OnComponentEndOverlap.RemoveDynamic(this, &AEnvLightInteractable::UpdateState);
	}

	Super::EndPlay(EndPlayReason);
}

void AEnvLightInteractable::CheckForCharacters() {
	TArray<AActor*> CollectedActors;
	GetMesh()->GetOverlappingActors(CollectedActors);
	
	APlayerCharacter* Player = nullptr;
	AEnemyAiCharacter* Enemy = nullptr;

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

	if (Player && !Player->IsPendingKill()) {
		if (Enemy && !Enemy->IsPendingKill()) {
			if (Player->GetCurrentCharacterEnergy() > Enemy->GetCurrentCharacterEnergy()) {
				this->ChangeState(EInteractionState::Lit);
			} else {
				this->ChangeState(EInteractionState::Unlit);
			}
		} else {
			this->ChangeState(EInteractionState::Lit);
		}
	} else if (Enemy && !Enemy->IsPendingKill()) {
		this->ChangeState(EInteractionState::Unlit);
	}
}

void AEnvLightInteractable::UpdateState(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	ChangeState(EInteractionState::Default);
	
	this->CheckForCharacters();
}