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
	bool exists[10];
	for (int i=0; i < 3; i++) {
		for (int j=0; j < 3; j++) {
			int cell = get(i, j);
			if (exists[cell]) {
				return false;
			}
			exists[cell] = true;
		}
	}
	return true;
}
