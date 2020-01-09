// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameState.h"

AABGameState::AABGameState()
{
	TotalGameScore = 0;
	bGameCleard = false;
}

int32 AABGameState::GetTotalGameScore() const
{
	return TotalGameScore;
}

void AABGameState::AddGameScore()
{
	TotalGameScore++;
}

void AABGameState::SetGameCleard()
{
	bGameCleard = true;
}

bool AABGameState::IsGameCleard() const
{
	return bGameCleard;
}

