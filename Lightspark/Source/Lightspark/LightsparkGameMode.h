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

	void SaveGame(FString const &slotName = "DefaultSlot");

	static class ULightsparkSaveGame* LoadGame(FString const &slotName = "DefaultSlot");
	static class UIndexList* LoadIndexList();

private:
	void CreateIndexLists();

	template <typename ActorType>
	FORCEINLINE void CreateIndexList(TArray<struct FIndexListData> &IndexList, uint32 range);

	template <typename ActorType>
	FORCEINLINE void LoadNPCsT(TArray<struct FNPCSaveData> &LoadDataList, class ULightsparkSaveGame* ActorLoadInstance);

	template <typename ActorType>
	FORCEINLINE void LoadActorsT(TArray<struct FActorSaveData> &LoadDataList, class ULightsparkSaveGame* ActorLoadInstance);

	template <typename ActorType>
	FORCEINLINE void SaveNPCs(TArray<struct FNPCSaveData> &SaveDataList);

	template <typename ActorType>
	FORCEINLINE void SaveActors(TArray<struct FActorSaveData> &SaveDataList);

	void LoadActors();

private:
	ELightsparkPlayState CurrentState;
};
