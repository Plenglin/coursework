#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "prog7.h"

using namespace std;

Circle::Circle() {
	setRadius(1);
}

Circle::Circle(double radius) {
	setRadius(radius);
}

void Circle::setRadius(double radius) {
	if (radius <= 0) {
		cout << "Error: radius is not positive! Exiting." << endl;
		exit(1);
	}
	this->radius = radius;
}

double Circle::getCircumference() {
	return M_PI * getDiameter();
}

double Circle::getArea() {
	return M_PI * radius * radius;
}

double Circle::getDiameter() {
	return radius * 2;
}