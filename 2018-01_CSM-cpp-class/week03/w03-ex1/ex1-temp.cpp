#include <iostream>
using namespace std;

int main() {
	cout << "Do you want:" << endl << "1. Centigrade -> Fahrenheit" << endl << "2. Fahrenheit -> Centigrade" << endl;
	int selection;
	cin >> selection;

	cout << "What temperature? ";
	double temp;
	cin >> temp;

	if (selection == 2) {  // F to C
		cout << (temp - 32) * 5 / 9 << "" << endl;
	} else if (selection == 1) {  // C to F
		cout << temp * 9 / 5 + 32 << "" << endl;
	}
	return 0;
}