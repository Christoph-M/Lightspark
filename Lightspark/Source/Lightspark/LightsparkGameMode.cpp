// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Lightspark.h"
#include "LightsparkGameMode.h"
#include "LightsparkCharacter/LightsparkCharacter.h"
#include "LightsparkCharacter/PlayerCharacter.h"
#include "PlayerSave.h"

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
		UGameplayStatics::OpenLevel(this, FName(*PlayerLoadInstance->LevelName));
	}*/

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

void ALightsparkGameMode::SaveGame(FString const &slotName, uint32 const userIndex) {
	APlayerCharacter* MyCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	UPlayerSave* PlayerSaveInstance = Cast<UPlayerSave>(UGameplayStatics::CreateSaveGameObject(UPlayerSave::StaticClass()));
	PlayerSaveInstance->SaveSlotName = slotName;
	PlayerSaveInstance->UserIndex = userIndex;

	PlayerSaveInstance->LevelName = GetWorld()->GetMapName();

	PlayerSaveInstance->CharacterLocation = MyCharacter->GetActorLocation();
	PlayerSaveInstance->CharacterRotation = MyCharacter->GetActorRotation();

	PlayerSaveInstance->CameraBoomRotation = MyCharacter->GetCameraBoom()->RelativeRotation;
	PlayerSaveInstance->CameraLocation = MyCharacter->GetFollowCamera()->RelativeLocation;
	PlayerSaveInstance->CameraRotation = MyCharacter->GetFollowCamera()->RelativeRotation;

	PlayerSaveInstance->currentMaxEnergy = MyCharacter->GetCurrentMaxEnergy();
	PlayerSaveInstance->characterEnergy = MyCharacter->GetCurrentCharacterEnergy();

	for (int i = 0; i < 4; ++i) {
		PlayerSaveInstance->SprintEmpowermentActive.Add(MyCharacter->GetSprintEmpowermentActive(i));
		PlayerSaveInstance->JumpEmpowermentActive.Add(MyCharacter->GetJumpEmpowermentActive(i));
	}

	UGameplayStatics::SaveGameToSlot(PlayerSaveInstance, PlayerSaveInstance->SaveSlotName, PlayerSaveInstance->UserIndex);
}

UPlayerSave* ALightsparkGameMode::LoadGame(FString const &slotName, uint32 const userIndex) {
	UPlayerSave* PlayerLoadInstance = Cast<UPlayerSave>(UGameplayStatics::CreateSaveGameObject(UPlayerSave::StaticClass()));
	PlayerLoadInstance = Cast<UPlayerSave>(UGameplayStatics::LoadGameFromSlot(slotName, userIndex));

	if (PlayerLoadInstance) {
		return PlayerLoadInstance;
	} else {
		return nullptr;
	}
}