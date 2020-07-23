
#include <stdlib.h>     // srand, rand 
#include <time.h>       // time 


int main ()
{
srand (time(NULL)); // initialize random seed. That�s important and comes right at the start of the main function and only once, never again.
int randomnumber = rand() % 10; // random number from 0 to 9
}
