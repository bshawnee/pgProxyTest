#include <iostream>
#include <string>
#include "cli/ProxyValidator.h"
#include "network/Socket.h"
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unordered_map>
#include "network/Proxy.h"

static constexpr size_t MAX_EVENTS = 32;

int main(int argc, char** argv) {
    try {
        auto [upstream, downstream] = cli::parseCliArguments(argc, argv);
        net::Proxy serv(
            net::Addr(upstream.host, upstream.port),
            net::Addr(downstream.host, downstream.port)
        );

        serv.listen();
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl
        << "Help: " << std::endl
        << "use -u [host_:port] for upstream (proxy => server)" << std::endl
        << "use -d [host_:port] for downstream (client => proxy)" << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}