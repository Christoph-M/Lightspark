// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Lightspark.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "LightsparkGameMode.h"
#include "LightsparkCharacter/LightsparkCharacter.h"
#include "LightsparkCharacter/PlayerCharacter.h"
#include "LightsparkCharacter/AI/FriendlyAiCharacter.h"
#include "LightsparkCharacter/AI/EnemyAiCharacter.h"
#include "LightInteractable/PlayerLightInteractable/PlayerLightInteractable.h"
#include "TriggeredActor/TriggeredActor.h"
#include "TriggeredActor/TriggeredActorSegmentDoor.h"
#include "IndexList.h"
#include "LightsparkSaveGame.h"


ALightsparkGameMode::ALightsparkGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	for (int i = 0; i < 20; ++i) {
		DoorsOpen.Add(false);
	}

	updateIndexList = false;
}

void ALightsparkGameMode::BeginPlay() {
	Super::BeginPlay();
	
	
	if (FString(*UGameplayStatics::GetCurrentLevelName(this)) != TEXT("MainMenu")) {
		if (updateIndexList) {
			this->CreateIndexLists();
			updateIndexList = false;
		} /*else {
			UE_LOG(LogClass, Warning, TEXT("HELLO"));
			this->LoadActors();
		}*/
	}

	this->SetCurrentPlayState(ELightsparkPlayState::Playing);
	/*UE_LOG(LogClass, Warning, TEXT("Broadcast"));
	OnGameModeBeginPlay.Broadcast();*/
}

void ALightsparkGameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	APlayerCharacter* MyCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter) {
		if (MyCharacter->GetCurrentCharacterEnergy() < 0.0f) {
			this->SetCurrentPlayState(ELightsparkPlayState::GameOver);

			UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
		}
	}
}

void ALightsparkGameMode::CreateIndexLists() {
	UIndexList* IndexListInstance = Cast<UIndexList>(UGameplayStatics::CreateSaveGameObject(UIndexList::StaticClass()));

		this->CreateIndexList<AFriendlyAiCharacter>     (IndexListInstance->NPCIndexList,                FRIENDLY_AI);
		this->CreateIndexList<AEnemyAiCharacter>        (IndexListInstance->EnemyIndexList,              ENEMY_AI);
		this->CreateIndexList<ALightInteractable>		(IndexListInstance->InteractableIndexList,		 INTERACTABLE);
		this->CreateIndexList<ATriggeredActor>          (IndexListInstance->TriggeredActorIndexList,     TRIGGERED_ACTOR);

	UGameplayStatics::SaveGameToSlot(IndexListInstance, IndexListInstance->SaveSlotName, IndexListInstance->UserIndex);
}

template <typename ActorType>
void ALightsparkGameMode::CreateIndexList(TArray<FIndexListData> &IndexList, uint32 range) {
	int i = 0;

	for (TActorIterator<ActorType> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ActorType* Actor = *ActorItr;

		IndexList.Add(FIndexListData());
		IndexList[i].id = range + i;
		IndexList[i].ActorLocation = Actor->GetActorLocation();

		++i;
	}
}

void ALightsparkGameMode::LoadActors() {
	ULightsparkSaveGame* ActorLoadInstance = ALightsparkGameMode::LoadGame();

	if (ActorLoadInstance) {
		for (FLevelSegmentData Entry : ActorLoadInstance->LevelSegments) {
			DoorsOpen[Entry.segment - 1] = Entry.doorOpen;
		}
	}


	this->LoadNPCsT<AFriendlyAiCharacter>(ActorLoadInstance->NPCs, ActorLoadInstance);
	this->LoadNPCsT<AEnemyAiCharacter>(ActorLoadInstance->Enemies, ActorLoadInstance);

	/*this->LoadActorsT<APlayerLightInteractable>(ActorLoadInstance->PlayerInteractables, ActorLoadInstance);
	this->LoadActorsT<ATriggeredActor>(ActorLoadInstance->TriggeredActors, ActorLoadInstance);*/

	for (TActorIterator<ALightInteractable> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ALightInteractable* Actor = *ActorItr;

		Actor->SetID();


		if (ActorLoadInstance) {
			for (FActorSaveData Entry : ActorLoadInstance->PlayerInteractables) {
				if (Entry.id == Actor->GetID()) {
					Actor->SetActorLocation(Entry.ActorLocation);
					Actor->SetActorRotation(Entry.ActorRotation);

					switch (Entry.interactionState) {
						case 0: Actor->ChangeState(EInteractionState::Default); break;
						case 1: Actor->ChangeState(EInteractionState::Lit); break;
						case 2: Actor->ChangeState(EInteractionState::Unlit); break;
						case 3: Actor->ChangeState(EInteractionState::Destroyed); break;
						case 4: Actor->ChangeState(EInteractionState::Unknown); break;
					}
				}
			}
		}
	}
}

