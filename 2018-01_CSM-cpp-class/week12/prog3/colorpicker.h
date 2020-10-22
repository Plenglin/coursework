#pragma once
#include <iostream>

using namespace std;

#include <string>

/* srand example */

#include <ctime>

#include <stdio.h>      /* NULL */

#include <stdlib.h>     /* srand, rand */

class ColorPicker {
	string colorArray[7];

	public:
		ColorPicker();
		void printAllColors();
		string randomColor();

};