//
// Created by blow_job on 8/9/24.
//

#ifndef BROTLI_SOCKET_H
#define BROTLI_SOCKET_H
#include <string_view>
#include <unistd.h>
#include "Addr.h"

static constexpr int MAX_QUEUE = 64;

namespace net
{
    class Socket {
    public:
        static constexpr int INVALID_FD = -1;

        Socket() = default;
        explicit Socket(int fd);
        Socket(Addr addr);
        Socket(Socket&& ref) noexcept;

        Socket(const Socket&) = delete;
        ~Socket();
        void setHandle(int fd);
        int getHandle() const;
        Addr& getAddrHandle();
        const Addr& getAddrHandle() const;
        void close();

        ssize_t readSome(char *buf, size_t bufSize) const;
        ssize_t writeSome(const char* buf, size_t bufSize) const;

        bool operator == (const Socket& rhs) const;
        Socket& operator = (const Socket&) = delete;
        Socket& operator = (Socket&& rhs) noexcept;

        static constexpr int INVALID_HANDLE = -1;

    private:
        Addr addr_;
        int fd_ = -1;
    };
}


#endif //BROTLI_SOCKET_H
