#include <fstream>
#include <string>
#include <iostream>

using namespace std;

int main() {
	cout << "Name: Maxim Yu - Program Name: part 2 - Date: 05/06/2018" << endl;
	ifstream file;
	string line;
	string fn[5], ln[5];
	int age[5];
	file.open("firstLastAge.txt");
	for (int i=0; i < 5; i++) {
		file >> fn[i];
		file >> ln[i];
		file >> age[i];
		getline(file, line);
	}
	file.close();

	int oIndex = 0;
	int oValue = 0;
	int yIndex = 0;
	int yValue = 0xffff;
	for (int i=1; i < 5; i++) {
		int a = age[i];
		if (a > oValue) {
			oIndex = i;
			oValue = a;
		} else if (a < yValue) {
			yIndex = i;
		}
	}

	cout << "oldest: " << fn[oIndex] << " " << ln[oIndex] << endl;
	cout << "youngest: " << fn[yIndex] << " " << ln[yIndex] << endl;
}