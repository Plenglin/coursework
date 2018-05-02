/*
 * SudokuBoard.cpp
 *
 *  Created on: Apr 13, 2018
 *      Author: maxim
 */

#include "SudokuBoard.h"

SudokuBoard::SudokuBoard() {
	// TODO Auto-generated constructor stub

}

void SudokuBoard::populate() {

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
	for (int x = 0; x < 9; x++) {
		bool existing[10]; // Use a boolean array as a lookup-table
		existing[0] = true; // Mark 0 as existing to end function when empty is found
		for (int y = 0; y < 9; y++) {
			if (existing[get(x, y)]) {
				return false;
			}
		}
	}
	// Check rows for unique and non-empty cells
	for (int y = 0; y < 9; y++) {
		bool existing[10];
		existing[0] = true;
		for (int x = 0; x < 9; x++) {
			if (existing[get(x, y)]) {
				return false;
			}
		}
	}
	// Check subgrids for unique and non-empty cells
}

int SudokuBoard::get(int x, int y) {
	return board[x][y];
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
