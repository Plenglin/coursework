#include "Player.h"

Player::Player() {
	name = "Unknown";
}

Player::Player(string name) {
	this->name = name;
}

void Player::setName(string name) {
	this->name = name;
}

string Player::getName() {
	return name;
}
