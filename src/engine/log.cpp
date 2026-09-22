#include "log.h"
#include <iostream>

namespace opendash::engine {

void log::log(const std::string& msg) {
    std::cout << msg << '\n';
}

};