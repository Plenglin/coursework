#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

template <class T>
class Circle {

	T radius;

public:
	Circle();
	Circle(T radius);
	void setRadius(T radius);

	T getCircumference();
	T getArea();
	T getDiameter();

};

template <class T>
Circle<T>::Circle() {
	setRadius(1);
}

template <class T>
Circle<T>::Circle(T radius) {
	setRadius(radius);
}

template <class T>
void Circle<T>::setRadius(T radius) {
	if (radius <= 0) {
		cout << "Error: radius is not positive! Exiting." << endl;
		exit(1);
	}
	this->radius = radius;
}

template <class T>
T Circle<T>::getCircumference() {
	return M_PI * getDiameter();
}

template <class T>
T Circle<T>::getArea() {
	return M_PI * radius * radius;
}

template <class T>
T Circle<T>::getDiameter() {
	return radius * 2;
}