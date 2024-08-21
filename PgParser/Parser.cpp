//
// Created by blow_job on 8/21/24.
//

#include <netinet/in.h>
#include "Parser.h"
#include <iostream>

Task::TaskStatus Parser::parse()
{
    if (dataStream_ == nullptr) {
        return Task::TaskStatus::tsDeffer;
    }

    auto prevState = state_;
    while (true) {
        switch(state_) {

            case ParsingState::MsgStartup:
            case ParsingState::MsgLen: {
                if (auto data = dataStream_->consumeBytes(4); data.has_value()) {
                    messageLen_ = ntohl(*reinterpret_cast<const uint32_t*>(data->data())) - 4;
                }
                else {
                    return Task::TaskStatus::tsDeffer;
                }
                state_ = ParsingState::MsgData;

                break;
            }
            case ParsingState::MsgType: {
                if (auto data = dataStream_->consumeBytes(1); data.has_value()) {
                    msgType_ = data->front();
                }
                else {
                    return Task::TaskStatus::tsDeffer;
                }
                state_ = ParsingState::MsgLen;
                break;
            }
            case ParsingState::MsgData: {
                if (auto data = dataStream_->consumeBytes(messageLen_); data.has_value()) {
                    if (messageLen_ == 4 && prevState == ParsingState::MsgStartup) {
                        auto sslCode = static_cast<int32_t>(ntohl(*reinterpret_cast<const int32_t*>(data->data())));
                        if (sslCode == 80877103) { // ssl request code
                            state_ = ParsingState::MsgStartup;
                            break;
                        }
                    }
                    switch (static_cast<PGMessageType>(msgType_)) {
                        case PGMessageType::Parse : {
                            auto it = data->begin();
                            while (*it == 0) {
                                it++;
                            }
                            auto itEnd = data->rbegin();
                            while (*itEnd == 0) {
                                itEnd++;
                            }
                            std::string_view msg = {it, itEnd.base()};
                            sender_ << observable_t {std::string(host_ + ("\t")).append(msg)};
                            break;
                        }
                        case PGMessageType::Query: {
                            std::string_view msg = {data->begin(), data->end()};
                            sender_ << observable_t {std::string(host_ + ("\t")).append(msg)};
                            break;
                        }
                        default:
                            break;
                    }
                    state_ = ParsingState::MsgType;
                }
                else {
                    return Task::TaskStatus::tsDeffer;
                }
                break;
            }
        }
    }
}


const std::string &Parser::getHost() const {
    return host_;
}

std::string &Parser::getHost() {
    return host_;
}

Task::TaskStatus Parser::execTask(std::shared_ptr<Parser> parser) {
    return parser->parse();
}

const std::unique_ptr<Stream> &Parser::getStream() const {
    return dataStream_;
}

std::unique_ptr<Stream> &Parser::getStream() {
    return dataStream_;
}

Parser::Parser(chanelBufPtr_t ptr) {
    sender_.init(std::move(ptr));
}