template <typename ActorType>
void ALightsparkGameMode::LoadNPCsT(TArray<FNPCSaveData> &LoadDataList, ULightsparkSaveGame* ActorLoadInstance) {
	for (TActorIterator<ActorType> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ActorType* NPC = *ActorItr;

		NPC->SetID();

		if (ActorLoadInstance) {
			for (FNPCSaveData Entry : LoadDataList) {
				if (Entry.id == NPC->GetID()) {
					NPC->SetActorLocation(Entry.ActorLocation);
					NPC->SetActorRotation(Entry.ActorRotation);
					
					if (Entry.destroyed) NPC->Disable();
				}
			}
		}
	}
}

template <typename ActorType>
void ALightsparkGameMode::LoadActorsT(TArray<FActorSaveData> &LoadDataList, ULightsparkSaveGame* ActorLoadInstance) {
	for (TActorIterator<ActorType> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ActorType* Actor = *ActorItr;

		Actor->SetID();


		if (ActorLoadInstance) {
			for (FActorSaveData Entry : LoadDataList) {
				if (Entry.id == Actor->GetID()) {
					Actor->SetActorLocation(Entry.ActorLocation);
					Actor->SetActorRotation(Entry.ActorRotation);
				}
			}
		}
	}
}

void ALightsparkGameMode::SaveGame(FString const &slotName) {
	ULightsparkSaveGame* LightpsarkSaveInstance = Cast<ULightsparkSaveGame>(UGameplayStatics::CreateSaveGameObject(ULightsparkSaveGame::StaticClass()));

		APlayerCharacter* MyCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

		LightpsarkSaveInstance->SaveSlotName = slotName;
		LightpsarkSaveInstance->UserIndex = 0;


		FStringAssetReference levelPath = FStringAssetReference(GetWorld()->GetCurrentLevel());

		FString str = levelPath.ToString();
		//str.RemoveAt(0, 11);
		//str.RemoveAt(str.Find(TEXT(":"), ESearchCase::IgnoreCase, ESearchDir::FromEnd), 16);
		str.ReplaceInline(TEXT("/Game/Maps/"), TEXT(""));
		str.ReplaceInline(TEXT(":PersistentLevel"), TEXT(""));

		LightpsarkSaveInstance->LevelName = FName(*UGameplayStatics::GetCurrentLevelName(this));
		UE_LOG(LogClass, Log, TEXT("Level Path: %s"), *LightpsarkSaveInstance->LevelName.ToString());


		int i = 0;

		for (TActorIterator<ATriggeredActorSegmentDoor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			ATriggeredActorSegmentDoor* Actor = *ActorItr;

			LightpsarkSaveInstance->LevelSegments.Add(FLevelSegmentData());
			LightpsarkSaveInstance->LevelSegments[i].segment = Actor->segment;
			LightpsarkSaveInstance->LevelSegments[i].doorOpen = Actor->IsDoorOpen();

			++i;
		}


		LightpsarkSaveInstance->Player.CharacterLocation = MyCharacter->GetActorLocation();
		LightpsarkSaveInstance->Player.CharacterRotation = MyCharacter->GetActorRotation();

		LightpsarkSaveInstance->Player.CameraBoomRotation = MyCharacter->GetCameraBoom()->RelativeRotation;
		LightpsarkSaveInstance->Player.CameraLocation = MyCharacter->GetFollowCamera()->RelativeLocation;
		LightpsarkSaveInstance->Player.CameraRotation = MyCharacter->GetFollowCamera()->RelativeRotation;

		LightpsarkSaveInstance->Player.currentMaxEnergy = MyCharacter->GetCurrentMaxEnergy();
		LightpsarkSaveInstance->Player.characterEnergy = MyCharacter->GetCurrentCharacterEnergy();

		for (int i = 0; i < 4; ++i) {
			LightpsarkSaveInstance->Player.SprintEmpowermentActive.Add(MyCharacter->GetSprintEmpowermentActive(i));
			LightpsarkSaveInstance->Player.JumpEmpowermentActive.Add(MyCharacter->GetJumpEmpowermentActive(i));
		}


		this->SaveNPCs<AFriendlyAiCharacter>(LightpsarkSaveInstance->NPCs);
		this->SaveNPCs<AEnemyAiCharacter>(LightpsarkSaveInstance->Enemies);

		/*this->SaveActors<APlayerLightInteractable>(LightpsarkSaveInstance->PlayerInteractables, PLAYER_INTERACTABLE);
		this->SaveActors<ATriggeredActor>(LightpsarkSaveInstance->TriggeredActors, TRIGGERED_ACTOR);*/

		i = 0;

		for (TActorIterator<ALightInteractable> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			ALightInteractable* Actor = *ActorItr;

			LightpsarkSaveInstance->PlayerInteractables.Add(FActorSaveData());
			LightpsarkSaveInstance->PlayerInteractables[i].id = Actor->GetID();
			LightpsarkSaveInstance->PlayerInteractables[i].ActorLocation = Actor->GetActorLocation();
			LightpsarkSaveInstance->PlayerInteractables[i].ActorRotation = Actor->GetActorRotation();
			
			switch (Actor->GetCurrentState()) {
				case EInteractionState::Default: LightpsarkSaveInstance->PlayerInteractables[i].interactionState = 0; break;
				case EInteractionState::Lit: LightpsarkSaveInstance->PlayerInteractables[i].interactionState = 1; break;
				case EInteractionState::Unlit: LightpsarkSaveInstance->PlayerInteractables[i].interactionState = 2; break;
				case EInteractionState::Destroyed: LightpsarkSaveInstance->PlayerInteractables[i].interactionState = 3; break;
				case EInteractionState::Unknown: LightpsarkSaveInstance->PlayerInteractables[i].interactionState = 4; break;
			}

			++i;
		}

	UGameplayStatics::SaveGameToSlot(LightpsarkSaveInstance, LightpsarkSaveInstance->SaveSlotName, LightpsarkSaveInstance->UserIndex);
}

