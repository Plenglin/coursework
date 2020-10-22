#include <iostream>

using namespace std;

int squareInteger(int x) {
	return x * x;
}

double squareDouble(double d) {
	return d * d;
}

float squareFloat(float f) {
	return f * f;
}

int main() {
	int i = 6;
	double d = 6.9;
	float f = 9.1;

	cout << squareInteger(i) << endl;
	cout << squareDouble(i) << endl;
	cout << squareFloat(i) << endl;
	
	cout << squareInteger(d) << endl;
	cout << squareDouble(d) << endl;
	cout << squareFloat(d) << endl;
	
	cout << squareInteger(f) << endl;
	cout << squareDouble(f) << endl;
	cout << squareFloat(f) << endl;

	return 0;
	
}
