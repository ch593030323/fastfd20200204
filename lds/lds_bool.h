#ifndef LDS_BOOL_H
#define LDS_BOOL_H

class lds_bool{
public:
    lds_bool(bool value):b(value) {}

    inline lds_bool operator=(bool other) {b = other;return *this;}

    inline operator bool () const
    {
        return b;
    }
protected:
    bool b;
};

class lds_bool_default_true : public lds_bool
{
public:
    inline lds_bool_default_true():lds_bool(true) {}
    inline lds_bool_default_true operator=(bool other) {b = other;return *this;}
};

class lds_bool_default_false : public lds_bool
{
public:
    inline lds_bool_default_false():lds_bool(false) {}
    inline lds_bool_default_false operator=(bool other) {b = other;return *this;}
};
#endif // LDS_BOOL_H
