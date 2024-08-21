//
// Created by blow_job on 8/8/24.
//

#include "Utils.h"


cli::NotEnoughArgsException::NotEnoughArgsException()
    : std::invalid_argument("not enough args. use --help")
{}

cli::UnexpectedArgException::UnexpectedArgException(std::string_view arg)
    : std::invalid_argument(std::string("Not founded arg ").append(arg))
{}

cli::InvalidArgumentException::InvalidArgumentException(std::string_view arg, std::string_view value)
    : std::invalid_argument(std::string("Unexpected value for ").append(arg).append(" = ").append(value))
{}
