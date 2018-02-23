#include "staticclass.h"

int Fun::count = 0;

Fun::Fun() {
	count++;
}

int Fun::getCount() {
	return count;
}
