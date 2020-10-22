#pragma once

template <class T>
class AThing {
    T weight;

public: 
	AThing();
	AThing(T weight);
    T getWeight(void); 
    void setWeight(T inWeight);

};

template <class T>
AThing<T>::AThing() {
	setWeight(0.0);
}

template <class T>
AThing<T>::AThing(T weight) {
	setWeight(weight);
}

template <class T>
void AThing<T>::setWeight(T x) {
	this->weight = x;
}

template <class T>
T AThing<T>::getWeight() {
	return weight;
}