#include "People.h"

#include <iostream>

using namespace std;

People::People(string x, Birthday bo) : name(x), birthday(bo) {
}

void People::printInfo() {
	cout << name << " was born on ";
	birthday.printDate();
}