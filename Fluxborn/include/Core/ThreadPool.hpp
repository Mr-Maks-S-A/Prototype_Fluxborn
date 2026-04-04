#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool {
public:
    ThreadPool(size_t threads) : m_Stop(false) {
        for (size_t i = 0; i < threads; ++i) {
            m_Workers.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->m_QueueMutex);
                        // Спим, пока нет задач и мы не останавливаем пул
                        this->m_Condition.wait(lock, [this] {
                            return this->m_Stop || !this->m_Tasks.empty();
                        });

                        if (this->m_Stop && this->m_Tasks.empty()) return;

                        task = std::move(this->m_Tasks.front());
                        this->m_Tasks.pop();
                    }
                    task(); // Выполняем задачу вне мьютекса!
                }
            });
        }
    }

    // Шаблонная функция для добавления задач любого типа
    template<class F, class... Args>
    auto Enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            if (m_Stop) throw std::runtime_error("Enqueue on stopped ThreadPool");
            m_Tasks.emplace([task]() { (*task)(); });
        }
        m_Condition.notify_one();
        return res;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Stop = true;
        }
        m_Condition.notify_all();
        for (std::thread &worker : m_Workers) worker.join();
    }

private:
    std::vector<std::thread> m_Workers;
    std::queue<std::function<void()>> m_Tasks;

    std::mutex m_QueueMutex;
    std::condition_variable m_Condition;
    bool m_Stop;
};
