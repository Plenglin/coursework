/*
 * Subgrid.h
 *
 *  Created on: Apr 13, 2018
 *      Author: maxim
 */

#ifndef SUBGRID_H_
#define SUBGRID_H_

class Subgrid {
	int grid[3][3];
public:
	int get(int i, int j);

	void set(int i, int j, int val);

	/**
	 * Returns true if all cells have unique members on the precondition
	 * that none of the cells have numbers outside of the interval [1, 9].
	 */
	bool isValid();
};

#endif /* SUBGRID_H_ */
