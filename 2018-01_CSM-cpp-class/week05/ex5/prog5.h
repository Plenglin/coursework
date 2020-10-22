#include <iostream>
using namespace std;

class Rectangle {
    int width, height;
  public:
  	Rectangle(int width, int height);
  	Rectangle();
    void set_values(int,int);
    int area();
};
