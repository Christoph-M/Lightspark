// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSPARK_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = Sound)
		void SetSoundClassVolume(USoundClass* TargetSoundClass, float NewVolume);

	UFUNCTION(BlueprintPure, Category = Sound)
		float GetSoundClassVolume(USoundClass* TargetSoundClass);	
};
