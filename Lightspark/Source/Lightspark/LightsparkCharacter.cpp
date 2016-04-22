// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Lightspark.h"
#include "LightsparkCharacter.h"

//////////////////////////////////////////////////////////////////////////
// ALightsparkCharacter

ALightsparkCharacter::ALightsparkCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	interactionRadius = 200.0f;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRange"));
	InteractionSphere->AttachTo(RootComponent);
	InteractionSphere->SetSphereRadius(interactionRadius);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ALightsparkCharacter::BeginPlay() {
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ALightsparkCharacter::EvaluateLightInteraction);
}

// Called to bind functionality to input
void ALightsparkCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ALightsparkCharacter::EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	UE_LOG(LogClass, Log, TEXT("Actor: %s; Component: %s"), *OtherActor->GetName(), *OtherComp->GetName());
}