#pragma once

#include <iostream>

#include "Date.h"

using namespace std;

class Person {
	Date birthday;
	string firstName, lastName;
public:
	Person();
	Person(string first, string last, Date birthday);
	
	string getFirstName();
	void setFirstName(string fn);
	string getLastName();
	void setLastName(string ln);

	Date* getBirthday();
};