// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSave.generated.h"


UCLASS()
class LIGHTSPARK_API UPlayerSave : public USaveGame
{
	GENERATED_BODY()
	

public:
	UPlayerSave();
	
public:
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;


	UPROPERTY(VisibleAnywhere, Category = "Location")
	FName LevelName;

	UPROPERTY(VisibleAnywhere, Category = "Location")
	FVector CharacterLocation;

	UPROPERTY(VisibleAnywhere, Category = "Location")
	FRotator CharacterRotation;


	UPROPERTY(VisibleAnywhere, Category = "Camera")
	FRotator CameraBoomRotation;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	FVector CameraLocation;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	FRotator CameraRotation;


	UPROPERTY(VisibleAnywhere, Category = "Energy")
	float currentMaxEnergy;

	UPROPERTY(VisibleAnywhere, Category = "Energy")
	float characterEnergy;


	UPROPERTY(VisibleAnywhere, Category = "Empowerment")
	TArray<bool> SprintEmpowermentActive;

	UPROPERTY(VisibleAnywhere, Category = "Empowerment")
	TArray<bool> JumpEmpowermentActive;
};
