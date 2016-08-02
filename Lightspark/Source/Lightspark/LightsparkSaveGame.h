// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "LightsparkSaveGame.generated.h"


USTRUCT()
struct FPlayerSaveData {
	GENERATED_BODY()


	UPROPERTY()
	FVector CharacterLocation;

	UPROPERTY()
	FRotator CharacterRotation;


	UPROPERTY()
	FRotator CameraBoomRotation;

	UPROPERTY()
	FVector CameraLocation;

	UPROPERTY()
	FRotator CameraRotation;


	UPROPERTY()
	int currentSegment;

	UPROPERTY()
	bool segmentLit;

	UPROPERTY()
	float currentMaxEnergy;

	UPROPERTY()
	float characterEnergy;


	UPROPERTY()
	TArray<bool> SprintEmpowermentActive;

	UPROPERTY()
	TArray<bool> JumpEmpowermentActive;
};

USTRUCT()
struct FNPCSaveData {
	GENERATED_BODY()


	UPROPERTY()
	uint32 id;

	UPROPERTY()
	FVector ActorLocation;

	UPROPERTY()
	FRotator ActorRotation;

	UPROPERTY()
	bool destroyed;
};

USTRUCT()
struct FActorSaveData {
	GENERATED_BODY()


	UPROPERTY()
	uint32 id;

	UPROPERTY()
	FVector ActorLocation;

	UPROPERTY()
	FRotator ActorRotation;

	UPROPERTY()
	uint8 interactionState;

	/*UPROPERTY()
	bool hasBeenTriggered;*/
};

USTRUCT()
struct FLevelSegmentData {
	GENERATED_BODY()

	UPROPERTY()
	uint32 segment;
	
	UPROPERTY()
	bool doorOpen;
};


UCLASS()
class LIGHTSPARK_API ULightsparkSaveGame : public USaveGame
{
	GENERATED_BODY()


public:
	ULightsparkSaveGame();

public:
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;


	UPROPERTY(VisibleAnywhere, Category = "Location")
	FName LevelName;


	UPROPERTY(VisibleAnywhere, Category = "LevelSegment")
	TArray<FLevelSegmentData> LevelSegments;


	UPROPERTY(VisibleAnywhere, Category = "Player")
	FPlayerSaveData Player;

	UPROPERTY(VisibleAnywhere, Category = "NPCs")
	TArray<FNPCSaveData> NPCs;

	UPROPERTY(VisibleAnywhere, Category = "NPCs")
	TArray<FNPCSaveData> Enemies;

	UPROPERTY(VisibleAnywhere, Category = "Actors")
	TArray<FActorSaveData> PlayerInteractables;

	/*UPROPERTY(VisibleAnywhere, Category = "Actors")
	TArray<FActorSaveData> TriggeredActors;*/
};
