#include <iostream>
using namespace std;

int main() {
	cout << "Maxim Yu -- 1/24/18" << endl;

	int num1, num2;

	// Accept user input
	cout << "First number: ";
	cin >> num1;
	cout << "Second number: ";
	cin >> num2;

	// Calculate the product
	int result = num1 * num2;

	// Display the product
	cout << num1 << " * " << num2 << " = " << result << endl;
	return 0;
}