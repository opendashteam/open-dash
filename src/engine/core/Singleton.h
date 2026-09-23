#pragma once

#include <memory>

template<typename T>
class Singleton {
public:
    static T* get() {
        return instance_;
    }

    static std::unique_ptr<T> create() {
        auto ret = std::make_unique<T>();
        if (!ret->init()) {
            return nullptr;
        }
        instance_ = ret.get();
        return ret;
    }

protected:
    virtual bool init() {
        return true;
    }

private:
    inline static T* instance_ = nullptr;
};