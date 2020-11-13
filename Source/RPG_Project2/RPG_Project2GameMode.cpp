// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPG_Project2GameMode.h"
#include "RPG_Project2Character.h"
#include "UObject/ConstructorHelpers.h"

ARPG_Project2GameMode::ARPG_Project2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/Player/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
