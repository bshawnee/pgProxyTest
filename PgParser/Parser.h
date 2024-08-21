//
// Created by blow_job on 8/21/24.
//

#ifndef PGPROXYSERVER_PARSER_H
#define PGPROXYSERVER_PARSER_H
#include "threads/ThreadManager.h"
#include "Stream.h"
#include "threads/Sender.h"

enum class PGMessageType {
    Query = 'Q',
    Parse = 'P',
};

enum class ParsingState {
    MsgType,
    MsgLen,
    MsgData,
    MsgStartup,
};
class Parser {
public:
    Parser(chanelBufPtr_t ptr);
    Task::TaskStatus parse();
    const std::unique_ptr<Stream>& getStream() const;
    std::unique_ptr<Stream>& getStream();
    const std::string& getHost() const;
    std::string& getHost();

    static Task::TaskStatus execTask(std::shared_ptr<Parser> parser);


private:
    ParsingState state_ = ParsingState::MsgStartup;
    char msgType_ = 0;
    uint32_t messageLen_ = 0;
    std::string host_;
    std::unique_ptr<Stream> dataStream_ = std::make_unique<Stream>();
    Sender<Notify> sender_;
};


#endif //PGPROXYSERVER_PARSER_H
