#include <iostream>
#include "athing.h"

using namespace std;

int main() {

	cout << "Name: Maxim Yu - Program Name: A Thing - Date: 02/26/2018" << endl;

	AThing<double> thing1(3.2);
	AThing<int> thing2(32);
	AThing<int> thing3(6.3);
	cout << thing1.getWeight() << endl << thing2.getWeight() << endl << thing3.getWeight() << endl;
	return 0;
}
