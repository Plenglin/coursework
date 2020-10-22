#include <iostream>
#include "prog1.h"

using namespace std;

BuckysClass::BuckysClass() {
	cout << "in first constructor" << endl;
}

BuckysClass::BuckysClass(int h) {
	cout << "in second constructor with param " << h << endl;
}

void BuckysClass::coolSaying() {
	cout << "preachin to the choir" << endl;
}
