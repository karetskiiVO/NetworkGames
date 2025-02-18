#include <iostream>
#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>
#include <set>

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
        auto buffer = new char[maxlen];

        auto users = std::set<UDPAddr>();
        auto mathDuel = std::set<UDPAddr>(); 
        std::string answ;

        while (true) {
            auto[len, addr] = conn.ReadFrom(buffer, maxlen);
            if (len == 0) continue;

            buffer[len] = '\0';
            std::cout << addr << " msg[" << len << "]: " << buffer << std::endl;

            auto spacepos = strchr(buffer, ' ');
            if (spacepos == nullptr) spacepos = buffer + len;
            
            auto command = std::string(buffer, spacepos);
            auto msg = std::string(spacepos + 1);
            
            if (command == "/hello") {
                users.insert(addr);
            }

            if (command == "/c") {
                for (const auto& user : users) {
                    if (user == addr) continue;
                    
                    conn.WriteTo(msg.c_str(), msg.length(), user);
                }
            }

            if (command == "/mathduel") {
                switch (mathDuel.size()) {
                case 0: {
                    const char* mathDuelBeginningMsg = "math duel begins";
                    mathDuel.insert(addr);

                    for (const auto& user : users) {
                        if (mathDuel.find(user) != mathDuel.end()) continue;
                        
                        conn.WriteTo(mathDuelBeginningMsg, strlen(mathDuelBeginningMsg), user);
                    }

                    break;
                }
                case 1: {
                    const char* mathDuelBeginningMsg = "math duel begins";
                    mathDuel.insert(addr);
                    
                    for (const auto& user : users) {
                        if (mathDuel.find(user) != mathDuel.end()) continue;

                        conn.WriteTo(mathDuelBeginningMsg, strlen(mathDuelBeginningMsg), user);
                    }

                    int fst = rand() % 10;
                    int snd = rand() % 10;

                    answ = std::to_string(fst + snd);

                    std::string task = std::to_string(fst) + " + " + std::to_string(snd);

                    for (const auto& user : mathDuel) {
                        conn.WriteTo(task.c_str(), task.length(), user);
                    }
                    break;
                }
                default:
                    break;
                }
            }

            if (command == "/ans") {
                // TODO: reply to all players
                if (mathDuel.size() != 2) continue;
                if (mathDuel.find(addr) == mathDuel.end()) continue;
                if (msg != answ) continue;

                mathDuel.erase(addr);
                const char* winMsg  = "you win";
                const char* loseMsg = "you loose";

                conn.WriteTo(winMsg, strlen(winMsg), addr);
                conn.WriteTo(loseMsg, strlen(loseMsg), *mathDuel.begin());

                mathDuel.clear();
            }
        }

        delete[] buffer;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    
    return 0;
}