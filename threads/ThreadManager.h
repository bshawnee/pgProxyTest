//
// Created by blow_job on 8/20/24.
//

#ifndef PGPROXYSERVER_THREADMANAGER_H
#define PGPROXYSERVER_THREADMANAGER_H
#include <list>
#include <thread>
#include <future>
#include <unordered_map>

struct Task
{
    enum class TaskStatus {
        tsUnknown,
        tsExitSuccess,
        tsExitFailure,
        tsDeffer,
    };
    Task(std::future<TaskStatus>&& f_, int64_t id_)
            : task(std::move(f_))
            , id(id_)
    {}

    std::future<TaskStatus> task;
    int64_t id;
};

class ThreadManager {
public:
    explicit ThreadManager(int64_t threads);

    ThreadManager() = delete;
    ThreadManager(const ThreadManager&) = delete;
    ThreadManager(ThreadManager&&) = delete;
    ThreadManager& operator = (const ThreadManager&) = delete;
    ThreadManager& operator = (ThreadManager&&) = delete;

    ~ThreadManager();

    template <typename Func, typename ... Args>
    int64_t add(Func&& f, Args&& ... args)
    {
        int64_t taskId = taskId_++;
        std::lock_guard lock(queueMutex_);

        queue_.emplace_back(std::async(std::launch::deferred, f, args ...), taskId);
        queueCV_.notify_one();

        return taskId;
    }

    template <typename Func, typename ... Args>
    void add(int64_t taskId, Func&& f, Args&& ... args)
    {
        std::lock_guard lock(queueMutex_);
        queue_.emplace_back(std::async(std::launch::deferred, f, args ...), taskId);
        queueCV_.notify_one();
    }

    Task::TaskStatus getResult(int64_t);

    void wait(int64_t taskId);
    void wait_all();
private:
    void run();


    std::list<Task> queue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;

    std::mutex completedMutex_;
    std::condition_variable completedCV_;

    std::atomic_int64_t taskId_{0};
    std::atomic_bool quite_{false};
    std::list<std::thread> threads_;
    std::unordered_map<int64_t, Task::TaskStatus> completedTaskId_;
};


#endif //PGPROXYSERVER_THREADMANAGER_H
