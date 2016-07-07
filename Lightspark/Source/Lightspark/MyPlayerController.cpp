// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "MyPlayerController.h"

void AMyPlayerController::SetSoundClassVolume(USoundClass* TargetSoundClass, float NewVolume)
{
	if (TargetSoundClass)
	{
		TargetSoundClass->Properties.Volume = NewVolume;
	}
}

float AMyPlayerController::GetSoundClassVolume(USoundClass* TargetSoundClass)
{
	if (TargetSoundClass)
	{
		return TargetSoundClass->Properties.Volume;
	}
	return 0.0f;
}


