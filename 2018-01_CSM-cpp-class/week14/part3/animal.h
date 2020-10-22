#include <iostream>

using namespace std;

class Animal {
protected:
	string name;
public:
	string sound;
    virtual void animalSound() {
        cout << name << " says " << sound << endl;
    }
};