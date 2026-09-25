#pragma once

#include <filesystem>
#include <cstdint>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace opendash::engine
{

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

struct Vertex {
    float x, y;
    float u, v;
};    

struct PathHash {
    std::size_t operator()(const std::filesystem::path& p) const {
        return std::filesystem::hash_value(p);
    }
};

struct Color4F;

struct Color3B {
    u8 r, g, b;

    static Color3B fromColor4F(const Color4F& color);

    bool operator==(const Color3B& other) const = default;

    static const Color3B WHITE;
    static const Color3B BLACK;
    static const Color3B RED;
    static const Color3B GREEN;
    static const Color3B BLUE;
    static const Color3B CYAN;
    static const Color3B MAGENTA;
    static const Color3B YELLOW;
};

inline constexpr Color3B Color3B::WHITE = {255, 255, 255};
inline constexpr Color3B Color3B::BLACK = {0, 0, 0};
inline constexpr Color3B Color3B::RED   = {255, 0, 0};
inline constexpr Color3B Color3B::GREEN = {0, 255, 0};
inline constexpr Color3B Color3B::BLUE  = {0, 0, 255};
inline constexpr Color3B Color3B::CYAN   = {0, 255, 255};
inline constexpr Color3B Color3B::MAGENTA = {255, 0, 255};
inline constexpr Color3B Color3B::YELLOW  = {255, 255, 0};

struct Color4B {
    u8 r, g, b, a;

    bool operator==(const Color4B& other) const = default;

    static const Color4B WHITE;
    static const Color4B BLACK;
    static const Color4B RED;
    static const Color4B GREEN;
    static const Color4B BLUE;
    static const Color4B CYAN;
    static const Color4B MAGENTA;
    static const Color4B YELLOW;
    static const Color4B TRANSPARENT;
};

inline constexpr Color4B Color4B::WHITE = {255, 255, 255, 255};
inline constexpr Color4B Color4B::BLACK = {0, 0, 0, 255};
inline constexpr Color4B Color4B::RED   = {255, 0, 0, 255};
inline constexpr Color4B Color4B::GREEN = {0, 255, 0, 255};
inline constexpr Color4B Color4B::BLUE  = {0, 0, 255, 255};
inline constexpr Color4B Color4B::CYAN   = {0, 255, 255, 255};
inline constexpr Color4B Color4B::MAGENTA = {255, 0, 255, 255};
inline constexpr Color4B Color4B::YELLOW  = {255, 255, 0, 255};
inline constexpr Color4B Color4B::TRANSPARENT = {0, 0, 0, 0};
struct Color4F {
    float r, g, b, a;

    bool operator==(const Color4F& other) const = default;

    static Color4F fromColor4B(const Color4B& color) {
        return {
            color.r / 255.0f,
            color.g / 255.0f,
            color.b / 255.0f,
            color.a / 255.0f
        };
    }

    static Color4F fromColor3B(const Color3B& color, float alpha = 1.0f) {
        return {
            color.r / 255.0f,
            color.g / 255.0f,
            color.b / 255.0f,
            alpha
        };
    }

    static const Color4F WHITE;
    static const Color4F BLACK;
    static const Color4F RED;
    static const Color4F GREEN;
    static const Color4F BLUE;
    static const Color4F CYAN;
    static const Color4F MAGENTA;
    static const Color4F YELLOW;
    static const Color4F TRANSPARENT;
};

inline constexpr Color4F Color4F::WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
inline constexpr Color4F Color4F::BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
inline constexpr Color4F Color4F::RED   = {1.0f, 0.0f, 0.0f, 1.0f};
inline constexpr Color4F Color4F::GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
inline constexpr Color4F Color4F::BLUE  = {0.0f, 0.0f, 1.0f, 1.0f};
inline constexpr Color4F Color4F::CYAN   = {0.0f, 1.0f, 1.0f, 1.0f};
inline constexpr Color4F Color4F::MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
inline constexpr Color4F Color4F::YELLOW  = {1.0f, 1.0f, 0.0f, 1.0f};
inline constexpr Color4F Color4F::TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};

inline Color3B Color3B::fromColor4F(const Color4F& color) {
    return {
        static_cast<u8>(color.r * 255.0f),
        static_cast<u8>(color.g * 255.0f),
        static_cast<u8>(color.b * 255.0f)
    };
}

struct Size {
    float width;
    float height;

    constexpr Size() = default;

    constexpr Size(float width, float height) : width(width), height(height) {}

    constexpr Size(const Size& other) : width(other.width), height(other.height) {}

    bool operator==(const Size& other) const = default;

    Size operator+(const Size& right) const {
        return Size(width + right.width, height + right.height);
    }

    Size operator-(const Size& right) const {
        return Size(width - right.width, height - right.height);
    }

    Size operator*(const Size& right) const {
        return Size(width * right.width, height * right.height);
    }

    Size operator/(const Size& right) const {
        return Size(width / right.width, height / right.height);
    }

