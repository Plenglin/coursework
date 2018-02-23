#include "prog2.h"

#include <string>

using namespace std;

BuckysClass::BuckysClass() {
	name = "";
}

BuckysClass::BuckysClass(string name) {
	setName(name);
}

void BuckysClass::setName(string x) {
	this->name = name;
}

string BuckysClass::getName() {
	return name;
}