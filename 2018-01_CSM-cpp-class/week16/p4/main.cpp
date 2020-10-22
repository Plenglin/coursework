#include <fstream>
#include <iostream>

#define TEST_SCORES_COUNT 10

using namespace std;

void getUserInput() {
	cout << "Name: Maxim Yu - Program Name: part 4 - Date: 05/06/2018" << endl;
	ofstream file;
	string buf;

	file.open("testScores.txt");

	for (int i=1; i <= TEST_SCORES_COUNT; i++) {
		cout << "Test #" << i << ": ";
		cin >> buf;
		file << buf << endl;
	}
}

void getTestScores() {
	ifstream file;
	int scores[10];

	file.open("testScores.txt");
	for (int i=0; i < TEST_SCORES_COUNT; i++) {
		file >> scores[i];
	}

	int sum = 0;
	int min = 0xffff;
	int max = 0;
	for (int i=0; i < TEST_SCORES_COUNT; i++) {
		int score = scores[i];
		sum += score;
		if (score < min) {
			min = score;
		} else if (score > max) {
			max = score;
		}
	}
	double avg = double(sum) / TEST_SCORES_COUNT;

	cout << "Average: " << avg << endl;
	cout << "Min: " << min << endl;
	cout << "Max: " << max << endl;
}

int main() {
	getUserInput();
	getTestScores();
}
