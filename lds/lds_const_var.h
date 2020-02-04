#ifndef LDS_CONST_VAR_H
#define LDS_CONST_VAR_H

#include <QVariant>

template <typename T>
class lds_const_var : public T
{
public:
    lds_const_var() {}
    lds_const_var(T other) {T & ref = *this; ref = other;}
    void setOther(const T &other) {T & ref = *this; ref = other;}
    void assignValue(const T &other) {T & ref = *this; ref = other;}
private:
    lds_const_var& operator=(const T & other) {T & ref = *this; ref = other; return *this;}
};

class lds_const_int {
public:
    lds_const_int() {}
    lds_const_int(int val):_val(val) {}
    operator int() const { return _val;}
    bool operator==(int other) {return _val == other;}
    void setOther(int other) {_val = other;}
    void assignValue(int other) {_val = other;}
private:
    lds_const_int& operator=(int other) {_val = other; return *this;}
    int _val;
};

class lds_const_bool {
public:
    lds_const_bool() {}
    lds_const_bool(bool val):_val(val) {}
    operator bool() const { return _val;}
    bool operator==(bool other) {return _val == other;}
    void setOther(bool other) {_val = other;}
    void assignValue(bool other) {_val = other;}
private:
    lds_const_bool& operator=(bool other) {_val = other; return *this;}
    bool _val;
};

#endif // LDS_CONST_VAR_H
