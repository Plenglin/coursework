#include "colorpicker.h"
#include <iostream>

using namespace std;

int main() {
	cout << "Name: Maxim Yu - Program Name: Color Picker - Date: 02/26/2018" << endl;

	int intColors[7] = {5, 3, 2, 5, 1, 7, 1};
	string stringColors[7] = {"r", "as", "fd", "pink", "ffffff", "gah", "green"};
	ColorPicker<string> cp1(stringColors);
	ColorPicker<int> cp2(intColors);

	cout << cp1.getRandomColor() << endl;
	cout << cp2.getRandomColor() << endl;

	cout << endl << "String picker" << endl;
	cp1.printColors();
	cout << endl << "Int picker" << endl;
	cp2.printColors();

}