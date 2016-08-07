// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightInteractable/EnvLightInteractable/EnvLightInteractable.h"
#include "EnvLightInteractableSaveSpot.generated.h"

UENUM(BlueprintType)
enum class ESaveSpotType {
	Red,
	Green,
	Blue,
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

protected:
	virtual void ActivateTriggerActor() override;

private:
	/**
	* Save Spot Type (TEnumAsByte<ESaveSpotType>)
	* The type of the save spot.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveSpotType", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESaveSpotType> saveSpotType;

	/**
	* Welcome Spot Pillar (class ATriggeredActor*)
	* Pillar to be triggered by state change.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	class ATriggeredActor* WelcomeSpotPillar;
};
