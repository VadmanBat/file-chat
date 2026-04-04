#pragma once

#include "progress-display.h"

#include <ansi/ansi.h>
#include <net/tcp-socket.h>

#include <atomic>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>

namespace fs = std::filesystem;

class ChatSession {
    std::unique_ptr<net::TcpSocket> socket;
    std::atomic<bool> connected{true};
    std::thread receive_thread;
    ProgressDisplay progress;
    std::mutex io_mutex;

    template <typename... Args>
    void print(Args&&... args) {
        std::lock_guard<std::mutex> lock(io_mutex);
        ansi::print(std::forward<Args>(args)...);
    }

    static constexpr uint64_t hash(const char* str);
    static std::string get_current_time();

    void printDirectoryContents(const std::string& title,
                                const fs::path& dir,
                                ansi::Color title_color = ansi::Color::LightViolet);
    void shutdown();

    void receiveLoop();

    void handleText(const std::string& message);
    void handleFile(std::u8string_view filename_u8);

    bool sendText(const std::string& text) const;
    bool sendFiles();

public:
    explicit ChatSession(std::unique_ptr<net::TcpSocket> sock);
    ~ChatSession();

    void run();
};