#include <iostream>
#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

#include "udpconn.h"

using namespace nlohmann;

struct Test {
    int num;
    std::string str;
};

void to_json (json& j, const Test& t) {
    j = json{ {"num", t.num}, {"str", t.str} };
}

int main (int argc, char* argv[]) {
    auto program = argparse::ArgumentParser{};
    int port;

    program.add_argument("port")
        .help("working udp port")
        .scan<'u', unsigned>()
        .default_value(8080)
        .store_into(port);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << program;
        return 1;
    }
    
    try {
        auto conn = UDPConn(port);

        int maxlen = 1024;
        std::cout << "started on port: " << port << std::endl;
        auto msg = std::string(maxlen, ' ');

        while (true) {
            auto[len, addr] = conn.ReadFrom(&msg[0], maxlen);
            
            std::cout << "recieved: " << len << "bytes from: " << addr << std::endl;

            conn.WriteTo("hello", 7, addr);

            // Server loop
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    
    return 0;
}