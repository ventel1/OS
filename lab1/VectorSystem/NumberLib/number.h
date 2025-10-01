#pragma once

class Number {
private:
    double value;

public:
    Number(double val = 0.0);
    double getValue() const;
    void setValue(double val);

    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;
};

extern const Number ZERO;
extern const Number ONE;
Number createNumber(double value);