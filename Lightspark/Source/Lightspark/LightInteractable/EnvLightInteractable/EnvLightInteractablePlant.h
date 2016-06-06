// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvLightInteractable.h"
#include "EnvLightInteractablePlant.generated.h"


UCLASS()
class LIGHTSPARK_API AEnvLightInteractablePlant : public AEnvLightInteractable
{
	GENERATED_BODY()
	
public:
	AEnvLightInteractablePlant();


	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	virtual void CheckForCharacters() override;
};
