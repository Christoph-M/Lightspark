// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightInteractable/EnvLightInteractable/EnvLightInteractable.h"
#include "EnvLightInteractableSaveSpot.generated.h"


UCLASS()
class LIGHTSPARK_API AEnvLightInteractableSaveSpot : public AEnvLightInteractable
{
	GENERATED_BODY()
	
	
public:
	AEnvLightInteractableSaveSpot();

	virtual void StateChangeLit_Implementation() override;

	virtual void CheckForCharacters() override;
};
