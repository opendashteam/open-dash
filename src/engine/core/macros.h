#pragma once

#define CREATE_FUNC(class) \
static std::unique_ptr<class> create() { \
    auto ret = std::make_unique<class>(); \
    if (!ret->init()) { \
        return nullptr; \
    } \
    return ret; \
}