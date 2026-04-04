#include "chat-session.h"

#include "constants/config.hxx"
#include "protocol-utils.h"
#include "file-utils.h"

#include <ansi/ansi.h>
#include <net/file-transfer.h>

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void ChatSession::handleText(const std::string& message) {
    print(get_current_time() + " [Friend:] [" + message + "]\n",
          ansi::Color::LightCyan, ansi::Color::LightGreen);
}

void ChatSession::handleFile(const std::u8string_view filename_u8) {
    const fs::path relative_path(filename_u8.begin(), filename_u8.end());
    const std::string display_path  = relative_path.string();
    const std::string original_name = relative_path.filename().string();

    const fs::path unique_path      = get_unique_path(INBOX_DIR, relative_path);
    const std::string final_display = fs::relative(unique_path, INBOX_DIR).string();

#ifdef _WIN32
    print("[Receiving file:] [{" + display_path + "}] [->] [{" + final_display + "}]\n",
          ansi::Color::LightViolet, ansi::Color::White, CP_UTF8,
          ansi::Color::LightViolet, ansi::Color::White, CP_UTF8);
#else
    print("[Receiving file:] [" + display_path + "] [->] [" + final_display + "]\n",
          ansi::Color::LightViolet, ansi::Color::White,
          ansi::Color::LightViolet, ansi::Color::White);
#endif

    progress.start("Receiving", display_path);

    const bool success = net::receive_file_with_progress(
        *socket, unique_path,
        [this](const uint64_t received, const uint64_t total) { progress.update(received, total); });

    if (success) {
        progress.finish("completed");
#ifdef _WIN32
        print("[File saved:] [{" + final_display + "}]\n",
              ansi::Color::LightGreen, ansi::Color::White, CP_UTF8);
#else
        print("[File saved:] [" + final_display + "]\n",
              ansi::Color::LightGreen, ansi::Color::White);
#endif
    }
    else {
        progress.finish("failed");
        if (fs::exists(unique_path))
            fs::remove(unique_path);
    }
}

bool ChatSession::sendText(const std::string& text) const {
    return send_message_type(*socket, MessageType::Text) &&
           send_uint64(*socket, text.size()) &&
           socket->sendBytes(std::vector<uint8_t>(text.begin(), text.end()));
}

bool ChatSession::sendFiles() {
    std::vector<fs::path> files;
    if (fs::exists(OUTBOX_DIR))
        for (const auto& entry : fs::recursive_directory_iterator(OUTBOX_DIR))
            if (entry.is_regular_file())
                files.push_back(entry.path());

    if (files.empty()) {
        print("[No files in outbox]\n", ansi::Color::LightYellow);
        return true;
    }

    print("[Sending " + std::to_string(files.size()) + " file(s)...]\n",
          ansi::Color::LightBlue);

    for (const auto& absolute_path : files) {
        const fs::path relative_path = fs::relative(absolute_path, OUTBOX_DIR);
        std::u8string relative_u8    = relative_path.u8string();

        const std::string display_path = relative_path.string();

        std::vector<uint8_t> name_bytes(relative_u8.begin(), relative_u8.end());
        if (!send_message_type(*socket, MessageType::File) ||
            !send_uint64(*socket, name_bytes.size()) ||
            !socket->sendBytes(name_bytes)) {
            return false;
        }

        progress.start("Sending", display_path, fs::file_size(absolute_path));

        const bool success = net::send_file_with_progress(
            *socket, absolute_path,
            [this](const uint64_t sent, const uint64_t total) { progress.update(sent, total); });

        if (success) {
            progress.finish("completed");
            fs::remove(absolute_path);
        }
        else {
            progress.finish("failed");
            return false;
        }
    }

    remove_empty_directories(OUTBOX_DIR);
    return true;
}