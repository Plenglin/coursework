#include <iostream>
#include "animals.h"

using namespace std;

int main() {
	Cat c1;
	Dog d1;
	Elephant e1;
	Mouse m1;

	cout << "Name: Maxim Yu - Program Name: part 3 - Date: 04/22/2018" << endl;

	int input;
	cout << "1. Cat" << endl << "2. Elephant" << endl << "3. Dog" << endl << "4. Mouse" << endl;
	cin >> input;

	switch (input) {
		case 1:
		c1.animalSound();
		break;
		case 2:
		e1.animalSound();
		break;
		case 3:
		d1.animalSound();
		break;
		case 4:
		m1.animalSound();
		break;
	}
	return 0;
}