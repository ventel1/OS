#ifndef VECTOR_H
#define VECTOR_H

class Number; // Предварительное объявление

class Vector {
private:
    Number* x;
    Number* y;

public:
    Vector(Number* x_val, Number* y_val);
    ~Vector();
    
    // Копирующий конструктор и оператор присваивания
    Vector(const Vector& other);
    Vector& operator=(const Vector& other);
    
    // Методы для полярных координат
    Number getR() const;    // Радиус
    Number getPhi() const;  // Угол
    
    // Сложение векторов
    Vector operator+(const Vector& other) const;
    
    // Получение координат
    Number getX() const;
    Number getY() const;
    
    // Вывод
    void print() const;
    void printPolar() const;
};

// Глобальные переменные векторов
extern Vector ZERO_VECTOR;
extern Vector ONE_VECTOR;

#endif
