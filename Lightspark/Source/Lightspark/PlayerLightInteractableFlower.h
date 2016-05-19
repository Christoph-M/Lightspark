// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerLightInteractable.h"
#include "PlayerLightInteractableFlower.generated.h"


UCLASS()
class LIGHTSPARK_API APlayerLightInteractableFlower : public APlayerLightInteractable
{
	GENERATED_BODY()
	
	
public:
	APlayerLightInteractableFlower();


	virtual void StateChangeDefault_Implementation() override;
	virtual void StateChangeLit_Implementation() override;
	virtual void StateChangeUnlit_Implementation() override;
	virtual void StateChangeDestroyed_Implementation() override;
	virtual void StateChangeUnknown_Implementation() override;

protected:
	virtual void ActivateTriggerActor() override;
};
