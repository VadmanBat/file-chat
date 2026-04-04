#include "progress-display.h"

#include <ansi/ansi.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <windows.h>

ProgressDisplay::ProgressDisplay(std::mutex& io_mut) : io_mutex(io_mut) {
}

void ProgressDisplay::start(const std::string& prefix_text,
                            const std::string& highlight_text,
                            const uint64_t known_total) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    prefix    = prefix_text;
    highlight = highlight_text;
    total     = known_total;
    active    = true;

    update(0, known_total);
}

void ProgressDisplay::update(const uint64_t sent, const uint64_t new_total) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (!active)
        return;

    if (new_total > 0)
        total = new_total;

    const double percent       = total > 0 ? (static_cast<double>(sent) / static_cast<double>(total)) * 100.0 : 0.0;
    constexpr size_t bar_width = 40;
    const auto filled          = static_cast<size_t>(percent * bar_width / 100.0);

    std::string bar(filled, '#');
    bar += "][";
    bar.append(bar_width - filled, '-');

    std::ostringstream oss;
    oss << prefix << ' ';

    if (highlight.empty())
        oss << ": ";
    else {
#ifdef _WIN32
        oss << '{' << highlight << "} : ";
#else
        oss << highlight << " : ";
#endif
    }

    oss << "^[[" << bar << "]^] " << std::fixed << std::setprecision(1) << percent << '%';

    if (total > 0)
        oss << " (" << format_bytes(sent) << " / " << format_bytes(total) << ')';
    else
        oss << " (" << format_bytes(sent) << ')';

    {
        std::lock_guard<std::mutex> io_lock(io_mutex);
#ifdef _WIN32
        ansi::print("\r" + oss.str(),
                    CP_UTF8,
                    ansi::Color::LightGreen,
                    ansi::Color::LightYellow);
#else
        ansi::print("\r" + oss.str(),
                    ansi::Color::LightGreen,
                    ansi::Color::LightYellow);
#endif
        fflush(stdout);
    }
}

void ProgressDisplay::finish(const std::string& status) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (!active)
        return;

    update(total, total);

    {
        std::lock_guard<std::mutex> io_lock(io_mutex);
        std::cout << " - " << status << std::endl;
    }

    active = false;
}

std::string ProgressDisplay::format_bytes(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit            = 0;
    auto size           = static_cast<double>(bytes);

    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        ++unit;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << ' ' << units[unit];
    return oss.str();
}