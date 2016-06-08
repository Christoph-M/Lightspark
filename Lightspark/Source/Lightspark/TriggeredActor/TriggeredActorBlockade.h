// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TriggeredActor.h"
#include "TriggeredActorBlockade.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSPARK_API ATriggeredActorBlockade : public ATriggeredActor
{
	GENERATED_BODY()
	
public:
	ATriggeredActorBlockade();
	
	virtual void Trigger(class AActor* OtherActor) override;
};
