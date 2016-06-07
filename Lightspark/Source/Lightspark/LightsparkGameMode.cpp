// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Lightspark.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "LightsparkGameMode.h"
#include "LightsparkCharacter/LightsparkCharacter.h"
#include "LightsparkCharacter/PlayerCharacter.h"
#include "LightsparkCharacter/AI/FriendlyAiCharacter.h"
#include "NPCIndexList.h"
#include "LightsparkSaveGame.h"

ALightsparkGameMode::ALightsparkGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ALightsparkGameMode::BeginPlay() {
	Super::BeginPlay();

	/*UPlayerSave* PlayerLoadInstance = this->LoadGame();

	if (PlayerLoadInstance) {
		UGameplayStatics::OpenLevel(this, PlayerLoadInstance->LevelName);
	}*/

	UNPCIndexList* NPCIndexListInstance = Cast<UNPCIndexList>(UGameplayStatics::CreateSaveGameObject(UNPCIndexList::StaticClass()));

	int i = 0;

	for (TActorIterator<AFriendlyAiCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		AFriendlyAiCharacter* NPC = *ActorItr;

		NPCIndexListInstance->IndexList.Add(FIndexList());
		NPCIndexListInstance->IndexList[i].id = FRIENDLY_AI + i;
		NPCIndexListInstance->IndexList[i].NPCPosition = NPC->GetActorLocation();

		++i;
	}

	UE_LOG(LogClass, Log, TEXT("NPCIndexList count: %d"), NPCIndexListInstance->IndexList.Num());

	UGameplayStatics::SaveGameToSlot(NPCIndexListInstance, NPCIndexListInstance->SaveSlotName, NPCIndexListInstance->UserIndex);


	ULightsparkSaveGame* NPCLoadInstance = ALightsparkGameMode::LoadGame();

	for (TActorIterator<AFriendlyAiCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		AFriendlyAiCharacter* NPC = *ActorItr;

		NPC->SetID();

		if (NPCLoadInstance) {
			for (FNPCSaveData Entry : NPCLoadInstance->NPCs) {
				if (Entry.id == NPC->GetID()) {
					NPC->SetActorLocation(Entry.NPCLocation);
					NPC->SetActorRotation(Entry.NPCRotation);

					if (Entry.destroyed) NPC->Disable();
				}
			}
		}
	}


	this->SetCurrentPlayState(ELightsparkPlayState::Playing);
}

void ALightsparkGameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	APlayerCharacter* MyCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter) {
		if (MyCharacter->GetCurrentCharacterEnergy() <= 0.0f) {
			this->SetCurrentPlayState(ELightsparkPlayState::GameOver);
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

	LightpsarkSaveInstance->LevelName = FName(*str);
	UE_LOG(LogClass, Log, TEXT("Level Path: %s"), *LightpsarkSaveInstance->LevelName.ToString());


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


	UNPCIndexList* NPCIndexListInstance = this->LoadIndexList();

	int i = 0;

	for (TActorIterator<AFriendlyAiCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		AFriendlyAiCharacter* NPC = *ActorItr;

		LightpsarkSaveInstance->NPCs.Add(FNPCSaveData());
		LightpsarkSaveInstance->NPCs[i].id = NPC->GetID();
		LightpsarkSaveInstance->NPCs[i].NPCLocation = NPC->GetActorLocation();
		LightpsarkSaveInstance->NPCs[i].NPCRotation = NPC->GetActorRotation();

		if (NPC->IsEnabled()) {
			LightpsarkSaveInstance->NPCs[i].destroyed = false;
		} else {
			LightpsarkSaveInstance->NPCs[i].destroyed = true;
		}

		++i;
	}


	UGameplayStatics::SaveGameToSlot(LightpsarkSaveInstance, LightpsarkSaveInstance->SaveSlotName, LightpsarkSaveInstance->UserIndex);
}

UNPCIndexList* ALightsparkGameMode::LoadIndexList() {
	return Cast<UNPCIndexList>(UGameplayStatics::LoadGameFromSlot(TEXT("NPCIndexList"), 0));
}

ULightsparkSaveGame* ALightsparkGameMode::LoadGame(FString const &slotName) {
	return Cast<ULightsparkSaveGame>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
}