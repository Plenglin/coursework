#pragma once

#include <string>

#include "Birthday.h"

using namespace std;

class People {
	string name;
	Birthday birthday;
public:
	People(string x, Birthday bo);
	void printInfo();
	
};