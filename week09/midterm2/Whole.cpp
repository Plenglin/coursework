#include "Whole.h"

using namespace std;

Whole::Whole() {
	wholeName = "None";
	cout << "Called whole default constructor: " << wholeName << endl;
}

Whole::Whole(string name) {
	cout << "In whole parm constructor" << endl;
	wholeName = name;
}

string Whole::getName() {
	return wholeName;
}

Whole::~Whole() {
	cout << "called destructor" << endl;
}
