//
// Created by blow_job on 8/8/24.
//
#include "Parser.h"

cli::Parser::Parser(cli::ValidatorSet_t validators)
    : validators_(std::move(validators))
{}

const cli::Arguments_t& cli::Parser::parse(int argc, char** argv)
{
    if (argc < 2) throw NotEnoughArgsException();

    for (int i = 1; i < argc; ++i) {

        if (i + 1 == argc) throw NotEnoughArgsException();

        int prev = i;
        args_[argv[prev]] = argv[++i];
    }

    validate();
    return args_;
}

void cli::Parser::validate()
{
    for (const auto& [name, checkFunc] : validators_) {
        try {
            const auto& value = args_.at(name);
            if (!checkFunc(value)) {
                throw InvalidArgumentException(name, value);
            }
        }
        catch (const std::out_of_range&) {
            throw UnexpectedArgException(name);
        }

    }
}

