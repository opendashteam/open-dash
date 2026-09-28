#pragma once

#define CREATE_FUNC(type) \
static std::unique_ptr<type> create() { \
    auto ret = std::make_unique<type>(); \
    if (!ret->init()) { \
        return nullptr; \
    } \
    return ret; \
}

/*
    For accuracy some constants need to be 1:1 replicas of the source.
*/
#define CC_PI 3.14159265358979323846
#define CC_PI_2 CC_PI / 2.0
#define CC_PI_X_2 (float)CC_PI * 2.0f