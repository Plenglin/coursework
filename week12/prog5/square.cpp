#include "square.h"

void Square::setLength(double length) {
	this->length = length;
}
double Square::getLength() {
	return length;
}

void Square::setWidth(double width) {
	this->width = width;
}
double Square::getWidth() {
	return width;
}

double Square::getArea() {
	return width * length;
}
