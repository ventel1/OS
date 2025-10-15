#include <iostream>
#include <vector>
#include <thread>
#include <pthread.h>
#include <chrono>
#include <random>
#include <iomanip>
#include <cmath>

class MatrixMultiplier {
private:
    int N;

    // Структура для передачи данных в поток
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
    static void* multiplyBlock(void* arg) {
        ThreadData* data = static_cast<ThreadData*>(arg);
        
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
        return nullptr;
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

    // Многопоточное умножение с блочным разбиением (pthread)
    std::vector<std::vector<int>> multiplyParallelPthread(
        const std::vector<std::vector<int>>& A,
        const std::vector<std::vector<int>>& B,
        int blockSize) {

        std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));
        
        // Вычисляем количество блоков по строкам и столбцам
        int numBlocksRows = (N + blockSize - 1) / blockSize;
        int numBlocksCols = (N + blockSize - 1) / blockSize;
        int totalThreads = numBlocksRows * numBlocksCols;
        
        std::vector<pthread_t> threads(totalThreads);
        int threadIndex = 0;

        // Создаем потоки для каждого блока
        for (int blockRow = 0; blockRow < numBlocksRows; blockRow++) {
            for (int blockCol = 0; blockCol < numBlocksCols; blockCol++) {
                int startRow = blockRow * blockSize;
                int endRow = std::min(startRow + blockSize, N);
                int startCol = blockCol * blockSize;
                int endCol = std::min(startCol + blockSize, N);

                // Создаем данные для потока
                ThreadData* data = new ThreadData{ &A, &B, &C, startRow, endRow, startCol, endCol };

                // Создаем поток
                if (pthread_create(&threads[threadIndex], nullptr, multiplyBlock, data) != 0) {
                    std::cerr << "Ошибка создания потока!" << std::endl;
                    delete data;
                }
                threadIndex++;
            }
        }

        // Ждем завершения всех потоков
        for (int i = 0; i < totalThreads; i++) {
            pthread_join(threads[i], nullptr);
        }

        return C;
    }

    // Многопоточное умножение с использованием std::thread
    std::vector<std::vector<int>> multiplyParallelStdThread(
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
                threads.emplace_back([&, blockRow, blockCol]() {
                    int startRow = blockRow * blockSize;
                    int endRow = std::min(startRow + blockSize, N);
                    int startCol = blockCol * blockSize;
                    int endCol = std::min(startCol + blockSize, N);
                    
                    for (int i = startRow; i < endRow; i++) {
                        for (int j = startCol; j < endCol; j++) {
                            int sum = 0;
                            for (int k = 0; k < N; k++) {
                                sum += A[i][k] * B[k][j];
                            }
                            C[i][j] = sum;
                        }
                    }
                });
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
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9);
        
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = dis(gen);
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

        // Тестируем параллельное умножение (std::thread)
        start = std::chrono::high_resolution_clock::now();
        auto C_par_std = multiplyParallelStdThread(A, B, blockSize);
        end = std::chrono::high_resolution_clock::now();
        auto par_std_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Тестируем параллельное умножение (pthread)
        start = std::chrono::high_resolution_clock::now();
        auto C_par_pthread = multiplyParallelPthread(A, B, blockSize);
        end = std::chrono::high_resolution_clock::now();
        auto par_pthread_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Проверяем корректность
        bool correct_std = areMatricesEqual(C_seq, C_par_std);
        bool correct_pthread = areMatricesEqual(C_seq, C_par_pthread);

        double speedup_std = 0.0;
        double speedup_pthread = 0.0;
        
        if (par_std_time.count() > 0) {
            speedup_std = static_cast<double>(seq_time.count()) / par_std_time.count();
        }
        if (par_pthread_time.count() > 0) {
            speedup_pthread = static_cast<double>(seq_time.count()) / par_pthread_time.count();
        }

        std::cout << "Последовательное: " << seq_time.count() << " мкс" << std::endl;
        std::cout << "Параллельное (std::thread): " << par_std_time.count() << " мкс";
        std::cout << " (ускорение: " << std::fixed << std::setprecision(2) << speedup_std << "x)" << std::endl;
        std::cout << "Параллельное (pthread): " << par_pthread_time.count() << " мкс";
        std::cout << " (ускорение: " << std::fixed << std::setprecision(2) << speedup_pthread << "x)" << std::endl;
        std::cout << "Корректность std::thread: " << (correct_std ? "Да" : "НЕТ!") << std::endl;
        std::cout << "Корректность pthread: " << (correct_pthread ? "Да" : "НЕТ!") << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
};

int main() {
    std::cout << "МНОГОПОТОЧНОЕ УМНОЖЕНИЕ МАТРИЦ (Linux)" << std::endl;
    std::cout << "========================================" << std::endl;

    // Тестируем для разных размеров матриц
    std::vector<int> matrixSizes = {100, 200, 500};

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

    return 0;
}
