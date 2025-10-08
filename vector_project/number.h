#ifndef NUMBER_H
#define NUMBER_H

class Number {
private:
    double value;

public:
    Number();
    explicit Number(double val);
    
    // Арифметические операции
    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;
    
    // Получение значения
    double getValue() const;
    void setValue(double val);
    
    // Вывод
    void print() const;
};

extern Number ZERO;
extern Number ONE;

Number createNumber(double value);

#endif
