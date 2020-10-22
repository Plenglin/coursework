#include <iostream>
using namespace std;

int main ( ) { // start program
	cout << "Maxim Yu -- 1/24/18" << endl;  // added, as required on all labs
	// Declare and Initialize Variables
	int height = 0;
	int feet = 0;
	int inches = 0;
	// Prompt for height
	cout << "Enter your height in inches: ";
	cin >> height;
	// Calculate Height in feet and inches
	feet = height / 12;
	inches = height % 12;
	//Print out height in feet and inches
	cout << "You are " << feet << " feet and " << inches << " inches" << endl;
	//system("pause"); // Mac user comment out this line 
	return 0;

} // end of program