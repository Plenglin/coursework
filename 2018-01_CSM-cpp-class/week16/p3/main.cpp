#include <fstream>
#include <iostream>

using namespace std;

int main() {
	cout << "Name: Maxim Yu - Program Name: part 3 - Date: 05/06/2018" << endl;
	string buf;
	int times;
	cout << "Number of names? ";
	cin >> times;

	ofstream file;
	file.open("savedNames.txt");
	for (int i=1; i <= times; i++) {
		cout << "Name #" << i << ": ";
		cin >> buf;
		file << buf << endl;
	}
	file.close();
}