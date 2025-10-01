#pragma once

// Предварительное объявление - MainApp будет использовать только указатели
class Vector;

// Чистый C-интерфейс для работы с DLL
extern "C" {
    __declspec(dllexport) Vector* CreateVector(double x, double y);
    __declspec(dllexport) void DeleteVector(Vector* vec);
    __declspec(dllexport) double GetVectorX(Vector* vec);
    __declspec(dllexport) double GetVectorY(Vector* vec);
    __declspec(dllexport) double GetVectorLength(Vector* vec);
    __declspec(dllexport) void PrintVector(Vector* vec);
}