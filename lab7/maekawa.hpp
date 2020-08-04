class MaekawaLock;

class Maekawa {
private:
    /* data */
public:
    Maekawa(/* args */);
    void do_isr();
    MaekawaLock lock();
    ~Maekawa();
};

class MaekawaLock {
private:
    /* data */
public:
    MaekawaLock(/* args */);
    ~MaekawaLock();
};

MaekawaLock::MaekawaLock(/* args */) {
}

MaekawaLock::~MaekawaLock() {
}


Maekawa::Maekawa(/* args */) {
}

void Maekawa::do_isr() {

}

void Maekawa::lock() {

}

Maekawa::~Maekawa() {
}
