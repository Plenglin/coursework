/*
 * Subgrid.cpp
 *
 *  Created on: Apr 13, 2018
 *      Author: maxim
 */

#include "Subgrid.h"


int Subgrid::get(int i, int j) {
	return grid[i][j];
}

void Subgrid::set(int i, int j, int val) {
	grid[i][j] = val;
}


bool Subgrid::isValid() {
	bool exists[10] = {true, false, false, false, false, false, false, false, false, false};
	for (int x=0; x < 3; x++) {
		for (int y=0; y < 3; y++) {
			int cell = get(x, y);
			if (exists[cell]) {  // is there a duplicate?
				return false;
			}
			exists[cell] = true;  // mark this number as "seen"
		}
	}
	return true;
}
