/*
 * SudokuConstants.h
 *
 *  Created on: May 14, 2018
 *      Author: maxim
 */

#ifndef SUDOKUPARAMETERS_H_
#define SUDOKUPARAMETERS_H_

#include <string>

using namespace std;

class SudokuParameters {
public:
	string dataPath;
	string difficultyLocalizations[5] = { "Simple", "Easy", "Medium", "Hard",
			"Extremely Hard" };
};

#endif /* SUDOKUPARAMETERS_H_ */
