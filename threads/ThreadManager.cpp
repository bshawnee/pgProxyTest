//
// Created by blow_job on 8/20/24.
//

#include "ThreadManager.h"

ThreadManager::ThreadManager(int64_t threads) {
    for (uint32_t i = 0; i < threads; ++i) {
        threads_.emplace_back(&ThreadManager::run, this);
    }
}

ThreadManager::~ThreadManager() {
    wait_all();
    quite_ = true;
    for (auto& thread : threads_) {
        queueCV_.notify_all();
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ThreadManager::run() {
    while (!quite_) {

        std::unique_lock lock(queueMutex_);

        queueCV_.wait(lock, [&](){
            return !queue_.empty() || quite_;
        });

        if (!queue_.empty()) {

            auto task = std::move(queue_.front());
            queue_.pop_front();
            lock.unlock();
            auto status = task.task.get();

            std::lock_guard guard(completedMutex_);
            completedTaskId_.emplace(task.id, status);
            completedCV_.notify_all();
        }
    }
}

void ThreadManager::wait(int64_t taskId) {
    std::unique_lock lock(completedMutex_);

    completedCV_.wait(lock, [&]() ->bool {
        return std::find_if(completedTaskId_.begin(), completedTaskId_.end(), [&](auto v){
            return taskId == v.first;
        }) != completedTaskId_.end();
    });
}

void ThreadManager::wait_all() {
    std::unique_lock lock(queueMutex_);
    completedCV_.wait(lock, [&](){
        return queue_.empty() && taskId_ == completedTaskId_.size();
    });

}

Task::TaskStatus ThreadManager::getResult(int64_t id) {
    std::unique_lock lc(completedMutex_);
    if (auto it = completedTaskId_.find(id); it != completedTaskId_.end()) {
        return it->second;
    }
    return Task::TaskStatus::tsUnknown;
}
