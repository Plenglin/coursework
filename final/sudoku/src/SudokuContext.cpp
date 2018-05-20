/*
 * SudokuContext.cpp
 *
 *  Created on: May 14, 2018
 *      Author: maxim
 */

#include "SudokuContext.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "SudokuBoard.h"
#include "SudokuLoader.h"

int SudokuContext::run() {
	while (true) {
		int diff = askForDifficulty();

		string diffName = constants.difficultyLocalizations[diff - 1];
		cout << "Getting a " << diffName << " level..." << std::endl;

		std::string path = constants.dataPath + "sudoku" + std::to_string(diff - 1) + ".txt";
		auto loader = SudokuLoader();
		loader.setPath(path);
		auto data = loader.load();
		SudokuBoard board = data.at(rand() % data.size());

		play(board);
		std::cout << "You win!" << endl;
	}
	return 0;
}

void SudokuContext::play(SudokuBoard board) {
	do {
		board.display();
		cout << "Input in this format: x y set" << std::endl;
		int x, y, val;
		std::cin >> x;
		std::cin >> y;
		std::cin >> val;
		if (1 <= x && x <= 9 && 1 <= y && y <= 9 && 0 <= val && val <= 9) {
			bool result = board.setPlayer(x - 1, y - 1, val);
			if (!result) {
				std::cout << "Error: (" << x << ", " << y << ") is immutable." << std::endl;
			}
		} else {
			std::cout << "Error: numbers out of range. x and y must be in [1, 9]. set must be in [1, 9], or 0 to clear cell." << std::endl;
		}
	} while (!(board.isFilled() && board.isValid()));
}

int SudokuContext::askForDifficulty() {
	int diff;
	do {
		cout << "Welcome to Sudoku! Select your difficulty:" << endl;
		for (int i=1; i <= 5; i++) {
			cout << i << ". " << constants.difficultyLocalizations[i - 1] << endl;
		}
		cout << "Difficulty: ";

		cin >> diff;
		string diffName = constants.difficultyLocalizations[diff - 1];

		cout << "You selected " << diff << " (" << diffName << ")" << ". Confirm? (y/n) ";

		string confirm;
		do {
			cin >> confirm;
		} while (confirm != "n" && confirm != "y");
		if (confirm == "y") {
			return diff;
		}
	} while (true);
}
