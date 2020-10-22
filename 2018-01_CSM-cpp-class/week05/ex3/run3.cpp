#include <iostream>

#include "prog3.h"

using namespace std;

int main() {
	cout << "Name: Maxim Yu -  Program Name: A Thing - Date: 02/19/2018" << endl;
	AThing t1, t2(110);

	cout << "t1: " << t1.getWeight() << endl;
	cout << "t2: " << t2.getWeight() << endl;
	t1.setWeight(100);
	cout << "t1: " << t1.getWeight() << endl;
    return 0;

}