#include <iostream>

#include "Date.h"
#include "Person.h"

using namespace std;

int main() {

	cout << "Name: Maxim Yu - Program Name: Person - Date: 03/11/2018" << endl;

	Person person1, person2("foo", "bar", Date(2, 10, 1024)), *ptrPerson1, *ptrPerson2;

	ptrPerson1 = &person1;
	ptrPerson2 = &person2;

	ptrPerson1->setFirstName("john");
	ptrPerson1->setLastName("smith");
	ptrPerson1->getBirthday()->setMonth(3);
	ptrPerson1->getBirthday()->setDay(4);
	ptrPerson1->getBirthday()->setYear(1519);
	cout << "Person1: " << ptrPerson1->getFirstName() << " " << ptrPerson1->getLastName() << " born on " << ptrPerson1->getBirthday()->getFormatted() << endl;
	cout << "Person2: " << ptrPerson2->getFirstName() << " " << ptrPerson2->getLastName() << " born on " << ptrPerson2->getBirthday()->getFormatted() << endl;

}