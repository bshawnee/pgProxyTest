//
// Created by blow_job on 8/20/24.
//

#ifndef PGPROXYSERVER_STREAM_H
#define PGPROXYSERVER_STREAM_H
#include <vector>
#include <mutex>
#include <optional>

class Stream {
public:
    void pushBack(const char* buf, size_t bufSize);
    std::optional<std::vector<char>> consumeBytes(size_t bytes);

private:
    std::vector<char> msg_;
    std::mutex mutex_;
};



#endif //PGPROXYSERVER_STREAM_H
