#include <iostream>
using namespace std;

int main() {

	cout << "Maxim Yu -- 1/24/18" << endl;

	int array[6];

	// Iterate through array, taking user input and putting it in
	cout << "Enter 6 numbers." << endl;
	for (int i=0; i < 6; i++) {
		cout << i + 1 << ". ";  // Arrays are zero-indexed, so we add 1 to the output
		cin >> array[i];
	}

	// Sum the array
	int sum = 0;
	for (int i=0; i < 6; i++) {
		sum += array[i];  // Increment the sum by the array content
	}

	// Take the average of the array. We use a double because averages are frequently fractional
	double average = sum / 6.0;
	cout << "The sum is " << sum << endl << "The average is " << average << endl;

}