#include "log.h"
#include <iostream>
#include <fstream>
#include "../../platform/Window.h"

namespace opendash::engine {

std::ofstream logStream;

void log::startSavingToFile(const std::filesystem::path& path) {
    log::info("{}", (platform::Window::getGameSavePath() / path).string());
    logStream = std::ofstream(platform::Window::getGameSavePath() / path);
    if (!logStream.is_open())
        log::warn("Failed to create a log file");
}

void log::quit() {
    if (logStream.is_open())
        logStream.close();
}

void log::log(const std::string& msg, const std::string& uncolored) {
    std::cout << msg << '\n';
    if (logStream.is_open()) {
        logStream << uncolored << '\n';
        logStream.flush();
    }
}

};