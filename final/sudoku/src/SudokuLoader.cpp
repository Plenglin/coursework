/*
 * SudokuLoader.cpp
 *
 *  Created on: May 1, 2018
 *      Author: maxim
 */

#include "SudokuLoader.h"

#include <fstream>
#include <string>
#include <iostream>

#include "SudokuBoard.h"

using namespace std;

vector<SudokuBoard> SudokuLoader::loadFromPath(string path) {
	vector<SudokuBoard> out;
	ifstream infile;
	infile.open(path.c_str(), ifstream::in);

	if (infile.is_open()) {
		char c;
		int line[81];
		int index = 0;
		while (infile.good()) {
			c = infile.get();
			if (c == '\n') {
				SudokuBoard b;
				b.populateWith(line);
				out.push_back(b);
				index = 0;
			} else {
				int num = c - '0';
				line[index] = num;
				index++;
			}
		}

		infile.close();
	}

	return out;
}

