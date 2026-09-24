#pragma once
#include <fmt/format.h>

namespace opendash::engine::log {

void log(const std::string& msg);

template <typename ...Args>
inline void info(fmt::format_string<Args...> fmtStr, Args&&... args) {
    log("\e[0;34m[INFO]\e[0m " + fmt::format(fmtStr, std::forward<Args>(args)...));
}

template <typename ...Args>
inline void warn(fmt::format_string<Args...> fmtStr, Args&&... args) {
    log("\e[0;33m[WARN]\e[0m " + fmt::format(fmtStr, std::forward<Args>(args)...));
}

template <typename ...Args>
inline void err(fmt::format_string<Args...> fmtStr, Args&&... args) {
    log("\e[0;31m[ERR] " + fmt::format(fmtStr, std::forward<Args>(args)...) + "\e[0m");
}

};