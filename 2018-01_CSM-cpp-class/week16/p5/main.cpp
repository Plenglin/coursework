#include <fstream>
#include <iostream>

using namespace std;

class Record {
public:
	string last, first, city, state;
	int zip;

	bool operator >(Record other) {
		return other.last > last;
	}
	
	bool operator <(Record other) {
		return other.last < last;
	}
	
};

template <class T>
void swap(T* a, T* b) {
	T tmp = *a;
	*a = *b;
	*b = tmp;
}

template <class T>
void quicksort(string compare[], T toSwap[], int start, int end) {
	if (start >= end) {
		return;
	}
	string pivotVal = compare[end];
	int i = start - 1;
	for (int j = start; j <= end - 1; j++) {
		if (compare[j] <= pivotVal) {
			i++;
			swap(&compare[i], &compare[j]);
			swap(&toSwap[i], &toSwap[j]);
		}
	}
	int pivot = i + 1;
	swap(&compare[pivot], &compare[end]);
	swap(&toSwap[pivot], &toSwap[end]);
	quicksort(compare, toSwap, start, pivot - 1);
	quicksort(compare, toSwap, pivot + 1, end);
}

void getRecords(int count, Record arr[]) {
	ifstream file;
	file.open("dudes.txt");
	for (int i=0; i < count; i++) {
		Record r;
		file >> r.last;
		file >> r.first;
		file >> r.city;
		file >> r.state;
		file >> r.zip;
		arr[i] = r;
	}
	file.close();
}

int main() {
	cout << "Name: Maxim Yu - Program Name: part 5 - Date: 05/06/2018" << endl;
	Record arr[10];
	string lns[10];
	getRecords(10, arr);
	for (int i=0; i < 10; i++) {
		lns[i] = arr[i].last;
	}
	quicksort(lns, arr, 0, 9);
	ofstream file;
	file.open("sorteddudes.txt");
	for (int i=0; i < 10; i++) {
		Record r = arr[i];
		file << r.last << " ";
		file << r.first << " ";
		file << r.city << " ";
		file << r.state << " ";
		file << r.zip << endl;
	}
	file.close();
}