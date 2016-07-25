// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "IndexList.generated.h"


#define FRIENDLY_AI 100
#define ENEMY_AI 200
#define INTERACTABLE 300
#define TRIGGERED_ACTOR 400


USTRUCT()
struct FIndexListData {
	GENERATED_BODY()


	UPROPERTY()
	uint32 id;

	UPROPERTY()
	FVector ActorLocation;
};


UCLASS()
class LIGHTSPARK_API UIndexList : public USaveGame
{
	GENERATED_BODY()


public:
	UIndexList();

public:
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;


	UPROPERTY(VisibleAnywhere, Category = "Index List")
	TArray<FIndexListData> NPCIndexList;

	UPROPERTY(VisibleAnywhere, Category = "Index List")
	TArray<FIndexListData> EnemyIndexList;

	UPROPERTY(VisibleAnywhere, Category = "Index List")
	TArray<FIndexListData> InteractableIndexList;
	
	UPROPERTY(VisibleAnywhere, Category = "Index List")
	TArray<FIndexListData> TriggeredActorIndexList;
};
