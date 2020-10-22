#include "Date.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace std;

Date::Date() {
	setMonth(1);
	setDay(1);
	setYear(1900);
}

Date::Date(int m, int d, int y) {
	setMonth(m);
	setYear(y);
	setDay(d);
}

int Date::getMonth() {
	return month;
}

void Date::setMonth(int m) {
	assert(1 <= m && m <= 12);
	month = m;
}

int Date::getDay() {
	return day;
}

void Date::setDay(int d) {
	assert(1 <= d && d <= getDaysInMonth(month, year));
	day = d;
}

int Date::getYear() {
	return year;
}

void Date::setYear(int y) {
	assert(0 <= y);
	year = y;
}

string Date::getFormatted() {
	return to_string(getMonth()) + "/" + to_string(getDay()) + "/" + to_string(getYear());
}

int Date::getDaysInMonth(int m, int y) {
	bool isLeapYear = (y % 4 == 0 && y % 100 != 0 || y % 400 == 0);
	switch (m) {
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:
			return 31;
		case 4: case 6: case 9: case 11:
			return 30;
		case 2:
			return isLeapYear ? 29 : 28;
	}
	return -1;  // error
}
