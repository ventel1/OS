#include "buffered_channel.h"
#include <iostream>
#include <thread>
#include <vector>

int main() {
    // Тест 1: Базовая функциональность
    {
        BufferedChannel<int> channel(2);
        channel.Send(1);
        channel.Send(2);

        std::pair<int, bool> result1 = channel.Recv();
        std::pair<int, bool> result2 = channel.Recv();

        std::cout << "Test 1: " << result1.first << "(" << result1.second << "), "
            << result2.first << "(" << result2.second << ")" << std::endl;
    }

    // Тест 2: Закрытие канала
    {
        BufferedChannel<int> channel(2);
        channel.Send(1);
        channel.Close();

        std::pair<int, bool> result1 = channel.Recv(); // должно вернуть 1, true
        std::pair<int, bool> result2 = channel.Recv(); // должно вернуть 0, false

        std::cout << "Test 2: " << result1.first << "(" << result1.second << "), "
            << result2.first << "(" << result2.second << ")" << std::endl;
    }

    // Тест 3: Многопоточность
    {
        BufferedChannel<int> channel(3);
        std::vector<std::thread> threads;

        // Потоки-отправители
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back([&channel, i]() {
                for (int j = 0; j < 2; ++j) {
                    channel.Send(i * 10 + j);
                }
                });
        }

        // Поток-получатель
        threads.emplace_back([&channel]() {
            int count = 0;
            while (count < 6) {
                std::pair<int, bool> result = channel.Recv();
                if (result.second) {
                    std::cout << "Received: " << result.first << std::endl;
                    count++;
                }
                else {
                    break;
                }
            }
            });

        // Даем время на отправку/получение
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        channel.Close();

        for (auto& t : threads) {
            t.join();
        }
    }

    std::cout << "All tests completed!" << std::endl;
    return 0;
}