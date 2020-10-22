#pragma once

#include <string>

using namespace std;

class Player {
	string name;	
public:
	Player();
	Player(string name);

	string getName();
	void setName(string name);	
};