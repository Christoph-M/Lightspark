// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "LightInteractable/LightInteractable.h"
#include "TriggeredActor/TriggeredActor.h"
#include "LightsparkCharacter/LightsparkCharacter.h"


// Sets default values
ALightInteractable::ALightInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentState = EInteractionState::Default;

	TriggerActorOnStateChange = EInteractionState::Unknown;
	TriggerCharacterOnStateChange = EInteractionState::Unknown;

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractionMesh"));
	RootComponent = InteractableMesh;

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->AttachTo(RootComponent);
	PointLight->SetVisibility(false);
}

// Called when the game starts or when spawned
void ALightInteractable::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ALightInteractable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ALightInteractable::ChangeState(EInteractionState newState) {
	if (CurrentState != newState) {
		if (CurrentState != EInteractionState::Destroyed) {
			CurrentState = newState;

			switch (CurrentState) {
				case EInteractionState::Default:	StateChangeDefault();	break;
				case EInteractionState::Lit:		StateChangeLit();		break;
				case EInteractionState::Unlit:		StateChangeUnlit();		break;
				case EInteractionState::Destroyed:	StateChangeDestroyed(); break;
				default:							StateChangeUnknown();
			}
		} else {
			
		}
	}
}

void ALightInteractable::StateChangeDefault_Implementation() {
	UE_LOG(LogClass, Log, TEXT("%s is in default state."), *this->GetName());

	if (this->GetTriggerActorOnStateChange() == EInteractionState::Default) this->ActivateTriggerActor();
	if (this->GetTriggerCharacterOnStateChange() == EInteractionState::Default) this->ActivateTriggerCharacter();
}

void ALightInteractable::StateChangeLit_Implementation() {
	UE_LOG(LogClass, Log, TEXT("%s was lit."), *this->GetName());

	if (this->GetTriggerActorOnStateChange() == EInteractionState::Lit) this->ActivateTriggerActor();
	if (this->GetTriggerCharacterOnStateChange() == EInteractionState::Lit) this->ActivateTriggerCharacter();
}

void ALightInteractable::StateChangeUnlit_Implementation() {
	UE_LOG(LogClass, Log, TEXT("%s was unlit."), *this->GetName());

	if (this->GetTriggerActorOnStateChange() == EInteractionState::Unlit) this->ActivateTriggerActor();
	if (this->GetTriggerCharacterOnStateChange() == EInteractionState::Unlit) this->ActivateTriggerCharacter();
}

void ALightInteractable::StateChangeDestroyed_Implementation() {
	UE_LOG(LogClass, Log, TEXT("%s was destroyed."), *this->GetName());

	if (this->GetTriggerActorOnStateChange() == EInteractionState::Destroyed) this->ActivateTriggerActor();
	if (this->GetTriggerCharacterOnStateChange() == EInteractionState::Destroyed) this->ActivateTriggerCharacter();
}

void ALightInteractable::StateChangeUnknown_Implementation() {
	UE_LOG(LogClass, Warning, TEXT("Unknown State."));
}

void ALightInteractable::ActivateTriggerActor() {
	if (TriggeredActor && !TriggeredActor->IsPendingKill()) {
		TriggeredActor->Trigger(this);
		UE_LOG(LogClass, Log, TEXT("Activated %s"), *this->TriggeredActor->GetName());
	} else {
		UE_LOG(LogClass, Warning, TEXT("Triggered Actor is not set or pending kill."));
	}
}

void ALightInteractable::ActivateTriggerCharacter() {
	if (TriggeredCharacter && !TriggeredCharacter->IsPendingKill()) {
		UE_LOG(LogClass, Log, TEXT("Activated %s"), *this->TriggeredActor->GetName());
	} else {
		UE_LOG(LogClass, Warning, TEXT("Triggered Character is not set or pending kill."));
	}
}