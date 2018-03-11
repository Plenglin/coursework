#include <iostream>

using namespace std;

class MyClass {
	int x;
public:
	MyClass() {
		x = 0;
	}
	MyClass(int x) {
		this->x = x;
	}
	int getX() {
		return x;
	}
	void setX(int x) {
		this->x = x;
	}
};

int main() {
	
	cout << "Name: Maxim Yu - Program Name: MyClass - Date: 03/11/2018" << endl;

	MyClass c1;
	c1.setX(42);
	cout << c1.getX() << endl;

	MyClass *ptrMyClass = NULL;
	ptrMyClass = &c1;

	cout << "Used arrow - value of x is " << ptrMyClass->getX() << endl;
	ptrMyClass->setX(104);
	cout << "Used arrow - value of x is " << ptrMyClass->getX() << endl;
	return 0;
}