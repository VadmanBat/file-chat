#include "chat-session.h"
#include "constants/config.hxx"

#include <ansi/ansi.h>
#include <net/tcp-server.h>
#include <net/tcp-socket.h>

#include <filesystem>
#include <fstream>
#include <memory>

namespace fs = std::filesystem;

int main(const int argc, char* argv[]) {
    ansi::init_colors();

    fs::create_directories(OUTBOX_DIR);
    fs::create_directories(INBOX_DIR);

    ansi::print("=== [File Chat] ===\n", ansi::Color::LightCyan);

    std::unique_ptr<net::TcpSocket> socket;

    switch (argc) {
        case 1: {
            if (net::TcpServer server; server.listen("0.0.0.0", DEFAULT_PORT)) {
                ansi::print("Auto mode: [starting] as server on port [" + std::to_string(DEFAULT_PORT) +
                            "], [waiting] for connection...\n",
                            ansi::Color::LightGreen, ansi::Color::LightGreen, ansi::Color::LightYellow);

                socket = server.acceptConnection();
                if (!socket) {
                    ansi::print("Accept [failed]\n", ansi::Color::LightRed);
                    return 1;
                }

                ansi::print("Client [connected]!\n", ansi::Color::LightGreen);
            }
            else {
                ansi::print("Auto mode: [failed] to start server (port [" + std::to_string(DEFAULT_PORT) +
                            "] busy?), [trying] as client to [127.0.0.1]\n",
                            ansi::Color::Yellow, ansi::Color::LightGreen, ansi::Color::LightYellow,
                            ansi::Color::LightBlue);

                socket = std::make_unique<net::TcpSocket>();
                if (!socket->connect("127.0.0.1", DEFAULT_PORT)) {
                    ansi::print("[Cannot] connect to [127.0.0.1]:[" + std::to_string(DEFAULT_PORT) + "]\n",
                                ansi::Color::LightRed, ansi::Color::LightBlue, ansi::Color::LightGreen);
                    return 1;
                }

                ansi::print("[Connected] to [127.0.0.1]:[" + std::to_string(DEFAULT_PORT) + "]\n",
                            ansi::Color::LightGreen, ansi::Color::LightBlue, ansi::Color::LightGreen);
            }
            break;
        }
        case 2: {
            const uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));

            net::TcpServer server;
            if (!server.listen("0.0.0.0", port)) {
                ansi::print("[Failed] to listen on port [" + std::to_string(port) + "]\n",
                            ansi::Color::LightRed, ansi::Color::LightGreen);
                return 1;
            }

            ansi::print("[Listening] on port [" + std::to_string(port) +
                        "], [waiting] for connection...\n",
                        ansi::Color::LightGreen, ansi::Color::LightGreen, ansi::Color::LightYellow);

            socket = server.acceptConnection();
            if (!socket) {
                ansi::print("Accept [failed]\n", ansi::Color::LightRed);
                return 1;
            }

            ansi::print("Client [connected]!\n", ansi::Color::LightGreen);
            break;
        }
        case 3: {
            const std::string ip = argv[1];
            const uint16_t port  = static_cast<uint16_t>(std::stoi(argv[2]));

            socket = std::make_unique<net::TcpSocket>();
            if (!socket->connect(ip, port)) {
                ansi::print("[Cannot] connect to " + ip + ":" + std::to_string(port) + "\n",
                            ansi::Color::LightRed);
                return 1;
            }

            ansi::print("[Connected] to [" + ip + "]:[" + std::to_string(port) + "]\n",
                        ansi::Color::LightGreen, ansi::Color::LightBlue, ansi::Color::LightGreen);
            break;
        }
        default: {
            ansi::print(
                "[Usage:]\n"
                "  [No args:]   auto mode (server on [50235] or client to [localhost])\n"
                "  [Server:]    [<port>]\n"
                "  [Client:]    [<ip>] [<port>]\n",
                ansi::Color::LightRed,
                ansi::Color::LightYellow, ansi::Color::LightGreen, ansi::Color::LightBlue,
                ansi::Color::LightYellow, ansi::Color::LightGreen,
                ansi::Color::LightYellow, ansi::Color::LightBlue, ansi::Color::LightGreen);
            return 1;
        }
    }

    ChatSession session(std::move(socket));
    session.run();

    ansi::print("[Goodbye!]\n", ansi::Color::LightCyan);
    return 0;
}