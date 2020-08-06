#ifndef ___RAII_LOCK_HPP___
#define ___RAII_LOCK_HPP___

/**
 * Calls unlock on its parent when leaving scope.
 */
template <class T>
class RAIILock {
private:
    T *parent;
public:
    RAIILock(T *parent) : parent(parent) { }
    ~RAIILock() {
        parent->unlock();
    }
};

#endif