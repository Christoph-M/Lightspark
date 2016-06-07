// Fill out your copyright notice in the Description page of Project Settings.

#define FRIENDLY_AI 100
#define ENEMY_AI 200

#pragma once

#include "GameFramework/SaveGame.h"
#include "NPCIndexList.generated.h"


USTRUCT()
struct FIndexList {
	GENERATED_BODY()


	UPROPERTY()
	uint32 id;

	UPROPERTY()
	FVector NPCPosition;
};


UCLASS()
class LIGHTSPARK_API UNPCIndexList : public USaveGame
{
	GENERATED_BODY()
	
	
public:
	UNPCIndexList();

public:
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;


	UPROPERTY(VisibleAnywhere, Category = "Index List")
	TArray<FIndexList> IndexList;
};
