#pragma once

#include <iostream>
#include "Part.h"

using namespace std;

class Whole {
	string wholeName;
public:
	Part part;
	Whole();
	Whole(string name);
	string getName();
	~Whole();	
};