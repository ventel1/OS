#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <utility>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : buffer_size_(size > 0 ? size : 0), closed_(false) {}

    void Send(T value) {
        std::unique_lock<std::mutex> lock(mutex_);

        // Ждем, пока появится место в буфере или канал закроется
        send_cv_.wait(lock, [this]() {
            return buffer_.size() < buffer_size_ || closed_;
            });

        // Если канал закрыт, бросаем исключение
        if (closed_) {
            throw std::runtime_error("Channel is closed");
        }

        // Добавляем значение в буфер
        buffer_.push(std::move(value));

        // Уведомляем получателей о новом элементе
        recv_cv_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> lock(mutex_);

        // Ждем, пока появится элемент в буфере или канал закроется и буфер пуст
        recv_cv_.wait(lock, [this]() {
            return !buffer_.empty() || (closed_ && buffer_.empty());
            });

        // Если есть элементы в буфере, извлекаем
        if (!buffer_.empty()) {
            T value = std::move(buffer_.front());
            buffer_.pop();

            // Уведомляем отправителей о свободном месте
            send_cv_.notify_one();

            return { std::move(value), true };
        }

        // Если канал закрыт и буфер пуст, возвращаем флаг закрытия
        return { T(), false };
    }

    void Close() {
        std::unique_lock<std::mutex> lock(mutex_);
        closed_ = true;

        // Уведомляем все ожидающие потоки
        send_cv_.notify_all();
        recv_cv_.notify_all();
    }

private:
    std::queue<T> buffer_;
    const size_t buffer_size_;
    bool closed_;

    std::mutex mutex_;
    std::condition_variable send_cv_;
    std::condition_variable recv_cv_;
};

#endif // BUFFERED_CHANNEL_H_