#include <stdlib.h>
#include <iostream>

using namespace std;

template <class T>
class ColorPicker {
	T colors[7];

public:
	ColorPicker();
	ColorPicker(T inColors[7]);

	void setElement(int index, T color);
	void printColors();
	T getRandomColor();
};

template <class T>
ColorPicker<T>::ColorPicker() {
	
}

template <class T>
ColorPicker<T>::ColorPicker(T inColors[7]) {
	for (int i=0; i < 7; i++) {
		setElement(i, inColors[i]);
	}
}

template <class T>
void ColorPicker<T>::setElement(int index, T color) {
	colors[index] = color;
}

template <class T>
void ColorPicker<T>::printColors() {
	for (int i=0; i < 7; i++) {
		cout << colors[i] << endl;
	}
}

template <class T>
T ColorPicker<T>::getRandomColor() {
	return colors[rand() % 7];
}
