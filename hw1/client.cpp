#include <iostream>
#include <argparse/argparse.hpp>
#include <thread>

#include "udpconn.h"

UDPConn* conn = nullptr;

void networkInputThread ();

int main (int argc, char* argv[]) {
    auto program = argparse::ArgumentParser{};
    int port;
    std::string serverAddr;

    program.add_argument("port")
        .help("working udp port")
        .nargs(1)
        .scan<'u', unsigned>();
    program.add_argument("addr")
        .nargs(1)
        .help("server addr");

        
    try {
        program.parse_args(argc, argv);

        port = program.get<unsigned>("port");
        serverAddr = program.get<std::string>("addr");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    try {
        conn = new UDPConn(port);
        auto addr = UDPAddr(serverAddr);

        std::thread(networkInputThread).detach();

        auto handshakeMsg = "/hello ";

        conn->WriteTo(handshakeMsg, strlen(handshakeMsg), addr);

        while (true) {
            std::string buffer;
            std::getline(std::cin, buffer);

            conn->WriteTo(buffer.c_str(), buffer.length(), addr);
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void networkInputThread () {
    int maxlen = 1024;
    auto buffer = new char[maxlen];
    
    while (true) {
        auto[len, addr] = conn->ReadFrom(buffer, maxlen);
        buffer[len] = '\0';

        std::cout << buffer << std::endl;
        std::cout.flush();
    }
}