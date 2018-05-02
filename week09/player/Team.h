#pragma once

#include "Player.h"

class Team {
public:
	Player *players[7];
	Team();	
	void printAllNames();
};