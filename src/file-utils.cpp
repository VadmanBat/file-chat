#include "file-utils.h"

#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

fs::path get_unique_path(const fs::path& base_dir, const fs::path& relative_path) {
    fs::path target = base_dir / relative_path;

    if (!target.parent_path().empty())
        fs::create_directories(target.parent_path());

    if (!fs::exists(target))
        return target;

    fs::path stem = target.stem();
    fs::path ext  = target.extension();
    fs::path dir  = target.parent_path();

    int counter = 0;
    do {
        std::ostringstream oss;
        oss << " (" << ++counter << ")";
        fs::path new_filename = stem;
        new_filename          += oss.str();
        new_filename          += ext;

        target = dir / new_filename;
    } while (fs::exists(target));

    return target;
}

void remove_empty_directories(const fs::path& dir) {
    if (!fs::exists(dir) || !fs::is_directory(dir))
        return;

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_directory()) {
            remove_empty_directories(entry.path());

            if (fs::is_empty(entry.path()))
                fs::remove(entry.path());
        }
    }
}

std::vector<fs::path> get_sorted_relative_files(const fs::path& base_dir) {
    std::vector<fs::path> files;

    if (fs::exists(base_dir))
        for (const auto& entry : fs::recursive_directory_iterator(base_dir))
            if (entry.is_regular_file())
                files.push_back(fs::relative(entry.path(), base_dir));

    std::ranges::sort(files);
    return files;
}