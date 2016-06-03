// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "LightsparkGameMode.generated.h"

UENUM(BlueprintType)
enum class ELightsparkPlayState {
	Playing,
	GameOver,
	Won,
	Unknown
};

UCLASS(minimalapi)
class ALightsparkGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALightsparkGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Lightspark")
	ELightsparkPlayState GetCurrentPlayState() const { return CurrentState; }

	void SetCurrentPlayState(ELightsparkPlayState NewState) { CurrentState = NewState; }

	void SaveGame(FString const &slotName = "DefaultSlot", uint32 const userIndex = 0);

	class UPlayerSave* LoadGame(FString const &slotName = "DefaultSlot", uint32 const userIndex = 0);

private:
	ELightsparkPlayState CurrentState;
};
