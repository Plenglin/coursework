// classes example
#include <iostream>
using namespace std;

class Rectangle {
    int width, height;
  public:
    void set_values (int,int);
    int area() {
       int answer = width * height;
       return answer;
    }
};

void Rectangle::set_values (int x, int y) {
   width = x;
   height = y;
}

int main () {  // Use this driver program
  cout << "Name: Maxim Yu -  Program Name: Rectangle -  Date: 02102018" << endl;

  // Use set_values function to set values 
  Rectangle rect1;
  rect1.set_values(5, 6);
  cout << "area: " << rect1.area() << endl;
  
  // Use set_values function to set values 
  Rectangle rect2;
  rect2.set_values(3, 4);
  cout << "area: " << rect2.area() << endl; 
  return 0; 
}
