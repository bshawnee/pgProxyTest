//
// Created by blow_job on 8/8/24.
//

#include "ProxyValidator.h"
#include "Parser.h"
#include <charconv>
#include <iostream>

namespace {
    bool validHostPort(std::string_view arg) {
        int countNotDigit = 0;
        int countDots = 0;
        int countColons = 0;
        for (auto letter : arg) {

            if (letter == ':') {
                ++countColons;
                continue;
            }
            if (letter == '.') {
                ++countDots;
                continue;
            }
            if (!::isdigit(letter)) ++countNotDigit;
        }
        return countNotDigit == 0 && countDots == 3 && countColons == 1;
    }

    Addr getAddrFromView(std::string_view key, std::string_view data) {
        Addr addr;
        if (auto i = data.find(':'); i != std::string_view::npos) {
            addr.host = {data.data(), i};
            if(std::from_chars(data.data() + i + 1, data.data() + data.size(), addr.port).ec != std::errc{}) {
                throw cli::InvalidArgumentException(key, data);
            }
        }
        return addr;
    }
}

std::pair<Addr, Addr> cli::parseCliArguments(int argc, char **argv) {
    std::pair<Addr, Addr> upDownStream;

    cli::Parser cliParser({
        {"-u", validHostPort},
        {"-d", validHostPort},
    });

    for (const auto& [key, value] : cliParser.parse(argc, argv)) {
        if (key == "-d") {
            upDownStream.second = getAddrFromView(key, value);
        }
        else if (key == "-u") {
            upDownStream.first = getAddrFromView(key, value);
        }
    }
    return upDownStream;
}
