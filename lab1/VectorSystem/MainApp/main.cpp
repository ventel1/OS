#include <iostream>
#include <windows.h>
#include "../NumberLib/number.h"

class Vector;  


extern "C" {
    typedef Vector* (*CreateVectorFunc)(double, double);
    typedef void (*DeleteVectorFunc)(Vector*);
    typedef double (*GetVectorXFunc)(Vector*);
    typedef double (*GetVectorYFunc)(Vector*);
    typedef double (*GetVectorLengthFunc)(Vector*);
    typedef void (*PrintVectorFunc)(Vector*);
}

void TestNumberLibrary() {
    std::cout << "=== ������������ ���������� NUMBER ===" << std::endl;

    Number a = createNumber(10.0);
    Number b = createNumber(3.0);

    std::cout << "a = " << a.getValue() << std::endl;
    std::cout << "b = " << b.getValue() << std::endl;
    std::cout << "a + b = " << (a + b).getValue() << std::endl;
    std::cout << "a * b = " << (a * b).getValue() << std::endl;
    std::cout << "ZERO = " << ZERO.getValue() << std::endl;
    std::cout << "ONE = " << ONE.getValue() << std::endl;
}

void TestVectorLibrary() {
    std::cout << "\n=== ������������ ���������� VECTOR ===" << std::endl;

    // �������� DLL
    HMODULE hDLL = LoadLibraryA("VectorLib.dll");
    if (!hDLL) {
        std::cout << "������ �������� DLL! ���: " << GetLastError() << std::endl;
        return;
    }

    // ��������� ���������� �� �������
    CreateVectorFunc CreateVector = (CreateVectorFunc)GetProcAddress(hDLL, "CreateVector");
    DeleteVectorFunc DeleteVector = (DeleteVectorFunc)GetProcAddress(hDLL, "DeleteVector");
    GetVectorXFunc GetVectorX = (GetVectorXFunc)GetProcAddress(hDLL, "GetVectorX");
    GetVectorYFunc GetVectorY = (GetVectorYFunc)GetProcAddress(hDLL, "GetVectorY");
    GetVectorLengthFunc GetVectorLength = (GetVectorLengthFunc)GetProcAddress(hDLL, "GetVectorLength");
    PrintVectorFunc PrintVector = (PrintVectorFunc)GetProcAddress(hDLL, "PrintVector");

    if (!CreateVector || !DeleteVector) {
        std::cout << "������ ��������� ������� �� DLL!" << std::endl;
        FreeLibrary(hDLL);
        return;
    }

    // �������� �������� ����� DLL
    Vector* v1 = CreateVector(3.0, 4.0);
    Vector* v2 = CreateVector(1.0, 2.0);

    std::cout << "������ 1: ";
    PrintVector(v1);
    std::cout << " -> X=" << GetVectorX(v1) << ", Y=" << GetVectorY(v1)
        << ", �����=" << GetVectorLength(v1) << std::endl;

    std::cout << "������ 2: ";
    PrintVector(v2);
    std::cout << " -> X=" << GetVectorX(v2) << ", Y=" << GetVectorY(v2)
        << ", �����=" << GetVectorLength(v2) << std::endl;

    // ������������ ��������
    DeleteVector(v1);
    DeleteVector(v2);
    FreeLibrary(hDLL);
}

int main() {
    setlocale(LC_ALL, "Russian");
    TestNumberLibrary();
    TestVectorLibrary();

    std::cout << "\n=== ������������ ��������� ===" << std::endl;
    return 0;
}