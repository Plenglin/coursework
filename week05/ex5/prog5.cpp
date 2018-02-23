#include "prog5.h"

Rectangle::Rectangle() {
	set_values(1, 1);
}

Rectangle::Rectangle(int w, int h) {
	set_values(w, h);
}

void Rectangle::set_values(int x, int y) {
   width = x;
   height = y;
}

int Rectangle::area() {
   int answer = width * height;
   return answer;
}
