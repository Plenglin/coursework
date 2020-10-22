#include <iostream>
using namespace std;

class BuckysClass {
public:
	void coolSaying() {
		cout << "preachin to the choir" << endl;
	}
};

int main() {
	cout << "Name: Maxim Yu - Program Name: Bucky 1 - Date: 02102018" << endl;
	BuckysClass buckysObject;
	buckysObject.coolSaying();
	return 0;
}