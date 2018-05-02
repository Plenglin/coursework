#include "cube.h"

void Cube::setHeight(double height) {
	this->height = height;
}

double Cube::getHeight() {
	return height;
}

double Cube::getVolume() {
	return height * getLength() * getWidth();
}
