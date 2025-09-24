#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
using namespace std;

class ThreadPool {
public:
    // numero de threads
    ThreadPool(size_t num_threads
               = thread::hardware_concurrency())
    {

        for (size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back([this] {
                while (true) {
                    function<void()> task;
                    {
                        // trava a fila de tarefas
                        unique_lock<mutex> lock(
                            queue_mutex_);

                        // espera algo pra fazer ou uma ordem de parada
                        cv_.wait(lock, [this] {
                            return !tasks_.empty() || stop_;
                        });

                        // para as threads se receber uma ordem de parada e a lista de tarefas estiver vazia
                        if (stop_ && tasks_.empty()) {
                            return;
                        }

                        // pega a proxima tarefa e remove a mesma da lista
                        task = move(tasks_.front());
                        tasks_.pop();
                    }
                    // realiza a tarefa
                    task();
                }
            });
        }
    }

    // destrutor
    ~ThreadPool()
    {
        {
            // trava a fila para parar de forma segura
            unique_lock<mutex> lock(queue_mutex_);
            stop_ = true;
        }

        // informa todas as threads
        cv_.notify_all();

        // garante que todos terminaram suas tarefas
        for (auto& thread : threads_) {
            thread.join();
        }
    }

    // coloca uma tarefa na fila
    void enqueue(function<void()> task)
    {
        {
            unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace(move(task));
        }
        cv_.notify_one();
    }

private:
    // vetor de threads
    vector<thread> threads_;

    // fila de tarefas
    queue<function<void()> > tasks_;

    // mutex da fila
    mutex queue_mutex_;

    // variavel condicional para infomar as threads de mudanças na fila
    condition_variable cv_;

    // flag que indica se a fila deve parar ou nao
    bool stop_ = false;
};

