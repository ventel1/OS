#include "number.h"
#include <stdexcept>

Number::Number(double val) : value(val) {}

double Number::getValue() const {
    return value;
}

void Number::setValue(double val) {
    value = val;
}

Number Number::operator+(const Number& other) const {
    return Number(value + other.value);
}

Number Number::operator-(const Number& other) const {
    return Number(value - other.value);
}

Number Number::operator*(const Number& other) const {
    return Number(value * other.value);
}

Number Number::operator/(const Number& other) const {
    if (other.value == 0.0) {
        throw std::runtime_error("Division by zero");
    }
    return Number(value / other.value);
}

const Number ZERO(0.0);
const Number ONE(1.0);

Number createNumber(double value) {
    return Number(value);
}