#include <iostream>
#include <stdlib.h>

#include "box.h"

using namespace std;

int main() {

	cout << "Name: Maxim Yu - Program Name: Box - Date: 02/26/2018" << endl;

	Box<int> b1(2, 3, 4);

	cout << "Height = " << b1.getHeight() << endl;
	cout << "Area = " << b1.calcArea() << endl;
	cout << "Volume = " << b1.calcVolume() << endl;

	Box<double> b2(3.1, 56.2, 0.1);

	cout << "Depth = " << b2.getDepth() << endl;
	cout << "Area = " << b2.calcArea() << endl;
	cout << "Volume = " << b2.calcVolume() << endl;

}
