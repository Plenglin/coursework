#include <iostream>
#include "prog7.h"

using namespace std;

int main() {

	cout << "Name: Maxim Yu - Program Name: Circle - Date: 02/19/2018" << endl;

	Circle c1(32), c2;
	cout << "c1 Circumference: " << c1.getCircumference() << endl << "Area: " << c1.getArea() << endl << "Diameter: " << c1.getDiameter() << endl;
	cout << "c2 Circumference: " << c2.getCircumference() << endl << "Area: " << c2.getArea() << endl << "Diameter: " << c2.getDiameter() << endl;
	c2.setRadius(0);
	cout << "c2 Circumference: " << c2.getCircumference() << endl << "Area: " << c1.getArea() << endl << "Diameter: " << c1.getDiameter() << endl;

}
