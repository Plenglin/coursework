#pragma once

#include <iostream>

using namespace std;

class Date {
	int month, day, year;

public:
	Date();
	Date(int m, int d, int y);
	
	int getMonth();
	void setMonth(int m);
	int getDay();
	void setDay(int d);
	int getYear();
	void setYear(int y);

	string getFormatted();

	static int getDaysInMonth(int m, int y);
};
