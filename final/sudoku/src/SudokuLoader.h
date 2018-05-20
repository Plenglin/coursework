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
	string path;
public:
	void setPath(string path);
	vector<SudokuBoard> load();
};

#endif /* SUDOKULOADER_H_ */
