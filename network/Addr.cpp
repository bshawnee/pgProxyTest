//
// Created by blow_job on 8/10/24.
//

#include "Addr.h"
#include <string>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>

namespace net {
    sockaddr *Addr::getHandle() {
        return reinterpret_cast<sockaddr*>(&address_);
    }

    Addr::Addr(std::string_view host, uint16_t port) {
        std::string temp(host.begin(), host.end());
        address_ = {
                .sin_family = PF_INET,
                .sin_port = htons(port),
                .sin_addr = {inet_addr(temp.c_str())},
        };
        if (address_.sin_addr.s_addr < 0) {
            throw std::invalid_argument("Invalid host_:port");
        }
    }

    std::string_view Addr::hostToStr() {
        if (const auto* ptr = inet_ntop(AF_INET, &(address_.sin_addr), ipStr_, INET_ADDRSTRLEN); ptr != nullptr) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "LocalValueEscapesScope"
            return {ptr};
#pragma clang diagnostic pop
        }
        throw std::runtime_error("invalid addr");
    }

    Addr::Addr(const Addr & rhs) {
        *this = rhs;
    }

    Addr::Addr(Addr && rhs) noexcept {
        *this = std::move(rhs);
    }

    Addr &Addr::operator=(const Addr &rhs) {
        std::memcpy(&address_, &rhs.address_, sizeof(sockaddr_in));
        std::memcpy(ipStr_, rhs.ipStr_, INET_ADDRSTRLEN);
        bzero(address_.sin_zero, std::size(address_.sin_zero));
        return *this;
    }

    Addr &Addr::operator=(Addr &&rhs) noexcept {
        std::memcpy(&address_, &rhs.address_, sizeof(sockaddr_in));
        std::memcpy(ipStr_, rhs.ipStr_, INET_ADDRSTRLEN);
        bzero(address_.sin_zero, std::size(address_.sin_zero));
        return *this;
    }

    uint16_t Addr::getPort() const {
        return ntohs(address_.sin_port);
    }
} // net