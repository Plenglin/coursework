#include <iostream>

#include "prog5.h"

using namespace std;

int main () { 
  cout << "Name: Maxim Yu -  Program Name: Rectangle -  Date: 02/19/2018" << endl;

  Rectangle rect1(5, 6), rect2;
  cout << "r1 area: " << rect1.area() << endl;
  
  cout << "r2 area: " << rect2.area() << endl; 
  rect2.set_values(3, 4);
  cout << "r2 area: " << rect2.area() << endl; 
  return 0; 
}
