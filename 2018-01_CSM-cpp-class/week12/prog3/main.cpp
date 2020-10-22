#include "colorpicker.h" 

int main()

{
	cout << "Name: Maxim Yu - Program Name: cube - Date: 03/08/2018" << endl;

	ColorPicker P;
	P.printAllColors();

	cout << "Random Color: " << P.randomColor();

	return 0;

}