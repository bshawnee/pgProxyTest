//
// Created by blow_job on 8/21/24.
//

#ifndef PGPROXYSERVER_SENDER_H
#define PGPROXYSERVER_SENDER_H

#include "ChanelStream.h"

template <typename T = observable_t>
class Sender
{
public:
    Sender() = default;

    Sender(const Sender& ref) = delete;
    Sender(Sender&& ref) noexcept = default;

    ~Sender()
    {
        streamPtr_->disconnect(ChanelSide::eSender);
    }

    Sender& operator = (Sender& ref) = delete;
    Sender& operator = (Sender&& ref) noexcept = default;

    void init(chanelBufPtr_t stream)
    {
        streamPtr_ = std::move(stream);
        streamPtr_->establishConnection(ChanelSide::eSender);

    }

    Sender& operator << (T&& ref)
    {
        (*streamPtr_) << std::forward<T>(ref);
        return *this;
    }

private:
    chanelBufPtr_t streamPtr_;
};

#endif //PGPROXYSERVER_SENDER_H
