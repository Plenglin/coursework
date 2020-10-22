#include <iostream>

#include "Player.h"
#include "Team.h"

using namespace std;

Team::Team() {
	for (int i=0; i < 7; i++) {
		this->players[i] = NULL;
	}
}

void Team::printAllNames() {
	for (int i=0; i < 7; i++) {
		Player *ptrPlayer = players[i];
		if (ptrPlayer != NULL) {  // Ensure it's been assigned
			cout << ptrPlayer->getName() << endl;
		}
	}
}