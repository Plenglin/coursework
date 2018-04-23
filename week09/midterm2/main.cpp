#include <iostream>

#include "Whole.h"
#include "Part.h"

using namespace std;

int main() {
	Whole w1, w2("w2");
	cout << "w1: " << w1.getName() << endl;
	cout << "w2: " << w2.getName() << endl;
}