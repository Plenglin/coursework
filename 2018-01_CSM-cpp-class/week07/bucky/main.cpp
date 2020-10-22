#include <iostream>

#include "People.h"
#include "Birthday.h"

using namespace std;

int main() {
	cout << "Name: Maxim Yu - Program Name: Video - Date: 03/05/2018" << endl;
	Birthday bo(12, 28, 1986);
	People buckyRoberts("Bucky the King", bo);
	buckyRoberts.printInfo();
}