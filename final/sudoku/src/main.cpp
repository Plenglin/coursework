
#include <string>

#include "SudokuContext.h"
#include "SudokuParameters.h"

using namespace std;

// Constants
const string BOARD_PATH = "data/";

int main(void) {
	SudokuContext ctx;
	ctx.constants.dataPath = BOARD_PATH;
	return ctx.run();
}
