#include <iostream>
#include "animals.h"

using namespace std;

int main() {
	cout << "Name: Maxim Yu - Program Name: part 3 - Date: 04/22/2018" << endl;
	Animal **ptrBaseAnimal = new Animal*[4];

	ptrBaseAnimal[0] = new Cat;
	ptrBaseAnimal[1] = new Dog;
	ptrBaseAnimal[2] = new Elephant;
	ptrBaseAnimal[3] = new Mouse;
	for (int i=0; i < 4; i++) {
		ptrBaseAnimal[i]->animalSound();
	}
	return 0;
}