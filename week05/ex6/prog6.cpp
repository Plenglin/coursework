#include <iostream>
#include <stdlib.h>
#include "prog6.h"

using namespace std;

Box::Box() {
	width = 0;
	height = 0;
	depth = 0;
}

Box::Box(double w, double h, double d) {
	width = w;
	height = h;
	depth = d;
}

double Box::getWidth() {
	return width;
}
double Box::getHeight() {
	return height;
}
double Box::getDepth() {
	return depth;
}

void Box::setWidth(double width) {
	this->width = width;
}
void Box::setHeight(double height) {
	this->height = height;
}
void Box::setDepth(double depth) {
	this->depth = depth;
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

