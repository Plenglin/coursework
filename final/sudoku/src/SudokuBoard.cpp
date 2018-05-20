/*
 * SudokuBoard.cpp
 *
 *  Created on: Apr 13, 2018
 *      Author: maxim
 */

#include "SudokuBoard.h"
#include <vector>
#include <stdlib.h>
#include <iostream>

using namespace std;


void SudokuBoard::populateWith(int values[81]) {
	for (int i=0; i < 9; i++) {
		for (int j=0; j < 9; j++) {
			set(i, j, values[i * 9 + j]);
		}
	}
	updateImmutable();
}

bool SudokuBoard::isFilled() {
	for (int i=0; i < 9; i++) {
		for (int j=0; j < 9; j++) {
			if (get(i, j) == 0) {
				return false;
			}
		}
	}
	return true;
}

bool SudokuBoard::isValid() {
	// Check columns for unique and non-empty cells
	bool existing[10]; // Use a boolean array as a lookup-table for each pass
	existing[0] = true; // Mark 0 as existing to end function when empty is found
	for (int x = 0; x < 9; x++) {
		for (int i=1; i < 10; i++) {  // Clear the lookup-table
			existing[i] = false;
		}
		for (int y = 0; y < 9; y++) {
			if (existing[get(x, y)]) {
				cout << "a";
				return false;
			}
		}
	}
	// Check rows for unique and non-empty cells
	for (int y = 0; y < 9; y++) {
		for (int i=1; i < 10; i++) {
			existing[i] = false;
		}
		for (int x = 0; x < 9; x++) {
			if (existing[get(x, y)]) {
				cout << "b";
				return false;
			}
		}
	}

	// Check subgrids for unique and non-empty cells
	Subgrid subgrids[9];
	getSubgrids(subgrids);
	for (int i=0; i < 9; i++) {
		if (!subgrids[i].isValid()) {
			cout << "c";
			return false;
		}
	}

	return true;
}

int SudokuBoard::get(int x, int y) {
	return board[x][y];
}

void SudokuBoard::set(int x, int y, int val) {
	board[x][y] = val;
}

void SudokuBoard::updateImmutable() {
	for (int x=0; x < 9; x++) {
		for (int y=0; y < 9; y++) {
			immutable[x][y] = get(x, y) != 0;
		}
	}
}

bool SudokuBoard::setPlayer(int x, int y, int val) {
	if (immutable[x][y]) {
		return false;
	}
	set(x, y, val);
	return true;
}

Subgrid SudokuBoard::getSubgrid(int i, int j) {
	int x0 = i / 3;
	int x1 = x0 + 3;
	int y0 = j / 3;
	int y1 = y0 + 3;

	Subgrid out;
	for (int x=x0; x < x1; x++) {
		for (int y=y0; y < y1; y++) {
			out.set(x, y, get(x, y));
		}
	}
	return out;
}

void SudokuBoard::getSubgrids(Subgrid grids[9]) {
	for (int i=0; i < 3; i++) {
		for (int j=0; j < 3; j++) {
			grids[i * 3 + j] = getSubgrid(i, j);
		}
	}
}

void SudokuBoard::display() {
	cout << "  x 1 2 3   4 5 6   7 8 9" << endl
	     << "y ╔══════════════════════" << endl;
	for (int y=0; y < 9; y++) {
		if (y == 3 || y == 6) {
			cout << "  ║───────┼───────┼───────" << endl;
		}
		cout << y + 1 << " ║ ";
		for (int x=0; x < 9; x++) {
			int value = get(x, y);
			if (value == 0) {
				cout << "_ ";
			} else {
				cout << value << " ";
			}
			if (x == 2 || x == 5) {
				cout << "│ ";
			}
		}
		cout << endl;
	}
}
