#include <iostream>
#include <stdlib.h>

using namespace std;

class ColorPicker {
	string colors[7];

public:
	void setElement(int index, string color) {
		colors[index] = color;
	}

	void printColors() {
		for (int i=0; i < 7; i++) {
			cout << colors[i] << endl;
		}
	}

	string getRandomColor() {
		return colors[rand() % 7];
	}
};

int main() {
	cout << "Name: Maxim Yu -  Program Name: Pick Color -  Date: 02102018" << endl;

	ColorPicker picker;

	picker.setElement(0, "red");
	picker.setElement(1, "orange");
	picker.setElement(2, "yellow");
	picker.setElement(3, "green");
	picker.setElement(4, "blue");
	picker.setElement(5, "indigo");
	picker.setElement(6, "violet");

	cout << "Colors:" << endl;
	picker.printColors();

	cout << "Random color: " << picker.getRandomColor() << endl;
}