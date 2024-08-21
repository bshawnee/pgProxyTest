# Minimum CMake version required
cmake_minimum_required(VERSION 3.22)

project(PgProxyServer VERSION 1.0)
include_directories(".")
set(CMAKE_CXX_STANDARD 20)

add_executable(proxyPg
        main.cpp
        cli/Parser.h
        cli/Parser.cpp
        cli/Utils.cpp
        cli/Utils.h
        cli/ProxyValidator.cpp
        cli/ProxyValidator.h
        network/Socket.cpp
        network/Socket.h
        network/Addr.cpp
        network/Addr.h
        network/Proxy.cpp
        network/Proxy.h
        threads/ThreadManager.cpp
        threads/ThreadManager.h
        PgParser/Stream.cpp
        PgParser/Stream.h
        PgParser/Parser.cpp
        PgParser/Parser.h
        threads/ChanelStream.h
        threads/Observer.h
        threads/Sender.h
        PgParser/Logger.cpp
        PgParser/Logger.h
        threads/Reader.h
)
