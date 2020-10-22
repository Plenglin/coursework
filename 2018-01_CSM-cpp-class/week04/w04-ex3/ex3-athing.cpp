// Sample Class - Print and Study, RUN the code...

#include <iostream>

#include <string>

using namespace std;

// STEP 1 - DEFINE THE new datatype/Class  'aThing'

class aThing {

public:  // Public means that is can be access in step 3 with the dot notation

    double getWeight(void) { return weight; } // public functions  accessible by dot notation

    void setWeight( double inWeight ) { weight = inWeight; }

private: // Private means that it can NOT be accessed with the dot notation, but indirectly with a public function

    double weight; // private variables only accessible by a function

};

int main()

{ // STEP 2 - DECLARATION - USE the new datatype/class 'aThing' in a Declaration statement to create 'ThingOne'.
	cout << "Name: Maxim Yu -  Program Name: A Thing -  Date: 02102018" << endl;

    aThing ThingOne;

    // STEP 3 - Use the OBJECT defined in step 2 - With dot notation

    ThingOne.setWeight(110);

    cout << "Use Function/Method get Weight - "<< "Weight is: " << ThingOne.getWeight() << endl;

    //system("PAUSE");  // MAC User comment this line out.

    return 0;

}