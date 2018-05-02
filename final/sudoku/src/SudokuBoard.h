/*
 * SudokuBoard.h
 *
 *  Created on: Apr 13, 2018
 *      Author: maxim
 */

#ifndef SUDOKUBOARD_H_
#define SUDOKUBOARD_H_

#include "Subgrid.h"

/**
 * Defines a 9x9 board used to store a Sudoku state.
 */
class SudokuBoard {
	int board[9][9];
public:
	SudokuBoard();

	/**
	 * Populates the grid with the given values.
	 */
	void populateWith(int values[81]);

	/**
	 * Gets the number at a position.
	 */
	int get(int x, int y);

	/**
	 * Sets the number at a position.
	 */
	void set(int x, int y, int val);

	/**
	 * Returns true if none of the cells have a value of 0 (unfilled)
	 */
	bool isFilled();

	/**
	 * Returns true if all the rows of the board have unique numbers and all the
	 * columns of the board have unique numbers and if the subgrids have
	 * unique numbers on the precondition that none of the cells are 0
	 */
	bool isValid();

	/**
	 * Fills the provided array with the 9 subgrids of this board.
	 */
	void getSubgrids(Subgrid grids[9]);

	/**
	 * Returns the subgrid of the subgrid coordinates.
	 */
	Subgrid getSubgrid(int i, int j);
};

#endif /* SUDOKUBOARD_H_ */
