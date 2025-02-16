#include "udpconn.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include <cstring>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

std::ostream& operator<< (std::ostream& stream, const UDPAddr& addr) {
    const uint8_t* ipbytes = reinterpret_cast<const uint8_t*>(&addr.addr.sin_addr);

    stream << unsigned(ipbytes[0]) << "." 
           << unsigned(ipbytes[1]) << "." 
           << unsigned(ipbytes[2]) << "."
           << unsigned(ipbytes[3]) << ":" 
           << unsigned(htons(addr.addr.sin_port));

    return stream;
}

UDPConn::UDPConn (uint16_t port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) throw std::runtime_error("can't wsa startup");
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) throw std::runtime_error("can't create a socket " + std::to_string(sockfd));
    
    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    // Настройка адреса сервера
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    std::cout << servaddr.sin_port << "\n";

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)) == SOCKET_ERROR) throw std::runtime_error("can't bind socket");
}

UDPConn::~UDPConn () {
    closesocket(sockfd);
}

std::pair<int, UDPAddr> UDPConn::ReadFrom(uint8_t *buffer, int size) const {
    sockaddr_in clientaddr = {0};
    int clientaddrLen = sizeof(clientaddr);
    int n  = recvfrom(sockfd, reinterpret_cast<char*>(buffer), size, 0, reinterpret_cast<sockaddr*>(&clientaddr), &clientaddrLen);
    
    if (n == SOCKET_ERROR) throw std::runtime_error("recieve error");
    return {n, UDPAddr{clientaddr}};
}

void UDPConn::WriteTo (const uint8_t* buffer, int size, const UDPAddr& addr) const {
    if (sendto(sockfd, reinterpret_cast<const char*>(buffer), size, 0, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
        throw std::runtime_error("can't write");
}

bool UDPAddr::operator== (const UDPAddr& other) const {
    return memcmp(&(this->addr), &(other.addr), sizeof(sockaddr_in));
}

bool UDPAddr::operator!= (const UDPAddr& other) const {
    return !(*this == other);
}
