/*
 * SudokuLoader.h
 *
 *  Created on: May 1, 2018
 *      Author: maxim
 */

#ifndef SUDOKULOADER_H_
#define SUDOKULOADER_H_

#include <string>
#include <vector>

#include "SudokuBoard.h"

using namespace std;

class SudokuLoader {
public:
	vector<SudokuBoard> loadFromPath(string path);
};

#endif /* SUDOKULOADER_H_ */
