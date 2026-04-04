#include "chat-session.h"

#include "constants/config.hxx"
#include "protocol-utils.h"

#include <ansi/ansi.h>
#include <net/file-transfer.h>

#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

constexpr uint64_t ChatSession::hash(const char* str) {
    uint64_t hash = 5381;
    unsigned char c;
    while ((c = static_cast<unsigned char>(*str++)) != 0)
        hash = ((hash << 5) + hash) + c;
    return hash;
}

void ChatSession::run() {
    receive_thread = std::thread(&ChatSession::receiveLoop, this);

    std::string line;
    while (connected && std::getline(std::cin, line)) {
        if (line.empty())
            continue;

        if (line[0] != '/') {
            print(get_current_time() + " [Myself:] [" + line + "]\n",
                  ansi::Color::LightCyan, ansi::Color::LightBlue);

            if (!sendText(line))
                connected = false;
            continue;
        }

        switch (hash(line.c_str())) {
            case hash("/quit"): {
                connected = false;
                break;
            }
            case hash("/send"): {
                if (!sendFiles())
                    connected = false;
                continue;
            }
            case hash("/list"): {
                printDirectoryContents("Outbox (waiting to send):", OUTBOX_DIR);
                printDirectoryContents("Inbox (received):", INBOX_DIR);
                continue;
            }
            case hash("/clear"): {
                {
                    std::lock_guard<std::mutex> lock(io_mutex);
                    ansi::clear();
                }
                continue;
            }
            default: {
                print(
                    "[Help:]\n"
                    "  /help        - show this help\n"
                    "  /send        - send all files from outbox\n"
                    "  /list        - show files in outbox/inbox\n"
                    "  /clear       - clear screen\n"
                    "  /quit        - exit\n"
                    "  <text>       - send text message\n",
                    ansi::Color::LightYellow);
                break;
            }
        }
    }

    connected = false;
    socket->disconnect();
    if (receive_thread.joinable())
        receive_thread.join();
}

void ChatSession::receiveLoop() {
    while (connected) {
        MessageType type;
        if (!receive_message_type(*socket, type)) {
            shutdown();
            break;
        }

        uint64_t len;
        if (!receive_uint64(*socket, len)) {
            shutdown();
            break;
        }

        std::vector<uint8_t> data(len);
        if (!receive_exact(*socket, data.data(), len)) {
            shutdown();
            break;
        }

        switch (type) {
            case MessageType::Text: {
                const std::string message(data.begin(), data.end());
                handleText(message);
                break;
            }
            case MessageType::File: {
                std::u8string filename_u8(reinterpret_cast<const char8_t*>(data.data()), len);
                handleFile(filename_u8);
                break;
            }
        }
    }
}