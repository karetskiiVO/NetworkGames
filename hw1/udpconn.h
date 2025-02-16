#pragma once

#include <cstdint>
#include <winsock2.h>
#include <utility>
#include <ostream>

class UDPAddr {
    sockaddr_in addr;
public:
    UDPAddr (sockaddr_in addr) : addr(addr) {}

    bool operator== (const UDPAddr& other) const;
    bool operator!= (const UDPAddr& other) const;

    friend std::ostream& operator<< (std::ostream& stream, const UDPAddr& addr);
};


class UDPConn {
    SOCKET sockfd;
public:
    UDPConn (uint16_t port);
    
    ~UDPConn ();

    std::pair<int, UDPAddr> ReadFrom (uint8_t* buffer, int size) const;
    std::pair<int, UDPAddr> ReadFrom (char* buffer, int size) const {
        return ReadFrom(reinterpret_cast<uint8_t*>(buffer), size);
    }

    void WriteTo (const uint8_t* buffer, int size, const UDPAddr& addr) const;
    void WriteTo (const char* buffer, int size, const UDPAddr& addr) const {
        return WriteTo(reinterpret_cast<const uint8_t*>(buffer), size, addr);
    }
private:

};