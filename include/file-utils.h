#pragma once
// Created by Vadim on 04.04.2026.

#include <filesystem>

namespace fs = std::filesystem;
#include <vector>

fs::path get_unique_path(const fs::path& base_dir, const fs::path& relative_path);

void remove_empty_directories(const fs::path& dir);

std::vector<fs::path> get_sorted_relative_files(const fs::path& base_dir);