#include "vector.h"
#include "../NumberLib/number.h"
#include <iostream>
#include <cmath>

// Реальная реализация класса Vector - скрыта внутри DLL
class Vector {
private:
    Number x;
    Number y;

public:
    Vector(const Number& x_val, const Number& y_val) : x(x_val), y(y_val) {}

    Number getX() const { return x; }
    Number getY() const { return y; }

    Number getRadius() const {
        double x_val = x.getValue();
        double y_val = y.getValue();
        return Number(std::sqrt(x_val * x_val + y_val * y_val));
    }

    void print() const {
        std::cout << "Vector(" << x.getValue() << ", " << y.getValue() << ")";
    }
};

// Реализации экспортируемых функций
extern "C" {
    __declspec(dllexport) Vector* CreateVector(double x, double y) {
        return new Vector(Number(x), Number(y));
    }

    __declspec(dllexport) void DeleteVector(Vector* vec) {
        delete vec;
    }

    __declspec(dllexport) double GetVectorX(Vector* vec) {
        return vec->getX().getValue();
    }

    __declspec(dllexport) double GetVectorY(Vector* vec) {
        return vec->getY().getValue();
    }

    __declspec(dllexport) double GetVectorLength(Vector* vec) {
        return vec->getRadius().getValue();
    }

    __declspec(dllexport) void PrintVector(Vector* vec) {
        vec->print();
    }
}