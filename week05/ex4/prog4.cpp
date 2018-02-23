#include <iostream>
#include <stdlib.h>

#include "prog4.h"

using namespace std;

ColorPicker::ColorPicker() {
	for (int i=0; i < 7; i++) {
		colors[i] = "brown";
	}
}

ColorPicker::ColorPicker(string inColors[7]) {
	for (int i=0; i < 7; i++) {
		setElement(i, inColors[i]);
	}
}

void ColorPicker::setElement(int index, string color) {
	colors[index] = color;
}

void ColorPicker::printColors() {
	for (int i=0; i < 7; i++) {
		cout << colors[i] << endl;
	}
}

string ColorPicker::getRandomColor() {
	return colors[rand() % 7];
}