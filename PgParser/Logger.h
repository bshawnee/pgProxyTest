//
// Created by blow_job on 8/21/24.
//

#ifndef PGPROXYSERVER_LOGGER_H
#define PGPROXYSERVER_LOGGER_H
#include <string>
#include <fstream>
#include "threads/Reader.h"
#include "threads/ThreadManager.h"

class Logger {
public:
    Logger(const std::string& logFile, chanelBufPtr_t ptr);
    Task::TaskStatus start();
private:
    std::ofstream file_;
    Reader<Notify> reader_;
};


#endif //PGPROXYSERVER_LOGGER_H
