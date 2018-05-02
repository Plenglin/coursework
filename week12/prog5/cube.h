#pragma once

#include "square.h"

class Cube: public Square {
	double height;
public:
	void setHeight(double height);
	double getHeight();

	double getVolume();
	
};
