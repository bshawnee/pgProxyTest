//
// Created by blow_job on 8/10/24.
//

#ifndef BROTLI_ADDR_H
#define BROTLI_ADDR_H
#include <string_view>
#include <netinet/in.h>

namespace net {

    class Addr {
    public:
        Addr() = default;
        Addr(const Addr& rhs);
        Addr(Addr&& rhs) noexcept;

        Addr& operator = (const Addr& rhs);
        Addr& operator = (Addr&& rhs) noexcept;
        Addr(std::string_view host, uint16_t port);

        sockaddr* getHandle();
        std::string_view hostToStr();
        uint16_t getPort() const;

    private:
        sockaddr_in address_ = {};
        char ipStr_[INET_ADDRSTRLEN] = {};
    };

} // net

#endif //BROTLI_ADDR_H
