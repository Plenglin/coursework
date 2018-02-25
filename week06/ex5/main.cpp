#include "colorpicker.h"
#include <iostream>

using namespace std;

int main() {
	cout << "Name: Maxim Yu - Program Name: Color Picker - Date: 02/26/2018" << endl;

	int intColors[7] = {5, 3, 2, 5, 1, 7, 1};
	float floatColors[7] = {5.3, 3.3, 2.3, 5.3, 1.3, 7.3, 1.2};
	double doubleColors[7] = {5, 3, 2, 5, 1, 7, 1};
	long longColors[7] = {5, 3, 2, 5, 1, 7, 1};
	
	ColorPicker<float> cp1(floatColors);
	ColorPicker<int> cp2(intColors);
	ColorPicker<double> cp3(doubleColors);
	ColorPicker<long> cp4(longColors);

	cout << cp1.getRandomColor() << endl;
	cout << cp2.getRandomColor() << endl;
	cout << cp3.getRandomColor() << endl;
	cout << cp4.getRandomColor() << endl;

}