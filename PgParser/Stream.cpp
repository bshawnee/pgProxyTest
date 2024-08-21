//
// Created by blow_job on 8/20/24.
//

#include "Stream.h"


void Stream::pushBack(const char *buf, size_t bufSize)
{
    std::string_view view(buf, bufSize);
    std::unique_lock lc(mutex_);

    msg_.insert(msg_.end(), view.begin(), view.end());
}

std::optional<std::vector<char>> Stream::consumeBytes(size_t bytes)
{
    std::unique_lock lc(mutex_);
    if (bytes > msg_.size()) {
        return std::nullopt;
    }
    std::vector ret(msg_.begin(),msg_.begin() + bytes);
    msg_.erase(msg_.begin(), msg_.begin() + bytes);
    return ret;
}
