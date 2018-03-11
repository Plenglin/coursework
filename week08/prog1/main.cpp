#include <iostream>

using namespace std;

class Test {
public:
	void testNullPtr() {
		int *pInt = NULL;
		*pInt = 42;
	}

	void memoryLeak() {
		double *pDouble = new double;
		*pDouble = 3.123456789;
	}

	string* deletePointer() {
		string *pString = new string;
		*pString = "Carlos";
		return pString;
	}

	void testTwoAlias() {
		int *pAlias1 = new int;
		int *pAlias2 = pAlias1;
		*pAlias1 = 42;
		cout << *pAlias1 << endl << *pAlias2 << endl;
	}
	
};

int main() {
	Test tc1;
	
	cout << "Name: Maxim Yu - Program Name: Test - Date: 03/11/2018" << endl;

	/*
	// Test 1
	cout << "Test 1";
	tc1.testNullPtr();*/

	
	// Test 2
	/*
	for (int i=0; i < 1000000000; i++) {
		tc1.memoryLeak();
	}*/

	/*
	// Test 3
	string *pStr = tc1.deletePointer();
	cout << *pStr;*/

	/*
	// Test 4
	tc1.testTwoAlias();*/

	return 0;
}