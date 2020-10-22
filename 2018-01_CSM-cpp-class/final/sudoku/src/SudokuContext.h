/*
 * SudokuContext.h
 *
 *  Created on: May 14, 2018
 *      Author: maxim
 */

#ifndef SUDOKUCONTEXT_H_
#define SUDOKUCONTEXT_H_

#include "SudokuBoard.h"
#include "SudokuParameters.h"

using namespace std;

class SudokuContext {
public:
	SudokuParameters constants;
	int run();
	int askForDifficulty();
	void play(SudokuBoard board);
};

#endif /* SUDOKUCONTEXT_H_ */
