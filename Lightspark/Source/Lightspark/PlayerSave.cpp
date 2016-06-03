// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "PlayerSave.h"


UPlayerSave::UPlayerSave() {
	SaveSlotName = TEXT("DefaultSlot");
	UserIndex = 0;
}

UPlayerSave::UPlayerSave(FString &slotName, uint32 userIndex) :
	SaveSlotName(slotName),
	UserIndex(userIndex) {}