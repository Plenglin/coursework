#include <iostream>

using namespace std;


class ColorPicker {
	string colors[7];

public:
	ColorPicker();
	ColorPicker(string inColors[7]);

	void setElement(int index, string color);
	void printColors();
	string getRandomColor();
};
