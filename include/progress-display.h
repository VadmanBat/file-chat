#pragma once
// Created by Vadim on 04.04.2026.

#include <mutex>
#include <string>
#include <cstdint>

class ProgressDisplay {
    std::recursive_mutex mutex;
    std::mutex& io_mutex;

    bool active = false;
    std::string prefix;
    std::string highlight;
    uint64_t total = 0;

    static std::string format_bytes(uint64_t bytes);

public:
    explicit ProgressDisplay(std::mutex& io_mut);

    void start(const std::string& prefix_text,
               const std::string& highlight_text = "",
               uint64_t known_total              = 0);

    void update(uint64_t sent, uint64_t new_total = 0);
    void finish(const std::string& status = "completed");
};