#include <iostream>
#include <stdlib.h>

using namespace std;

class Box {
	double width, height, depth;

public:

	Box();

	// Inlined getters and setters
	double getWidth() {
		return width;
	}
	double getHeight() {
		return height;
	}
	double getDepth() {
		return depth;
	}
	void setWidth(double width) {
		this->width = width;
	}
	void setHeight(double height) {
		this->height = height;
	}
	void setDepth(double depth) {
		this->depth = depth;
	}

	double calcArea();
	double calcVolume();

	bool isValid();
};

Box::Box() {
	width = 0;
	height = 0;
	depth = 0;
}

double Box::calcArea() {
	double value = 2 * (width * height + width * depth + height * depth);
	if (!isValid()) {  // Ensure that the dimensions are positive, stop program if not
		cout << "Error: one of the dimensions is not positive! Exiting." << endl;
		exit(1);
	}
	return value;
}

double Box::calcVolume() {
	double value = width * height * depth;
	if (!isValid()) {  // Ensure that the dimensions are positive, stop program if not
		cout << "Error: one of the dimensions is not positive! Exiting." << endl;
		exit(1);
	}
	return value;
}

bool Box::isValid() {
	return width > 0 && height > 0 && depth > 0;
}


int main() {

	cout << "Name: Maxim Yu - Program Name: Box - Date: 02102018" << endl;

	// Box 1 - Test set functions, Volume, getHeight and area functions

	Box B1;    // HINT MAKE A Default constructor or set functions...
	B1.setWidth(2);
	B1.setHeight(3);
	B1.setDepth(4);

	cout << "Height = " << B1.getHeight() << endl;
	cout << "Area = " << B1.calcArea() << endl;
	cout << "Volume = " << B1.calcVolume() << endl;

	// Box 2 - Test zero value error for calc Area and Volume of sides functions

	Box B2;

	B2.setWidth(3);
	B2.setHeight(4);

	cout << "Depth = " << B2.getDepth() << endl;
	cout << "Area = " << B2.calcArea() << endl;
	cout << "Volume = " << B2.calcVolume() << endl;

}

