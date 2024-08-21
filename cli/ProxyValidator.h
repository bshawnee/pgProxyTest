//
// Created by blow_job on 8/8/24.
//

#ifndef BROTLI_PROXYVALIDATOR_H
#define BROTLI_PROXYVALIDATOR_H
#include <string_view>
#include <optional>

struct Addr
{
    std::string_view host;
    uint16_t port;
};

namespace cli {
    std::pair<Addr, Addr> parseCliArguments(int argc, char **argv);
}


#endif //BROTLI_PROXYVALIDATOR_H
