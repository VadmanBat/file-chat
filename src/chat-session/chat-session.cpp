#include "chat-session.h"

#include "file-utils.h"

ChatSession::ChatSession(std::unique_ptr<net::TcpSocket> sock)
    : socket(std::move(sock)), progress(io_mutex) {
}

ChatSession::~ChatSession() {
    if (receive_thread.joinable())
        receive_thread.join();
}

std::string ChatSession::get_current_time() {
    const auto now = std::chrono::system_clock::now();
    const auto tt  = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&tt), "%H:%M:%S");
    return ss.str();
}

void ChatSession::shutdown() {
    connected = false;
    print("[Connection lost]\n", ansi::Color::LightRed);
}

void ChatSession::printDirectoryContents(const std::string& title,
                                         const fs::path& dir,
                                         ansi::Color title_color) {
    const auto files = get_sorted_relative_files(dir);

    print("[" + title + "]\n", title_color);

    if (files.empty())
        print("  ([empty])\n", ansi::Color::LightBlack);
    else
        for (const auto& rel_path : files) {
#ifdef _WIN32
            print("  - {" + rel_path.string() + "}\n", CP_UTF8);
#else
            print("  - " + rel_path.string() + "\n");
#endif
        }
}