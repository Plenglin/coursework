#include <iostream>
#include "animals.h"

using namespace std;

void polyNoise(Animal *ptrBase) {
	cout << ptrBase->sound << endl;
}

int main() {
	Cat c1;
	Dog d1;
	Elephant e1;
	Mouse m1;

	cout << "Name: Maxim Yu - Program Name: part 3 - Date: 04/22/2018" << endl;

	polyNoise(&c1);
	polyNoise(&d1);
	polyNoise(&e1);
	polyNoise(&m1);
	return 0;
}