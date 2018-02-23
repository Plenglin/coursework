#include "prog3.h"

#include <string>

using namespace std;

AThing::AThing() {
	weight = 1;
}

AThing::AThing(double weight) {
	setWeight(weight);
}

void AThing::setWeight(double x) {
	this->weight = weight;
}

double AThing::getWeight() {
	return weight;
}