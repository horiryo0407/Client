#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

void receive_handler(SOCKET sock) {
    char buf[1024];
    while (true) {
        int ret = recv(sock, buf, sizeof(buf) - 1, 0);
        if (ret > 0) {
            buf[ret] = '\0';//
            // 受信したログをそのまま表示。 \n が含まれているので自動で改行されます。
            std::cout << buf << "> " << std::flush; //
        }
        else if (ret == 0) {
            std::cout << "\n[SYSTEM] Server closed the connection.\n";
            exit(0);
        }
        else {
            if (WSAGetLastError() != WSAEWOULDBLOCK) exit(0);
        }
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Error: Could not connect to server.\n";
        return 1;
    }

    // 受信専用スレッドを起動
    std::thread(receive_handler, sock).detach();

    std::cout << "--- SPEED TYPING GAME CLIENT ---\n";

    while (true) {
        std::string input;
        if (!(std::getline(std::cin, input))) break;

        if (!input.empty()) {
            send(sock, input.c_str(), static_cast<int>(input.size()), 0);
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}