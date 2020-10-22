#include <iostream>

#include "Date.h"
#include "Person.h"

using namespace std;

int main() {

	cout << "Name: Maxim Yu - Program Name: Person - Date: 03/05/2018" << endl;

	Person person1;
	person1.setFirstName("Henry");
	person1.setLastName("Tudor");
	Date* p1bd = person1.getBirthday();
	p1bd->setYear(1491);
	p1bd->setMonth(6);
	p1bd->setDay(28);
	cout << "Person1 is named " << person1.getFirstName() << " " << person1.getLastName() << endl;
	cout << "He was born on " << p1bd->getFormatted() << endl;

	Person person2("Maria", "Poppleton", Date(2, 05, 1953));
	cout << "Person2 is named " << person2.getFirstName() << " " << person2.getLastName() << endl;
	cout << "She was born on " << person2.getBirthday()->getFormatted() << endl;

}