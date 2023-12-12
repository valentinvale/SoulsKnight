// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulslikeTutorialGameMode.h"
#include "SoulslikeTutorialCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASoulslikeTutorialGameMode::ASoulslikeTutorialGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
