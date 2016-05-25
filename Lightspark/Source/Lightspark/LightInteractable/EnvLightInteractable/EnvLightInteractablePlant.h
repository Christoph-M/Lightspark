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

protected:
	UFUNCTION()
	void ResetState(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
