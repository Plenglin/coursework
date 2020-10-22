#include <iostream>

#include "Person.h"

using namespace std;

Person::Person() : firstName("None"), lastName("None") {
}

Person::Person(string fn, string ln, Date bd) : 
	firstName(fn), lastName(ln), birthday(bd) {
}

string Person::getFirstName() {
	return firstName;
}

void Person::setFirstName(string fn) {
	firstName = fn;
}

string Person::getLastName() {
	return lastName;
}

void Person::setLastName(string ln) {
	lastName = ln;
}

Date* Person::getBirthday() {
	return &birthday;
}
