#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

void receive_handler(SOCKET sock)
{
    char buf[1024];
    std::string buffer;

    while (true) {
        int ret = recv(sock, buf, sizeof(buf) - 1, 0);

        if (ret > 0) {
            buf[ret] = '\0';
            buffer += buf;

            size_t pos;
            while ((pos = buffer.find('\n')) != std::string::npos) {

                std::string line = buffer.substr(0, pos + 1);
                buffer.erase(0, pos + 1);

                std::cout << "\x1b[0m";  // 色リセット

                if (line.find(">>") != std::string::npos)
                    std::cout << "\x1b[38;2;255;230;0m";
                else if (line.find("プレイヤー") != std::string::npos)
                    std::cout << "\x1b[36m";
                else if (line.find("スコア") != std::string::npos)
                    std::cout << "\x1b[32m";

                std::cout << line;
                std::cout << "\x1b[0m";
            }

            std::cout << "> " << std::flush;
        }
        else if (ret == 0) {
            std::cout << "\n[SYSTEM] Server closed connection.\n";
            exit(0);
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
    inet_pton(AF_INET, "192.168.42.13", &addr.sin_addr);

    if (connect(sock, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "接続失敗\n";
        return 1;
    }

    std::thread(receive_handler, sock).detach();

    std::cout << "--- SPEED TYPING GAME CLIENT ---\n";

    while (true) {
        std::string input;
        if (!std::getline(std::cin, input)) break;

        if (!input.empty())
            send(sock, input.c_str(), (int)input.size(), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}