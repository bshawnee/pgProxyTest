//
// Created by blow_job on 8/9/24.
//

#include "Socket.h"
#include <stdexcept>
#include <utility>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace net
{
    Socket::Socket(Socket &&ref) noexcept
    {
        fd_ = ref.fd_;
        addr_ = ref.addr_;
        ref.fd_ = INVALID_FD;
    }

    Socket::Socket(Addr addr)
        : addr_(addr)
    {
        if (fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); fd_ < 0) {
            throw std::runtime_error("cant open socket");
        }
        int reuse = 1;
        if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)) < 0) {
            throw std::runtime_error("cant set socket option");
        }

        if (bind(fd_, addr_.getHandle(), sizeof(sockaddr_in)) < 0) {
            close();
            throw std::runtime_error("cant bind socket");
        }
        if (listen(fd_, MAX_QUEUE) < 0) {
            close();
            throw std::runtime_error("cant listen addr");
        }
    }

    void Socket::close()
    {
        ::close(fd_);
        fd_ = INVALID_FD;
    }


    int Socket::getHandle() const {
        return fd_;
    }

    ssize_t Socket::readSome(char *buf, size_t bufSize) const {
        return recv(fd_, buf, bufSize, 0);
    }

    ssize_t Socket::writeSome(const char *buf, size_t bufSize) const {
        return send(fd_, buf, bufSize, 0);
    }

    void Socket::setHandle(int fd)
    {
        fd_ = fd;
    }

    Socket::Socket(int fd)
        : fd_(fd)
    {}

    Socket::~Socket()
    {
        if (fd_ != INVALID_FD) { // if closed manually
            close();
        }
    }

    bool Socket::operator==(const Socket &rhs) const {
        return fd_ == rhs.fd_;
    }

    Addr &Socket::getAddrHandle() {
        return addr_;
    }

    const Addr &Socket::getAddrHandle() const {
        return addr_;
    }

    Socket &Socket::operator=(Socket &&rhs) noexcept {
        fd_ = rhs.fd_;
        rhs.fd_ = INVALID_FD;
        addr_ = rhs.addr_;
        return *this;
    }
}

