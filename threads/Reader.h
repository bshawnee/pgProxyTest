//
// Created by blow_job on 8/21/24.
//

#ifndef PGPROXYSERVER_READER_H
#define PGPROXYSERVER_READER_H

#include "ChanelStream.h"

template <typename T = observable_t>
class Reader
{
public:
    Reader() = default;
    Reader(const Reader& ref) = delete;
    Reader(Reader&& ref) noexcept = default;
    Reader& operator = (Reader&& ref) noexcept = default;
    Reader& operator = (const Reader& ref) = delete;
    ~Reader() = default;

    void init(chanelBufPtr_t stream)
    {
        streamPtr_ = std::move(stream);
        streamPtr_->establishConnection(ChanelSide::eReader);
    }

    Reader& operator >> (T& ref)
    {
        (*streamPtr_) >> ref;
        return *this;
    }

    bool eof() const
    {
        return streamPtr_->eof();
    }
private:
    chanelBufPtr_t streamPtr_;
};

#endif //PGPROXYSERVER_READER_H
