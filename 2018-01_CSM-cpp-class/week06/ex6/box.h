#include <iostream>
#include <stdlib.h>

using namespace std;

template <class T>
class Box {
	T width, height, depth;

public:

	Box();
	Box(T w, T h, T d);

	T getWidth();
	T getHeight();
	T getDepth();
	void setWidth(T width);
	void setHeight(T height);
	void setDepth(T depth);

	T calcArea();
	T calcVolume();

	bool isValid();
};

template <class T>
Box<T>::Box() {
	width = 0;
	height = 0;
	depth = 0;
}

template <class T>
Box<T>::Box(T w, T h, T d) {
	width = w;
	height = h;
	depth = d;
}

template <class T>
T Box<T>::getWidth() {
	return width;
}
template <class T>
T Box<T>::getHeight() {
	return height;
}
template <class T>
T Box<T>::getDepth() {
	return depth;
}

template <class T>
void Box<T>::setWidth(T width) {
	this->width = width;
}
template <class T>
void Box<T>::setHeight(T height) {
	this->height = height;
}
template <class T>
void Box<T>::setDepth(T depth) {
	this->depth = depth;
}

template <class T>
T Box<T>::calcArea() {
	T value = 2 * (width * height + width * depth + height * depth);
	if (!isValid()) {  // Ensure that the dimensions are positive, stop program if not
		cout << "Error: one of the dimensions is not positive! Exiting." << endl;
		exit(1);
	}
	return value;
}

template <class T>
T Box<T>::calcVolume() {
	T value = width * height * depth;
	if (!isValid()) {  // Ensure that the dimensions are positive, stop program if not
		cout << "Error: one of the dimensions is not positive! Exiting." << endl;
		exit(1);
	}
	return value;
}

template <class T>
bool Box<T>::isValid() {
	return width > 0 && height > 0 && depth > 0;
}

