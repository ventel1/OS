#include <iostream>
#include <dlfcn.h>
#include "number.h"
#include "vector.h"

using namespace std;

int main() {
    cout << "=== Тестирование библиотеки Number ===" << endl;
    
    // Тестирование базовых операций
    Number a = createNumber(5.0);
    Number b = createNumber(3.0);
    
    cout << "a = "; a.print(); cout << endl;
    cout << "b = "; b.print(); cout << endl;
    
    Number sum = a + b;
    cout << "a + b = "; sum.print(); cout << endl;
    
    Number diff = a - b;
    cout << "a - b = "; diff.print(); cout << endl;
    
    Number prod = a * b;
    cout << "a * b = "; prod.print(); cout << endl;
    
    Number quot = a / b;
    cout << "a / b = "; quot.print(); cout << endl;
    
    cout << "ZERO = "; ZERO.print(); cout << endl;
    cout << "ONE = "; ONE.print(); cout << endl;
    
    cout << "\n=== Тестирование библиотеки Vector ===" << endl;
    
    // Тестирование векторов
    Vector v1(&a, &b);  // (5, 3)
    Vector v2(&b, &a);  // (3, 5)
    
    cout << "Вектор v1: "; v1.print(); cout << endl;
    cout << "Вектор v2: "; v2.print(); cout << endl;
    
    Vector sum_vec = v1 + v2;
    cout << "v1 + v2: "; sum_vec.print(); cout << endl;
    
    cout << "Полярные координаты v1: "; v1.printPolar(); cout << endl;
    cout << "Полярные координаты v2: "; v2.printPolar(); cout << endl;
    
    cout << "ZERO_VECTOR: "; ZERO_VECTOR.print(); cout << endl;
    cout << "ONE_VECTOR: "; ONE_VECTOR.print(); cout << endl;
    
    // Тестирование конкретных случаев
    cout << "\n=== Дополнительные тесты ===" << endl;
    
    // Вектор с отрицательными координатами
    Number neg = createNumber(-2.0);
    Vector v3(&neg, &b);  // (-2, 3)
    cout << "Вектор v3: "; v3.print(); cout << endl;
    cout << "Полярные координаты v3: "; v3.printPolar(); cout << endl;
    
    // Вектор на осях
    Vector v4(&a, &ZERO);  // (5, 0)
    cout << "Вектор v4: "; v4.print(); cout << endl;
    cout << "Полярные координаты v4: "; v4.printPolar(); cout << endl;
    
    return 0;
}
