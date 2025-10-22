#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <cmath>
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <thread>
#endif

class MatrixMultiplier {
private:
    int N;
    struct ThreadData {
        const std::vector<std::vector<int>>* A;
        const std::vector<std::vector<int>>* B;
        std::vector<std::vector<int>>* C;
        int startRow;
        int endRow;
        int startCol;
        int endCol;
    };

#ifdef _WIN32
    // Функция для Windows потока
    static unsigned __stdcall multiplyBlockWindows(void* arg) {
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

        delete data;
        return 0;
    }
#else
    // Функция для pthread (Linux)
    static void* multiplyBlockPthread(void* arg) {
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

        delete data;
        return nullptr;
    }
#endif
    void processBlock(const std::vector<std::vector<int>>& A,
        const std::vector<std::vector<int>>& B,
        std::vector<std::vector<int>>& C,
        int startRow, int endRow,
        int startCol, int endCol) {
        for (int i = startRow; i < endRow; i++) {
            for (int j = startCol; j < endCol; j++) {
                int sum = 0;
                for (int k = 0; k < N; k++) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
    }

public:
    MatrixMultiplier(int size) : N(size) {}

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

    std::vector<std::vector<int>> multiplyParallelNative(
        const std::vector<std::vector<int>>& A,
        const std::vector<std::vector<int>>& B,
        int blockSize) {

        std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

        int numBlocksRows = (N + blockSize - 1) / blockSize;
        int numBlocksCols = (N + blockSize - 1) / blockSize;
        int totalThreads = numBlocksRows * numBlocksCols;

#ifdef _WIN32
        // Реализация для Windows
        std::vector<HANDLE> threads(totalThreads);
        int threadIndex = 0;

        for (int blockRow = 0; blockRow < numBlocksRows; blockRow++) {
            for (int blockCol = 0; blockCol < numBlocksCols; blockCol++) {
                int startRow = blockRow * blockSize;
                int endRow = (startRow + blockSize < N) ? startRow + blockSize : N;
                int startCol = blockCol * blockSize;
                int endCol = (startCol + blockSize < N) ? startCol + blockSize : N;

                ThreadData* data = new ThreadData{ &A, &B, &C, startRow, endRow, startCol, endCol };

                threads[threadIndex] = (HANDLE)_beginthreadex(nullptr, 0, multiplyBlockWindows, data, 0, nullptr);
                if (threads[threadIndex] == nullptr) {
                    std::cerr << "Ошибка создания потока Windows!" << std::endl;
                    delete data;
                }
                threadIndex++;
            }
        }
        WaitForMultipleObjects(totalThreads, threads.data(), TRUE, INFINITE);
        for (auto handle : threads) {
            CloseHandle(handle);
        }
#else
        // Реализация для Linux (pthread)
        std::vector<pthread_t> threads(totalThreads);
        int threadIndex = 0;

        for (int blockRow = 0; blockRow < numBlocksRows; blockRow++) {
            for (int blockCol = 0; blockCol < numBlocksCols; blockCol++) {
                int startRow = blockRow * blockSize;
                int endRow = (startRow + blockSize < N) ? startRow + blockSize : N;
                int startCol = blockCol * blockSize;
                int endCol = (startCol + blockSize < N) ? startCol + blockSize : N;

                ThreadData* data = new ThreadData{ &A, &B, &C, startRow, endRow, startCol, endCol };

                if (pthread_create(&threads[threadIndex], nullptr, multiplyBlockPthread, data) != 0) {
                    std::cerr << "Ошибка создания pthread!" << std::endl;
                    delete data;
                }
                threadIndex++;
            }
        }
        for (int i = 0; i < totalThreads; i++) {
            pthread_join(threads[i], nullptr);
        }
#endif

        return C;
    }

    // Многопоточное умножение с использованием std::thread (только для Linux)
    std::vector<std::vector<int>> multiplyParallelStdThread(
        const std::vector<std::vector<int>>& A,
        const std::vector<std::vector<int>>& B,
        int blockSize) {

        std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

#ifdef _WIN32

        return multiplyParallelNative(A, B, blockSize);
#else
        std::vector<std::thread> threads;

        int numBlocksRows = (N + blockSize - 1) / blockSize;
        int numBlocksCols = (N + blockSize - 1) / blockSize;

        for (int blockRow = 0; blockRow < numBlocksRows; blockRow++) {
            for (int blockCol = 0; blockCol < numBlocksCols; blockCol++) {
                int startRow = blockRow * blockSize;
                int endRow = (startRow + blockSize < N) ? startRow + blockSize : N;
                int startCol = blockCol * blockSize;
                int endCol = (startCol + blockSize < N) ? startCol + blockSize : N;

                threads.push_back(std::thread(&MatrixMultiplier::processBlock, this,
                    std::cref(A), std::cref(B), std::ref(C),
                    startRow, endRow, startCol, endCol));
            }
        }

        for (auto& thread : threads) {
            thread.join();
        }
#endif

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

    // Определение ОС
    std::string getOSName() {
#ifdef _WIN32
        return "Windows";
#elif __linux__
        return "Linux";
#else
        return "Unknown";
#endif
    }

 
    void testBlockSize(int blockSize) {
        std::vector<std::vector<int>> A(N, std::vector<int>(N));
        std::vector<std::vector<int>> B(N, std::vector<int>(N));
        fillMatrixRandom(A);
        fillMatrixRandom(B);

        int numBlocksRows = (N + blockSize - 1) / blockSize;
        int numBlocksCols = (N + blockSize - 1) / blockSize;
        int totalBlocks = numBlocksRows * numBlocksCols;

        std::cout << "Matrix: " << N << "x" << N;
        std::cout << ", Block: " << blockSize;
        std::cout << ", Blocks: " << totalBlocks;
        std::cout << ", Threads: " << totalBlocks << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        auto C_seq = multiplySequential(A, B);
        auto end = std::chrono::high_resolution_clock::now();
        auto seq_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        start = std::chrono::high_resolution_clock::now();
        auto C_par_native = multiplyParallelNative(A, B, blockSize);
        end = std::chrono::high_resolution_clock::now();
        auto par_native_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        bool correct_native = areMatricesEqual(C_seq, C_par_native);

        double speedup_native = 0.0;

        if (par_native_time.count() > 0) {
            speedup_native = static_cast<double>(seq_time.count()) / par_native_time.count();
        }

        std::cout << "Sequential: " << seq_time.count() << " microseconds" << std::endl;
        std::cout << "Parallel (native): " << par_native_time.count() << " microseconds";
        std::cout << " (speedup: " << std::fixed << std::setprecision(2) << speedup_native << "x)" << std::endl;
        std::cout << "Correct: " << (correct_native ? "Yes" : "NO!") << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
};

int main() {
    MatrixMultiplier multiplier(100); 

    std::cout << "MULTITHREADED MATRIX MULTIPLICATION (" << multiplier.getOSName() << ")" << std::endl;
    std::cout << "========================================" << std::endl;

#ifdef _WIN32
    std::cout << "Running on Windows - using Windows Threads API" << std::endl;
#else
    std::cout << "Running on Linux - using pthread and std::thread" << std::endl;
#endif

    std::vector<int> matrixSizes = { 100, 200 };

    for (int size : matrixSizes) {
        MatrixMultiplier multiplier(size);

        std::cout << "\nTEST FOR MATRIX " << size << "x" << size << std::endl;
        std::cout << "========================================" << std::endl;

        std::vector<int> blockSizes;
        for (int blockSize = 1; blockSize <= size; blockSize *= 2) {
            if (blockSize > size) break;
            blockSizes.push_back(blockSize);
        }
        if (blockSizes.empty() || blockSizes.back() != size) {
            blockSizes.push_back(size);
        }

        for (int blockSize : blockSizes) {
            multiplier.testBlockSize(blockSize);
        }
    }

    return 0;
}