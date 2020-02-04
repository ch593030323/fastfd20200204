#ifndef POINTERAUTODEL_H
#define POINTERAUTODEL_H

#include <QtDebug>

template <typename T>
class pointerAutoDel{
public:
    pointerAutoDel(T* o = 0) {
        this->o = o;
    }

    ~pointerAutoDel() {
        delPointer();
    }
    T *getPointer() {
        return o;
    }
    void setPointer(T* o) {
        this->o = o;
    }
    void delPointer() {
        if(o) {
            delete o;
            o = 0;
        }
    }
    inline T* operator->() const
    { return static_cast<T*>(o); }

private:
    T *o;
};


#endif // POINTERAUTODEL_H
