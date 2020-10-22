#include <iostream>

#include "Player.h"
#include "Team.h"


int main() {
	Player *players[7] = {
		new Player("George"),
		new Player("Ivan"),
		new Player("Hang"),
		new Player("Tuyet"),
		new Player(),
		new Player(),
		new Player()
	};
	players[4]->setName("Sue");
	players[5]->setName("Victoria");
	players[6]->setName("Tumbo");

	Team *soccerTeam, *basketballTeam;
	soccerTeam = new Team();
	basketballTeam = new Team();

	for (int i=0; i < 7; i++) {
		basketballTeam->players[i] = players[i];
	}

	for (int i=1; i < 6; i++) {
		soccerTeam->players[i-1] = players[i];
	}

	cout << "Basketball team" << endl;
	basketballTeam->printAllNames();
	cout << endl << "Soccer team" << endl;
	soccerTeam->printAllNames();

	cout << endl << "Deleting basketball team" << endl;
	delete basketballTeam;
	basketballTeam->printAllNames();

	cout << endl << "Deleting soccer team" << endl;
	delete soccerTeam;
	for (int i=0; i < 7; i++) {
		cout << players[i]->getName() << endl;
		delete players[i];
	}
}