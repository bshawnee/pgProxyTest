//
// Created by blow_job on 8/8/24.
//

#ifndef BROTLI_PARSER_H
#define BROTLI_PARSER_H
#include "Utils.h"

namespace cli {
    class Parser {
    public:
        Parser(ValidatorSet_t validators);

        const Arguments_t& parse(int argc, char** argv);
    private:
        void validate();
        ValidatorSet_t validators_;
        Arguments_t args_;
    };


}

#endif //BROTLI_PARSER_H
