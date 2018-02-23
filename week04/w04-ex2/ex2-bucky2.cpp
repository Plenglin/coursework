#include <iostream>
#include <string>

using namespace std;

class BuckysClass {
public:
	void setName(string x) {
		name = x;
	}
	string getName() {
		return name;
	}
private:
	string name;
};

int main() {
	cout << "Name: Maxim Yu -  Program Name: Bucky 2 -  Date: 02102018" << endl;
	BuckysClass bo;
	bo.setName("Sir Bucky Wallace");
	cout << bo.getName() << endl;
	return 0;
}