#include <iostream>
using namespace std;

int main() {
	cout << "Maxim Yu -- 1/24/18" << endl;

	double num1, num2, num3;

	// User input
	cout << "First number: ";
	cin >> num1;
	cout << "Second number: ";
	cin >> num2;
	cout << "Third number: ";
	cin >> num3;

	// Print results
	cout << num1 * (num2 + num3) << " is your result." << endl;
}