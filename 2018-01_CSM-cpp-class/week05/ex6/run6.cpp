#include <iostream>
#include <stdlib.h>

#include "prog6.h"

using namespace std;

int main() {

	cout << "Name: Maxim Yu - Program Name: Box - Date: 02/19/2018" << endl;

	Box b1(2, 3, 4);

	cout << "Height = " << b1.getHeight() << endl;
	cout << "Area = " << b1.calcArea() << endl;
	cout << "Volume = " << b1.calcVolume() << endl;

	Box b2;

	b2.setWidth(3);
	b2.setHeight(4);

	cout << "Depth = " << b2.getDepth() << endl;
	cout << "Area = " << b2.calcArea() << endl;
	cout << "Volume = " << b2.calcVolume() << endl;

}
