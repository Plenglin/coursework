#include <iostream>
#include <string>

using namespace std;

class BuckysClass {
public:
	BuckysClass();
	BuckysClass(string name);
	void setName(string x);
	string getName();
private:
	string name;
};