template <typename ActorType>
void ALightsparkGameMode::SaveNPCs(TArray<FNPCSaveData> &SaveDataList) {
	int i = 0;

	for (TActorIterator<ActorType> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ActorType* NPC = *ActorItr;
		
		SaveDataList.Add(FNPCSaveData());
		SaveDataList[i].id = NPC->GetID();
		SaveDataList[i].ActorLocation = NPC->GetActorLocation();
		SaveDataList[i].ActorRotation = NPC->GetActorRotation();
		
		if (NPC->IsEnabled()) {
			SaveDataList[i].destroyed = false;
		} else {
			SaveDataList[i].destroyed = true;
		}

		++i;
	}
}

template <typename ActorType>
void ALightsparkGameMode::SaveActors(TArray<FActorSaveData> &SaveDataList, uint32 type) {
	int i = 0;

	for (TActorIterator<ActorType> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ActorType* Actor = *ActorItr;

		SaveDataList.Add(FActorSaveData());
		SaveDataList[i].id = Actor->GetID();
		SaveDataList[i].ActorLocation = Actor->GetActorLocation();
		SaveDataList[i].ActorRotation = Actor->GetActorRotation();

		++i;
	}
}

UIndexList* ALightsparkGameMode::LoadIndexList() {
	return Cast<UIndexList>(UGameplayStatics::LoadGameFromSlot(TEXT("IndexList"), 0));
}

ULightsparkSaveGame* ALightsparkGameMode::LoadGame(FString const &slotName) {
	return Cast<ULightsparkSaveGame>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
}