#include "colorpicker.h"
#include <iostream>

using namespace std;

int main() {
	int intColors[7] = {5, 3, 2, 5, 1, 7, 1};
	ColorPicker<string> cp1;
	ColorPicker<int> cp2(intColors);

	cout << cp1.getRandomColor() << endl;
	cout << cp2.getRandomColor() << endl;

	cout << "String picker" << endl;
	cp1.printColors();
	cout << "Int picker" << endl;
	cp2.printColors();

}