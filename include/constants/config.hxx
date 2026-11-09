#pragma once
// Created by Vadim on 04.04.2026.

#include <cstdint>

inline constexpr auto OUTBOX_DIR       = "outbox";
inline constexpr auto INBOX_DIR        = "inbox";
inline constexpr uint16_t DEFAULT_PORT = 50235;

enum class MessageType : uint8_t {
    Text = 0,
    File = 1
};