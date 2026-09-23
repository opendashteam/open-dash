#pragma once

#define CREATE_FUNC(type) \
static std::unique_ptr<type> create() { \
    auto ret = std::make_unique<type>(); \
    if (!ret->init()) { \
        return nullptr; \
    } \
    return ret; \
}