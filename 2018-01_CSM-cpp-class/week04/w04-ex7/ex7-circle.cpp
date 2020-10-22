#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

class Circle {

	double radius;

public:

	Circle(double radius) {
		setRadius(radius);
	}

	void setRadius(double radius) {
		if (radius <= 0) {
			cout << "Error: radius is not positive! Exiting." << endl;
			exit(1);
		}
		this->radius = radius;
	}

	double getCircumference() {
		return M_PI * getDiameter();
	}

	double getArea() {
		return M_PI * radius * radius;
	}

	double getDiameter() {
		return radius * 2;
	}

};

int main() {

	cout << "Name: Maxim Yu - Program Name: Circle - Date: 02102018" << endl;

	double radius;
	cout << "Radius: ";
	cin >> radius;

	Circle circle(radius);
	cout << "Circumference: " << circle.getCircumference() << endl << "Area: " << circle.getArea() << endl << "Diameter: " << circle.getDiameter() << endl;

}