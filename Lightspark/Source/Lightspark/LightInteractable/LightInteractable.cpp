// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "LightInteractable/LightInteractable.h"
#include "TriggeredActor/TriggeredActor.h"
#include "TriggeredActor/TriggeredActorSegmentDoor.h"
#include "LightsparkCharacter/LightsparkCharacter.h"
#include "LightsparkGameMode.h"
#include "IndexList.h"
#include "LightsparkSaveGame.h"


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

	this->SetID();

	ULightsparkSaveGame* ActorLoadInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode())->LoadGame();

	if (ActorLoadInstance) {
		for (FActorSaveData Entry : ActorLoadInstance->PlayerInteractables) {
			if (Entry.id == this->GetID()) {
				this->SetActorLocation(Entry.ActorLocation);
				this->SetActorRotation(Entry.ActorRotation);

				switch (Entry.interactionState) {
					case 0: this->ChangeState(EInteractionState::Default); break;
					case 1: this->ChangeState(EInteractionState::Lit); break;
					case 2: this->ChangeState(EInteractionState::Unlit); break;
					case 3: this->ChangeState(EInteractionState::Destroyed); break;
					case 4: this->ChangeState(EInteractionState::Unknown); break;
				}

				break;
			}
		}
	}
	

	for (TActorIterator<ATriggeredActorSegmentDoor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ATriggeredActorSegmentDoor* Door = *ActorItr;

		if (Door && !Door->IsPendingKill() && Door->segment == this->segment) {
			SegmentDoor = Door;

			if (!SegmentDoor->OnDoorOpened.IsAlreadyBound(this, &ALightInteractable::LightUp)) {
				SegmentDoor->OnDoorOpened.AddDynamic(this, &ALightInteractable::LightUp);
			}
		}
	}


	/*ALightsparkGameMode* GameModeInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());

	if (GameModeInstance && !GameModeInstance->OnGameModeBeginPlay.IsAlreadyBound(this, &ALightInteractable::MyBeginPlay)) {
		GameModeInstance->OnGameModeBeginPlay.AddDynamic(this, &ALightInteractable::MyBeginPlay);
	}*/
}

void ALightInteractable::MyBeginPlay() {
	UE_LOG(LogClass, Warning, TEXT("%s"), *this->GetName());

	for (TActorIterator<ATriggeredActorSegmentDoor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ATriggeredActorSegmentDoor* Door = *ActorItr;

		if (Door && !Door->IsPendingKill() && Door->segment == this->segment) {
			SegmentDoor = Door;

			if (!SegmentDoor->OnDoorOpened.IsAlreadyBound(this, &ALightInteractable::LightUp)) {
				SegmentDoor->OnDoorOpened.AddDynamic(this, &ALightInteractable::LightUp);
			}
		}
	}
}

void ALightInteractable::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	ALightsparkGameMode* GameModeInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());

	if (GameModeInstance && !GameModeInstance->OnGameModeBeginPlay.IsAlreadyBound(this, &ALightInteractable::MyBeginPlay)) {
		GameModeInstance->OnGameModeBeginPlay.RemoveDynamic(this, &ALightInteractable::MyBeginPlay);
	}
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ALightInteractable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ALightInteractable::SetID() {
	ALightsparkGameMode* GameMode = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());
	UIndexList* IndexListInstance = GameMode->LoadIndexList();

	for (FIndexListData Entry : IndexListInstance->InteractableIndexList) {
		if (this->GetActorLocation() == Entry.ActorLocation) {
			id = Entry.id;
		}
	}

	UE_LOG(LogClass, Log, TEXT("LightInteractable ID: %d; %s"), id, *this->GetName());
}

void ALightInteractable::ChangeState(EInteractionState newState) {
	if (CurrentState != newState) {
		if (CurrentState != EInteractionState::Destroyed) {
			CurrentState = newState;

			switch (CurrentState) {
				case EInteractionState::Default:	StateChangeDefault();	break;
				case EInteractionState::Lit:		StateChangeLit();		break;
				case EInteractionState::Unlit:		StateChangeUnlit();	    break;
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

void ALightInteractable::LightUp(int32 segment) {
	this->ChangeState(EInteractionState::Lit);

	if (SegmentDoor->OnDoorOpened.IsAlreadyBound(this, &ALightInteractable::LightUp)) {
		SegmentDoor->OnDoorOpened.RemoveDynamic(this, &ALightInteractable::LightUp);
	}
}