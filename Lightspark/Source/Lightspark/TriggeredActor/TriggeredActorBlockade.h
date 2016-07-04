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
	
	UFUNCTION(BlueprintNativeEvent)
	void Trigger(class AActor* OtherActor);
	virtual void Trigger_Implementation(class AActor* OtherActor) override;
};
