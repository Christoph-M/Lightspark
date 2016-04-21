// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Lightspark.h"
#include "LightsparkGameMode.h"
#include "LightsparkCharacter.h"

ALightsparkGameMode::ALightsparkGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
