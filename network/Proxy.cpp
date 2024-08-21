//
// Created by blow_job on 8/19/24.
//

#include "Proxy.h"
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <list>
#include "threads/Sender.h"

size_t net::utils::SockHasher::operator()(const Socket &socket) const
{
    return static_cast<size_t>(socket.getHandle());
}


namespace net {
    Proxy::Proxy(Addr listenUp, Addr listenDown, int queueSize, std::chrono::milliseconds timeout)
        : qSize_(queueSize)
        , timeout_(timeout)
        , addrUp_(std::move(listenUp))
        , listenerDown_(std::move(listenDown))
        , tm_(std::thread::hardware_concurrency())
        , buf_(std::make_shared<chanelBuf_t>())
        , log_("query_log.tsv", buf_)
    {
        if (epollFd_ = epoll_create(qSize_); epollFd_ < 0) {
            throw std::runtime_error("cant init epoll");
        }

        events_ = std::make_unique<epoll_event[]>(qSize_);
        addListener(listenerDown_);
    }

    void Proxy::addListener(const Socket& listen, int flags) const
    {
        epoll_event event = {};
        event.data.fd = listen.getHandle();
        event.events = flags;
        epoll_ctl(epollFd_, EPOLL_CTL_ADD, listen.getHandle(), &event);
    }

    epoll_event* Proxy::listen() {
        char buffer[1024] = {};
        sender_.init(buf_);
        sender_ << observable_t {"from, query"};
        tm_.add(&Logger::start, &log_);
        std::cout << "run server" << std::endl;
        while (true) {
            int eventCount = epoll_wait(epollFd_, events_.get(), qSize_, timeout_.count());
            if (eventCount == -1) {
                throw std::runtime_error("Internal error: epoll_wait");
            }
            if (eventCount == 0) {
                continue;
            }
            for (auto [fd, taskId] : associatedTask_) {

                auto status = tm_.getResult(taskId);
                if (status == Task::TaskStatus::tsDeffer) {
                    auto& parser = parsers_[fd];
                    tm_.add(taskId, &Parser::execTask, parser);
                }
            }
            for (int i = 0; i < eventCount; i++) {

                // TODO: handle error
                if (events_[i].events & (EPOLLHUP | EPOLLERR)) {
                    printf("error epoll event\n");
                    continue;
                }

                if (!(events_[i].events & EPOLLIN)) continue;

                // TODO: accept new connection client => proxy
                if (events_[i].data.fd == listenerDown_.getHandle()) {
                    Socket clientSocket;
                    socklen_t len = sizeof(sockaddr_in);
                    if (int fd = accept(listenerDown_.getHandle(), clientSocket.getAddrHandle().getHandle(), &len); fd != -1) {

                        if (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0)) == -1) {
                            throw std::runtime_error("internal error: fcntl");
                        }
                        clientSocket.setHandle(fd);
                    }
                    else {
                        continue;
                    }
                    // threads
                    auto& parser = parsers_[clientSocket.getHandle()];
                    parser = std::make_shared<Parser>(buf_);
                    auto view = clientSocket.getAddrHandle().hostToStr();
                    parser->getHost() = {view.begin(), view.end()};
                    parser->getHost().append(":").append(std::to_string(clientSocket.getAddrHandle().getPort()));
                    associatedTask_[clientSocket.getHandle()] = tm_.add(&Parser::execTask, parser);

                    addListener(clientSocket);
                    // creating connection proxy => db
                    Socket dbSocket(socket(AF_INET, SOCK_STREAM, 0));
                    dbSocket.getAddrHandle() = addrUp_;
                    if (connect(dbSocket.getHandle(), dbSocket.getAddrHandle().getHandle(), sizeof (sockaddr_in)) < 0) {
                        throw std::runtime_error(std::string("cant connect to db: ").append(strerror(errno)));
                    }
                    addListener(dbSocket);
                    int tempFd = dbSocket.getHandle();
                    clientDb_[clientSocket.getHandle()] = std::move(dbSocket);
                    dbClient_[tempFd] = std::move(clientSocket);
                    continue;
                }
                Socket src(events_[i].data.fd);
                Socket* target = nullptr;
                bool isClient = false;
                if (auto it = dbClient_.find(src.getHandle()); it != clientDb_.end()) {
                    target = &(it->second);
                }
                if (auto it = clientDb_.find(src.getHandle()); it != dbClient_.end()) {
                    target = &(it->second);
                    isClient = true;
                }
                if (target == nullptr) {

                    throw std::logic_error("internal error:");
                }
                // TODO: read send
                ssize_t consumedBytes = 0;
                consumedBytes = src.readSome(buffer, std::size(buffer));
                if (consumedBytes > 0) {
                    if (isClient) {
                        parsers_.at(src.getHandle())->getStream()->pushBack(buffer, consumedBytes);
                       /* std::string_view test(buffer, consumedBytes);
                        if (test.find("select") != std::string_view::npos) {
                            std::cout << test << std::endl;
                        }*/
                    }

                    target->writeSome(buffer, consumedBytes);
                }
                if (consumedBytes == 0) {
                    printf("connection closed\n");
                    epoll_ctl(epollFd_, EPOLL_CTL_DEL, src.getHandle(), nullptr);
                    epoll_ctl(epollFd_, EPOLL_CTL_DEL, target->getHandle(), nullptr);
                    int clientFd = isClient ? src.getHandle() : target->getHandle();
                    associatedTask_.erase(clientFd);
                    auto dbFd = clientDb_.at(clientFd).getHandle();
                    parsers_.erase(clientFd);
                    clientDb_.erase(clientFd);
                    dbClient_.erase(dbFd);
                }

                src.setHandle(Socket::INVALID_FD);// disable close(fd)
            }
        }
    }

} // net