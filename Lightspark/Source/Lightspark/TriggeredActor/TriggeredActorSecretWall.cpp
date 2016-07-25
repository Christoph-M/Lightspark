// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "TriggeredActorSecretWall.h"
#include "LightsparkCharacter/PlayerCharacter.h"


ATriggeredActorSecretWall::ATriggeredActorSecretWall() {
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	Box->AttachTo(RootComponent);
}

void ATriggeredActorSecretWall::BeginPlay() {
	Super::BeginPlay();

	if (!Box->OnComponentBeginOverlap.IsAlreadyBound(this, &ATriggeredActorSecretWall::DestroyWall)) {
		Box->OnComponentBeginOverlap.AddDynamic(this, &ATriggeredActorSecretWall::DestroyWall);
	}
}

void ATriggeredActorSecretWall::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (Box->OnComponentBeginOverlap.IsAlreadyBound(this, &ATriggeredActorSecretWall::DestroyWall)) {
		Box->OnComponentBeginOverlap.RemoveDynamic(this, &ATriggeredActorSecretWall::DestroyWall);
	}

	Super::EndPlay(EndPlayReason);
}

void ATriggeredActorSecretWall::DestroyWall(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	APlayerCharacter* const CheckPlayer = Cast<APlayerCharacter>(OtherActor);

	if (CheckPlayer && !CheckPlayer->IsPendingKill() && OtherComp->ComponentHasTag(TEXT("Player"))) {
		if (CheckPlayer->GetCurrentMovementState() == EMovementState::Sprint) {
			// Destrooooooy!!
			this->SetActorHiddenInGame(true);
			this->SetActorTickEnabled(false);
			this->SetActorEnableCollision(false);
		}
	}
}