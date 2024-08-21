//
// Created by blow_job on 8/8/24.
//

#ifndef BROTLI_UTILS_H
#define BROTLI_UTILS_H
#include <stdexcept>
#include <unordered_set>
#include <functional>

namespace cli
{
    class NotEnoughArgsException : public std::invalid_argument
    {
    public:
        NotEnoughArgsException();
    };

    class UnexpectedArgException : public std::invalid_argument
    {
    public:
        UnexpectedArgException(std::string_view arg);

    };
    class InvalidArgumentException : public std::invalid_argument
    {
    public:
        InvalidArgumentException(std::string_view arg, std::string_view value);
    };
    struct Validator {
        std::string argName;
        std::function<bool(std::string_view)> validateFunc;
        bool operator == (const Validator& rhs) const {
            return argName == rhs.argName;
        }
    };

    struct Hasher {
        size_t operator() (const Validator& value) const {
            return std::hash<std::string>{}(value.argName);
        }
    };

    using ValidatorSet_t = std::unordered_set<Validator, Hasher>;
    using Arguments_t = std::unordered_map<std::string_view, std::string_view>;

}

#endif //BROTLI_UTILS_H
