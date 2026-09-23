#pragma once
#include <fmt/format.h>

namespace opendash::engine::log {

void log(const std::string& msg);

template <typename ...Args>
inline void info(fmt::format_string<Args...> fmtStr, Args&&... args) {
    log("[INFO] " + fmt::format(fmtStr, std::forward<Args>(args)...));
}

template <typename ...Args>
inline void warn(fmt::format_string<Args...> fmtStr, Args&&... args) {
    log("[WARN] " + fmt::format(fmtStr, std::forward<Args>(args)...));
}

template <typename ...Args>
inline void err(fmt::format_string<Args...> fmtStr, Args&&... args) {
    log("[ERR] " + fmt::format(fmtStr, std::forward<Args>(args)...));
}

};