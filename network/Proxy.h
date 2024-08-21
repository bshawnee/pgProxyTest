//
// Created by blow_job on 8/19/24.
//

#ifndef BROTLI_IOSERVICE_H
#define BROTLI_IOSERVICE_H
#include "Socket.h"
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>
#include <chrono>
#include <vector>
#include "PgParser/Stream.h"
#include "threads/ThreadManager.h"
#include <iostream>
#include "PgParser/Parser.h"
#include "PgParser/Logger.h"

namespace net {
    namespace utils {

        struct SockHasher
        {
            size_t operator () (const net::Socket& socket) const;
        };
    }

    using namespace std::chrono;

    static constexpr int DEFAULT_Q_SIZE = 32;
    static constexpr std::chrono::milliseconds DEFAULT_TIMEOUT = 1000ms;

    class Proxy
    {
    public:
        Proxy(Addr listenUp, Addr listenDown, int queueSize = DEFAULT_Q_SIZE, std::chrono::milliseconds timeout = DEFAULT_TIMEOUT);
        void addListener(const Socket& listen, int flags = EPOLLIN | EPOLLERR | EPOLLHUP) const;

        epoll_event* listen();



    private:

        int qSize_;
        std::chrono::milliseconds timeout_;
        Addr addrUp_;
        Socket listenerDown_;
        std::unique_ptr<epoll_event[]> events_;
        int epollFd_ = -1;
        std::unordered_map<int, Socket> clientDb_;
        std::unordered_map<int, Socket> dbClient_;

        std::unordered_map<int, std::shared_ptr<Parser>> parsers_;
        std::unordered_map<int, int64_t> associatedTask_;
        ThreadManager tm_;
        chanelBufPtr_t buf_;
        Logger log_;
        Sender<Notify> sender_;
    };

} // net

#endif //BROTLI_IOSERVICE_H
