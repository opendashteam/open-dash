#pragma once
#include <fmt/format.h>
#include "../core/types.h"

namespace opendash::engine::log {

void startSavingToFile(const std::filesystem::path& path);

void quit();

void log(const std::string& msg, const std::string& uncolored);

template <typename ...Args>
inline void info(fmt::format_string<Args...> fmtStr, Args&&... args) {
    auto str = fmt::format(fmtStr, std::forward<Args>(args)...);
    log("\e[0;34m[INFO]\e[0m " + str, "[INFO] " + str);
}

template <typename ...Args>
inline void warn(fmt::format_string<Args...> fmtStr, Args&&... args) {
    auto str = fmt::format(fmtStr, std::forward<Args>(args)...);
    log("\e[0;33m[WARN]\e[0m " + str, "[WARN] " + str);
}

template <typename ...Args>
inline void err(fmt::format_string<Args...> fmtStr, Args&&... args) {
    auto str = fmt::format(fmtStr, std::forward<Args>(args)...);
    log("\e[0;31m[ERR] " + str + "\e[0m", "[ERR] " + str);
}

};