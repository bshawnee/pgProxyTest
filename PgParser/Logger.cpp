//
// Created by blow_job on 8/21/24.
//

#include "Logger.h"

Logger::Logger(const std::string &logFile, chanelBufPtr_t ptr)
    : file_(logFile, std::ofstream::trunc | std::ofstream ::out)
{
    reader_.init(std::move(ptr));
}

Task::TaskStatus Logger::start() {
    while (!reader_.eof()) {
        observable_t notification;
        reader_ >> notification;
        file_ << notification.notification << std::endl;
    }
    return Task::TaskStatus::tsExitSuccess;
}

