// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightInteractable/EnvLightInteractable/EnvLightInteractable.h"
#include "EnvLightInteractableSaveSpot.generated.h"

UENUM(BlueprintType)
enum class ESaveSpotType {
	Red,
	Blue,
	Green,
	Unknown
};

UCLASS()
class LIGHTSPARK_API AEnvLightInteractableSaveSpot : public AEnvLightInteractable
{
	GENERATED_BODY()
	
	
public:
	AEnvLightInteractableSaveSpot();

	virtual void StateChangeLit_Implementation() override;

	virtual void CheckForCharacters() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveSpotType", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESaveSpotType> saveSpotType;
};
