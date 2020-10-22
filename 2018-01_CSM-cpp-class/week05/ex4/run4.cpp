#include <iostream>

#include "prog4.h"

int main() {
	cout << "Name: Maxim Yu -  Program Name: Color Picker - Date: 02/19/2018" << endl;

	string colors[7] = {
		"red", "orange", "yellow", "blue", "indigo", "violet"
	};
	ColorPicker picker(colors);

	cout << "Colors:" << endl;
	picker.printColors();

	cout << "Random color: " << picker.getRandomColor() << endl;
}