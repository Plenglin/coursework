#include <iostream>
#include "SudokuBoard.h"

#include "SudokuLoader.h"

using namespace std;

int main(void) {
	SudokuLoader loader;
	vector<SudokuBoard> board1 = loader.loadFromPath("/home/maxim/Documents/cpp-class/final/sudoku/data/sudoku0.txt");
    vector<SudokuBoard>::iterator iter;
    cout << board1.size() << endl;
    for (iter=board1.begin(); iter != board1.end(); iter++) {
    	cout << iter->get(0, 0) << endl;
    }
	return 0;
}
