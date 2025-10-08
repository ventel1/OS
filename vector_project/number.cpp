#include "number.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// Конструкторы
Number::Number() : value(0.0) {}

Number::Number(double val) : value(val) {}

// Арифметические операции
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
        throw runtime_error("Division by zero");
    }
    return Number(value / other.value);
}

// Получение и установка значения
double Number::getValue() const {
    return value;
}

void Number::setValue(double val) {
    value = val;
}

// Вывод
void Number::print() const {
    cout << value;
}

// Глобальные переменные
Number ZERO(0.0);
Number ONE(1.0);

// Функция создания числа
Number createNumber(double value) {
    return Number(value);
}
