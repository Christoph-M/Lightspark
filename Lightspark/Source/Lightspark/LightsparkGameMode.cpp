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

	GameSave = nullptr;
	IndexList = nullptr;

	for (int i = 0; i < 20; ++i) {
		DoorsOpen.Add(false);
	}
}

void ALightsparkGameMode::BeginPlay() {
	Super::BeginPlay();
	
	
	if (!IndexList) this->LoadIndexList();

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

	UGameplayStatics::SaveGameToSlot(IndexListInstance, IndexListInstance->SaveSlotName + "_" + *UGameplayStatics::GetCurrentLevelName(this), IndexListInstance->UserIndex);
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
	ULightsparkSaveGame* ActorLoadInstance = this->LoadGame();

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
	ULightsparkSaveGame* LightsparkSaveInstance = Cast<ULightsparkSaveGame>(UGameplayStatics::CreateSaveGameObject(ULightsparkSaveGame::StaticClass()));

		APlayerCharacter* MyCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

		LightsparkSaveInstance->SaveSlotName = slotName;
		LightsparkSaveInstance->UserIndex = 0;


		FStringAssetReference levelPath = FStringAssetReference(GetWorld()->GetCurrentLevel());

		FString str = levelPath.ToString();
		//str.RemoveAt(0, 11);
		//str.RemoveAt(str.Find(TEXT(":"), ESearchCase::IgnoreCase, ESearchDir::FromEnd), 16);
		str.ReplaceInline(TEXT("/Game/Maps/"), TEXT(""));
		str.ReplaceInline(TEXT(":PersistentLevel"), TEXT(""));

		LightsparkSaveInstance->LevelName = FName(*UGameplayStatics::GetCurrentLevelName(this));
		UE_LOG(LogClass, Log, TEXT("Level Path: %s"), *LightsparkSaveInstance->LevelName.ToString());


		int i = 0;

		for (TActorIterator<ATriggeredActorSegmentDoor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			ATriggeredActorSegmentDoor* Actor = *ActorItr;

			LightsparkSaveInstance->LevelSegments.Add(FLevelSegmentData());
			LightsparkSaveInstance->LevelSegments[i].segment = Actor->segment;
			LightsparkSaveInstance->LevelSegments[i].doorOpen = Actor->IsDoorOpen();

			++i;
		}


		LightsparkSaveInstance->Player.CharacterLocation = MyCharacter->GetActorLocation();
		LightsparkSaveInstance->Player.CharacterRotation = MyCharacter->GetActorRotation();

		LightsparkSaveInstance->Player.CameraBoomRotation = MyCharacter->GetCameraBoom()->RelativeRotation;
		LightsparkSaveInstance->Player.CameraLocation = MyCharacter->GetFollowCamera()->RelativeLocation;
		LightsparkSaveInstance->Player.CameraRotation = MyCharacter->GetFollowCamera()->RelativeRotation;

		LightsparkSaveInstance->Player.currentSegment = MyCharacter->GetCurrentSegment();
		UE_LOG(LogClass, Log, TEXT("Current Player Segment: %d"), MyCharacter->GetCurrentSegment());
		LightsparkSaveInstance->Player.segmentLit = this->IsDoorOpen(MyCharacter->GetCurrentSegment());
		LightsparkSaveInstance->Player.currentMaxEnergy = MyCharacter->GetCurrentMaxEnergy();
		LightsparkSaveInstance->Player.characterEnergy = MyCharacter->GetCurrentCharacterEnergy();
		LightsparkSaveInstance->Player.maxLightFlashUses = MyCharacter->GetMaxLightFlashUses();
		LightsparkSaveInstance->Player.lightFlashUses = MyCharacter->GetLightFlashUses();

		for (int i = 0; i < 4; ++i) {
			LightsparkSaveInstance->Player.SprintEmpowermentActive.Add(MyCharacter->GetSprintEmpowermentActive(i));
			LightsparkSaveInstance->Player.JumpEmpowermentActive.Add(MyCharacter->GetJumpEmpowermentActive(i));
		}


		this->SaveNPCs<AFriendlyAiCharacter>(LightsparkSaveInstance->NPCs);
		this->SaveNPCs<AEnemyAiCharacter>(LightsparkSaveInstance->Enemies);

		/*this->SaveActors<APlayerLightInteractable>(LightsparkSaveInstance->PlayerInteractables, PLAYER_INTERACTABLE);
		this->SaveActors<ATriggeredActor>(LightsparkSaveInstance->TriggeredActors, TRIGGERED_ACTOR);*/

		i = 0;

		for (TActorIterator<ALightInteractable> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			ALightInteractable* Actor = *ActorItr;

			LightsparkSaveInstance->PlayerInteractables.Add(FActorSaveData());
			LightsparkSaveInstance->PlayerInteractables[i].id = Actor->GetID();
			LightsparkSaveInstance->PlayerInteractables[i].ActorLocation = Actor->GetActorLocation();
			LightsparkSaveInstance->PlayerInteractables[i].ActorRotation = Actor->GetActorRotation();
			
			switch (Actor->GetCurrentState()) {
				case EInteractionState::Default: LightsparkSaveInstance->PlayerInteractables[i].interactionState = 0; break;
				case EInteractionState::Lit: LightsparkSaveInstance->PlayerInteractables[i].interactionState = 1; break;
				case EInteractionState::Unlit: LightsparkSaveInstance->PlayerInteractables[i].interactionState = 2; break;
				case EInteractionState::Destroyed: LightsparkSaveInstance->PlayerInteractables[i].interactionState = 3; break;
				case EInteractionState::Unknown: LightsparkSaveInstance->PlayerInteractables[i].interactionState = 4; break;
			}

			++i;
		}

	UGameplayStatics::SaveGameToSlot(LightsparkSaveInstance, LightsparkSaveInstance->SaveSlotName + "_" + UGameplayStatics::GetCurrentLevelName(this), LightsparkSaveInstance->UserIndex);
}

void ALightsparkGameMode::DeleteSaveGame() {
	ULightsparkSaveGame* SaveGame = Cast<ULightsparkSaveGame>(UGameplayStatics::LoadGameFromSlot("DefaultSlot_Segment_01_the_crypt", 0));
	
	if (SaveGame) UGameplayStatics::DeleteGameInSlot("DefaultSlot_Segment_01_the_crypt", 0);
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
	if (IndexList) return IndexList;

	IndexList = Cast<UIndexList>(UGameplayStatics::LoadGameFromSlot(TEXT("IndexList_") + UGameplayStatics::GetCurrentLevelName(this), 0));

	if (!IndexList) {
		this->CreateIndexLists();
		UE_LOG(LogClass, Log, TEXT("IndexList created."));
		IndexList = Cast<UIndexList>(UGameplayStatics::LoadGameFromSlot(TEXT("IndexList_") + UGameplayStatics::GetCurrentLevelName(this), 0));
	}

	return IndexList;
}

ULightsparkSaveGame* ALightsparkGameMode::LoadGame(FString const &slotName) {
	if (GameSave) return GameSave;

	GameSave = Cast<ULightsparkSaveGame>(UGameplayStatics::LoadGameFromSlot(slotName + "_" + UGameplayStatics::GetCurrentLevelName(this), 0));

	return GameSave;
}