    Size operator*(float a) const {
        return Size(width * a, height * a);
    }

    Size operator/(float a) const {
        return Size(width / a, height / a);
    }

    Size& operator=(const Size& other) {
        width = other.width;
        height = other.height;
        return *this;
    }

    float getAspect() {
        return width / height;
    }

    void swap() {
        std::swap(width, height);
    }

    float getPerimeter() {
        return (width + height) * 2.0f;
    }

    glm::vec2 toGLM() const {
        return { width, height };
    }

    Size inUnits() const;

    Size inPixels() const;
};

struct Point {
    float x;
    float y;

    constexpr Point() = default;

    constexpr Point(float x, float y) : x(x), y(y) {}

    constexpr Point(const Point& other) : x(other.x), y(other.y) {}

    bool operator==(const Point& other) const = default;

    Point operator+(const Point& right) const {
        return Point(x + right.x, y + right.y);
    }

    Point operator-(const Point& right) const {
        return Point(x - right.x, y - right.y);
    }

    Point operator*(const Point& right) const {
        return Point(x * right.x, y * right.y);
    }

    Point operator/(const Point& right) const {
        return Point(x / right.x, y / right.y);
    }

    Point operator*(float a) const {
        return Point(x * a, y * a);
    }

    Point operator/(float a) const {
        return Point(x / a, y / a);
    }

    Point& operator=(const Point& right) {
        x = right.x;
        y = right.y;
        return *this;
    }

    float getLength() {
        return std::sqrt(x * x + y * y);
    }

    float getLengthSq() {
        return x * x + y * y;
    }

    float getDistanceSq(const Point& other) {
        float dx = other.x - x;
        float dy = other.y - y;
        return (dx * dx + dy * dy);
    }

    float getDistance(const Point& other) {
        return std::sqrt(getDistanceSq(other));
    }

    void swap() {
        std::swap(x, y);
    }

    glm::vec2 toGLM() const {
        return { x, y };
    }

    Point inUnits() const;

    Point inPixels() const;
};

struct Rect {
    Point origin;
    Size size;

    constexpr Rect() = default;

    constexpr Rect(float x, float y, float width, float height) : origin{x, y}, size{width, height} {}

    constexpr Rect(const Point& origin, const Size& size) : origin(origin), size(size) {}

    bool operator==(const Rect& other) const = default;

    Rect& operator=(const Rect& right) {
        origin = right.origin;
        size   = right.size;
        return *this;
    }

    float getMinX() const {
        return origin.x;
    }

    float getMinY() const {
        return origin.y;
    }

    float getMaxX() const {
        return origin.x + size.width;
    }

    float getMaxY() const {
        return origin.y + size.height;
    }

    bool containsPoint(const Point& point) const {
        return point.x > getMinX() &&
               point.x < getMaxX() &&
               point.y > getMinY() &&
               point.y < getMinY();
    }

    bool intersectsRect(const Rect& rect) const {
        return !(rect.getMinX() >= getMaxX() ||
                 rect.getMaxX() <= getMinX() ||
                 rect.getMinY() >= getMaxY() ||
                 rect.getMaxY() <= getMinY());
    }
};

inline constexpr Point PointZero(0.0f, 0.0f);
inline constexpr Size SizeZero(0.0f, 0.0f);
inline constexpr Rect RectZero(PointZero, SizeZero);

// Unused
enum class ResolutionPolicy {
    ExactFit,    // Stretch to fit the whole image on the display
    NoBorder,    // Scale to fill the whole display, may cause cropping
    ShowAll,     // Scale to fill while ensuring the entire design is visible, causes letterboxing
    FixedHeight, // Keep the design height fixed, adjust the width to the display
    FixedWidth   // Keep the design width fixed, adjust the height to the display
};

enum class WrapMode {
    Repeat, Clamp, MirroredRepeat, Count
};

struct TextureWrapParameters {
    WrapMode u;
    WrapMode v;

    inline u32 asBitCode() const {
        return (u32)u * (u32)WrapMode::Count + (u32)v;
    }
};

}

// This allows you to use Point, etc.. in the logger and it will be automatically converted
// Thank you IliasHDZ - StarryDawn72
template <>
class fmt::formatter<opendash::engine::Size> {
public:
    constexpr auto parse (format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format (const opendash::engine::Size& value, Context& ctx) const {
        return format_to(ctx.out(), "({}, {})", value.width, value.height);
    }
};

template <>
class fmt::formatter<opendash::engine::Point> {
public:
    constexpr auto parse (format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format (const opendash::engine::Point& value, Context& ctx) const {
        return format_to(ctx.out(), "({}, {})", value.x, value.y);
    }
};

template <>
class fmt::formatter<opendash::engine::Rect> {
public:
    constexpr auto parse (format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format (const opendash::engine::Rect& value, Context& ctx) const {
        return format_to(ctx.out(), "({}, {}, {}, {})", value.origin.x, value.origin.y, value.size.width, value.size.height);
    }
};