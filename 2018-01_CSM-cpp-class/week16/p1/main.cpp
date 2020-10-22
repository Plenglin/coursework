#include <fstream>
#include <string>
#include <iostream>

using namespace std;

int main() {
	cout << "Name: Maxim Yu - Program Name: part 1 - Date: 05/06/2018" << endl;
	ifstream file;
	string line, fn, ln, age;
	file.open("firstLastAge.txt");
	while (!file.eof()) {
		file >> fn;
		file >> ln;
		file >> age;
		getline(file, line);
		cout << "first: " << fn << "; last: " << ln << "; age: " << age << endl;
	}
	file.close();
}