#include "vector.h"
#include "number.h"
#include <iostream>
#include <cmath>

using namespace std;

// Конструктор
Vector::Vector(Number* x_val, Number* y_val) : x(new Number(*x_val)), y(new Number(*y_val)) {}

// Деструктор
Vector::~Vector() {
    delete x;
    delete y;
}

// Копирующий конструктор
Vector::Vector(const Vector& other) : x(new Number(*other.x)), y(new Number(*other.y)) {}

// Оператор присваивания
Vector& Vector::operator=(const Vector& other) {
    if (this != &other) {
        *x = *other.x;
        *y = *other.y;
    }
    return *this;
}

// Методы для полярных координат
Number Vector::getR() const {
    double x_val = x->getValue();
    double y_val = y->getValue();
    return createNumber(sqrt(x_val * x_val + y_val * y_val));
}

Number Vector::getPhi() const {
    double x_val = x->getValue();
    double y_val = y->getValue();
    
    if (x_val == 0.0 && y_val == 0.0) {
        return createNumber(0.0);
    }
    
    return createNumber(atan2(y_val, x_val));
}

// Сложение векторов
Vector Vector::operator+(const Vector& other) const {
    Number new_x = *x + *other.x;
    Number new_y = *y + *other.y;
    return Vector(&new_x, &new_y);
}

// Получение координат
Number Vector::getX() const {
    return *x;
}

Number Vector::getY() const {
    return *y;
}

// Вывод
void Vector::print() const {
    cout << "(";
    x->print();
    cout << ", ";
    y->print();
    cout << ")";
}

void Vector::printPolar() const {
    cout << "(r = ";
    getR().print();
    cout << ", φ = ";
    getPhi().print();
    cout << ")";
}

// Глобальные переменные векторов
Vector ZERO_VECTOR(&ZERO, &ZERO);
Vector ONE_VECTOR(&ONE, &ONE);
