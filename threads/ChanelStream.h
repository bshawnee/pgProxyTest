//
// Created by blow_job on 8/21/24.
//

#ifndef PGPROXYSERVER_CHANELSTREAM_H
#define PGPROXYSERVER_CHANELSTREAM_H

//
// Created by Илья Рожнев on 21.02.2023.
//

#ifndef CHANELSTREAM_HPP
#define CHANELSTREAM_HPP
#include "Observer.h"
#include <memory>
#include <mutex>
#include <condition_variable>
#include <list>
template <typename T> class ChanelStream;
using chanelBuf_t = ChanelStream<observable_t>;
using chanelBufPtr_t = std::shared_ptr<chanelBuf_t>;

enum class ChanelSide {
    eReader,
    eSender
};


template <typename T>
class ChanelStream
{
public:

    ChanelStream() = default;
    ChanelStream(const ChanelStream&) = delete;
    ChanelStream(ChanelStream&&) = delete;

    ChanelStream& operator = (const ChanelStream&) = delete;
    ChanelStream& operator = (ChanelStream&&) = delete;

    ChanelStream& operator >> (T& var)
    {
        std::unique_lock lock(sendMutex_);
        var = std::move(ct_.front());
        ct_.pop_front();
        return *this;
    }

    ChanelStream& operator << (T&& var)
    {
        std::unique_lock lock(sendMutex_);
        ct_.emplace_back(std::forward<T>(var));
        condVar_.notify_one();

        return *this;
    }

    bool eof()
    {
        using namespace std::chrono_literals;

        std::unique_lock lock(sendMutex_);

        const auto now = std::chrono::system_clock::now();

        condVar_.wait(lock, [&]() {
            return !ct_.empty() && sendersConnected_;
        });

        return !sendersConnected_ && ct_.empty();
    }

    void establishConnection(ChanelSide side)
    {
        switch (side) {
            case ChanelSide::eReader:
                break;
            case ChanelSide::eSender:
                increaseSenders();
                break;
        }
        condVar_.notify_one();
    }

    void disconnect(ChanelSide side)
    {
        switch (side) {
            case ChanelSide::eSender:
                --senders_;
                break;
            case ChanelSide::eReader:
                break;
        }

        if (senders_ == 0) {
            sendersConnected_ = false;
        }
        condVar_.notify_one();
    }

private:

    void increaseSenders()
    {
        ++senders_;
        sendersConnected_ = true;

    }

    std::mutex sendMutex_;
    std::condition_variable condVar_;
    std::atomic_size_t senders_{0};
    std::atomic_bool sendersConnected_{false};
    std::list<T> ct_;
};


#endif //CHANELSTREAM_HPP

#endif //PGPROXYSERVER_CHANELSTREAM_H
