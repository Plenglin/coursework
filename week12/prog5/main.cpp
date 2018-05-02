#include <iostream>

#include "square.h"
#include "cube.h"

using namespace std;


int main() {

	cout << "Name: Maxim Yu - Program Name: cube - Date: 03/08/2018" << endl;

	Cube a, b, c;

	a.setWidth(30);
	a.setHeight(32);
	a.setLength(13);
	cout << "Cube A: " << a.getVolume() << endl;

	b.setWidth(523);
	b.setHeight(598);
	b.setLength(3.2);
	cout << "Cube b: " << b.getVolume() << endl;

	c.setWidth(8493);
	c.setHeight(243);
	c.setLength(1244);
	cout << "Cube c: " << c.getVolume() << endl;

	return 0;
}