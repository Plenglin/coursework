#include <iostream>
using namespace std;

int main() {

	cout << "Maxim Yu -- 1/24/18" << endl;

	int n;

	// Accept user input
	cout << "Enter a number: ";
	cin >> n;

	// Calculate the cube
	int cube = n * n * n;

	// Display the output
	cout << "The cube of " << n << " is " << cube << endl;

	return 0;
}