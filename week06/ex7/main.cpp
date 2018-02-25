#include <iostream>
#include "circle.h"

using namespace std;

int main() {

	cout << "Name: Maxim Yu - Program Name: Circle - Date: 02/26/2018" << endl;

	Circle<int> c1(32);
	Circle<double> c2(3.1);
	Circle<long> c3(32);
	Circle<float> c4(3.1);
	cout << "int Circumference: " << c1.getCircumference() << endl << "Area: " << c1.getArea() << endl << "Diameter: " << c1.getDiameter() << endl;
	cout << "double Circumference: " << c2.getCircumference() << endl << "Area: " << c2.getArea() << endl << "Diameter: " << c2.getDiameter() << endl;
	cout << "long Circumference: " << c1.getCircumference() << endl << "Area: " << c1.getArea() << endl << "Diameter: " << c1.getDiameter() << endl;
	cout << "float Circumference: " << c2.getCircumference() << endl << "Area: " << c2.getArea() << endl << "Diameter: " << c2.getDiameter() << endl;

}
