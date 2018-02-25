#include <iostream>
#include <stdlib.h>

#include "box.h"

using namespace std;

int main() {

	cout << "Name: Maxim Yu - Program Name: Box - Date: 02/26/2018" << endl;

	Box<int> b1(2, 3, 4);
	Box<long> b2(2, 3, 4);
	Box<double> b3(3.1, 56.2, 0.1);
	Box<float> b4(3.1, 56.2, 0.1);

	cout << "Int box" << endl;
	cout << "Height = " << b1.getHeight() << endl;
	cout << "Area = " << b1.calcArea() << endl;
	cout << "Volume = " << b1.calcVolume() << endl;

	cout << endl << "Long box" << endl;
	cout << "Height = " << b2.getHeight() << endl;
	cout << "Area = " << b2.calcArea() << endl;
	cout << "Volume = " << b2.calcVolume() << endl;

	cout << endl << "double box" << endl;
	cout << "Height = " << b3.getHeight() << endl;
	cout << "Area = " << b3.calcArea() << endl;
	cout << "Volume = " << b3.calcVolume() << endl;

	cout << endl << "float box" << endl;
	cout << "Height = " << b4.getHeight() << endl;
	cout << "Area = " << b4.calcArea() << endl;
	cout << "Volume = " << b4.calcVolume() << endl;

}
