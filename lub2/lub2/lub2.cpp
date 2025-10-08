#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <windows.h>
#include <iomanip>

class MatrixMultiplier {
private:
    int N;

    // Вспомогательная структура для передачи данных в поток
    struct ThreadData {
        const std::vector<std::vector<int>>* A;
        const std::vector<std::vector<int>>* B;
        std::vector<std::vector<int>>* C;
        int startRow;
        int endRow;
        int startCol;
        int endCol;
    };

    // Статическая функция для потока
    static void multiplyBlock(ThreadData* data) {
        for (int i = data->startRow; i < data->endRow; i++) {
            for (int j = data->startCol; j < data->endCol; j++) {
                int sum = 0;
                for (int k = 0; k < (int)data->A->size(); k++) {
                    sum += (*data->A)[i][k] * (*data->B)[k][j];
                }
                (*data->C)[i][j] = sum;
            }
        }
        delete data; // Освобождаем память
    }

public:
    MatrixMultiplier(int size) : N(size) {}

    // Обычное умножение матриц (последовательное)
    std::vector<std::vector<int>> multiplySequential(
        const std::vector<std::vector<int>>& A,
        const std::vector<std::vector<int>>& B) {

        std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        return C;
    }

    // Многопоточное умножение с блочным разбиением
    std::vector<std::vector<int>> multiplyParallel(
        const std::vector<std::vector<int>>& A,
        const std::vector<std::vector<int>>& B,
        int blockSize) {

        std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));
        std::vector<std::thread> threads;

        // Вычисляем количество блоков по строкам и столбцам
        int numBlocksRows = (N + blockSize - 1) / blockSize;
        int numBlocksCols = (N + blockSize - 1) / blockSize;

        // Создаем потоки для каждого блока
        for (int blockRow = 0; blockRow < numBlocksRows; blockRow++) {
            for (int blockCol = 0; blockCol < numBlocksCols; blockCol++) {
                int startRow = blockRow * blockSize;
                int endRow = (blockRow + 1) * blockSize;
                if (endRow > N) endRow = N;

                int startCol = blockCol * blockSize;
                int endCol = (blockCol + 1) * blockSize;
                if (endCol > N) endCol = N;

                // Создаем данные для потока
                ThreadData* data = new ThreadData{ &A, &B, &C, startRow, endRow, startCol, endCol };

                // Создаем поток
                threads.push_back(std::thread(multiplyBlock, data));
            }
        }

        // Ждем завершения всех потоков
        for (auto& thread : threads) {
            thread.join();
        }

        return C;
    }

    // Проверка равенства матриц
    bool areMatricesEqual(const std::vector<std::vector<int>>& A,
        const std::vector<std::vector<int>>& B) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (A[i][j] != B[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    // Заполнение матрицы случайными значениями
    void fillMatrixRandom(std::vector<std::vector<int>>& matrix) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = rand() % 10; // Числа от 0 до 9 для простоты
            }
        }
    }

    // Тестирование для одного размера блока
    void testBlockSize(int blockSize) {
        // Создаем матрицы
        std::vector<std::vector<int>> A(N, std::vector<int>(N));
        std::vector<std::vector<int>> B(N, std::vector<int>(N));

        // Заполняем случайными значениями
        fillMatrixRandom(A);
        fillMatrixRandom(B);

        // Вычисляем количество блоков и потоков
        int numBlocksRows = (N + blockSize - 1) / blockSize;
        int numBlocksCols = (N + blockSize - 1) / blockSize;
        int totalBlocks = numBlocksRows * numBlocksCols;

        std::cout << "Матрица: " << N << "x" << N;
        std::cout << ", Блок: " << blockSize;
        std::cout << ", Блоков: " << totalBlocks;
        std::cout << ", Потоков: " << totalBlocks << std::endl;

        // Тестируем последовательное умножение
        auto start = std::chrono::high_resolution_clock::now();
        auto C_seq = multiplySequential(A, B);
        auto end = std::chrono::high_resolution_clock::now();
        auto seq_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Тестируем параллельное умножение
        start = std::chrono::high_resolution_clock::now();
        auto C_par = multiplyParallel(A, B, blockSize);
        end = std::chrono::high_resolution_clock::now();
        auto par_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Проверяем корректность
        bool correct = areMatricesEqual(C_seq, C_par);

        double speedup = 0.0;
        if (par_time.count() > 0) {
            speedup = static_cast<double>(seq_time.count()) / par_time.count();
        }

        std::cout << "Последовательное: " << seq_time.count() << " мкс" << std::endl;
        std::cout << "Параллельное: " << par_time.count() << " мкс" << std::endl;
        std::cout << "Ускорение: " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
        std::cout << "Корректность: " << (correct ? "Да" : "НЕТ!") << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(CP_UTF8);
    srand(static_cast<unsigned>(time(0)));

    std::cout << "МНОГОПОТОЧНОЕ УМНОЖЕНИЕ МАТРИЦ" << std::endl;
    std::cout << "========================================" << std::endl;

    // Тестируем для разных размеров матриц
    std::vector<int> matrixSizes = { 100, 200 };

    for (int size : matrixSizes) {
        MatrixMultiplier multiplier(size);

        std::cout << "\nТЕСТ ДЛЯ МАТРИЦЫ " << size << "x" << size << std::endl;
        std::cout << "========================================" << std::endl;

        // Тестируем разные размеры блоков
        std::vector<int> blockSizes;
        for (int blockSize = 1; blockSize <= size; blockSize *= 2) {
            if (blockSize > size) break;
            blockSizes.push_back(blockSize);
        }
        // Добавляем последний размер блока
        if (blockSizes.empty() || blockSizes.back() != size) {
            blockSizes.push_back(size);
        }

        for (int blockSize : blockSizes) {
            multiplier.testBlockSize(blockSize);
        }
    }

    // Тест с маленькой матрицей для демонстрации
    std::cout << "\nДЕМОНСТРАЦИЯ С МАЛЕНЬКОЙ МАТРИЦЕЙ" << std::endl;
    std::cout << "========================================" << std::endl;

    MatrixMultiplier smallMultiplier(8);
    smallMultiplier.testBlockSize(2);
    smallMultiplier.testBlockSize(4);
    smallMultiplier.testBlockSize(8);

    return 0;
}