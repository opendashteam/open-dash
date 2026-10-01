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

template <typename... args>
using Callback = std::function<void(args...)>;

struct Vertex {
    float x, y;
    float u, v;
};   

struct OutlineVertex {
    glm::vec2 segStart;
    glm::vec2 segEnd;
    float side; // -1 or +1
    float endpoint; // 0 or 1
};

struct OutlineMesh {
    std::vector<OutlineVertex> vertices; // Points
    std::vector<u32> indices; // Which points connect to which to form triangles
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

    static const Color3B White;
    static const Color3B Black;
    static const Color3B Red;
    static const Color3B Green;
    static const Color3B Blue;
    static const Color3B Cyan;
    static const Color3B Magenta;
    static const Color3B Yellow;
    static const Color3B GoldenYellow;
    static const Color3B VividRed;
    static const Color3B VividBlue;
    static const Color3B BrightOrange;
    static const Color3B NeonGreen;
};

inline constexpr Color3B Color3B::White = {255, 255, 255};
inline constexpr Color3B Color3B::Black = {0, 0, 0};
inline constexpr Color3B Color3B::Red   = {255, 0, 0};
inline constexpr Color3B Color3B::Green = {0, 255, 0};
inline constexpr Color3B Color3B::Blue  = {0, 0, 255};
inline constexpr Color3B Color3B::Cyan   = {0, 255, 255};
inline constexpr Color3B Color3B::Magenta = {255, 0, 255};
inline constexpr Color3B Color3B::Yellow  = {255, 255, 0};
inline constexpr Color3B Color3B::GoldenYellow  = {255, 200, 0};
inline constexpr Color3B Color3B::VividRed  = {255, 50, 50};
inline constexpr Color3B Color3B::VividBlue  = {0, 150, 255};
inline constexpr Color3B Color3B::BrightOrange = {255, 150, 0};
inline constexpr Color3B Color3B::NeonGreen = {0, 255, 100};

struct Color4B {
    u8 r, g, b, a;

    bool operator==(const Color4B& other) const = default;
};
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

    static glm::vec4 toVector(const Color4F& color) {
        return {color.r, color.g, color.b, color.a};
    };
};

inline Color3B Color3B::fromColor4F(const Color4F& color) {
    return {
        static_cast<u8>(color.r * 255.0f),
        static_cast<u8>(color.g * 255.0f),
        static_cast<u8>(color.b * 255.0f)
    };
}

class Point;

struct Size {
    float width;
    float height;

    constexpr Size() = default;

    constexpr Size(float width, float height) : width(width), height(height) {}

    constexpr Size(const Size& other) : width(other.width), height(other.height) {}

    constexpr Size(const Point& other);

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

    Size toPoints() const;

    Size toPixels() const;
};

struct Point {
    float x;
    float y;

    constexpr Point() = default;

    constexpr Point(float x, float y) : x(x), y(y) {}

    constexpr Point(const Point& other) : x(other.x), y(other.y) {}

    constexpr Point(const Size& other) : x(other.width), y(other.height) {}

    bool operator==(const Point& other) const = default;

    Point operator+(const Point& right) const { return {x + right.x, y + right.y}; }
    Point operator-(const Point& right) const { return {x - right.x, y - right.y}; }
    Point operator*(const Point& right) const { return {x * right.x, y * right.y}; }
    Point operator/(const Point& right) const { return {x / right.x, y / right.y}; }

    Point operator+(float a) const { return {x + a, y + a}; }
    Point operator-(float a) const { return {x - a, y - a}; }
    Point operator*(float a) const { return {x * a, y * a}; }
    Point operator/(float a) const { return {x / a, y / a}; }

    Point& operator+=(const Point& right) { *this = *this + right; return *this; }
    Point& operator-=(const Point& right) { *this = *this - right; return *this; }
    Point& operator*=(const Point& right) { *this = *this * right; return *this; }
    Point& operator/=(const Point& right) { *this = *this / right; return *this; }

    Point& operator+=(float a) { *this = *this + a; return *this; }
    Point& operator-=(float a) { *this = *this - a; return *this; }
    Point& operator*=(float a) { *this = *this * a; return *this; }
    Point& operator/=(float a) { *this = *this / a; return *this; }

    Point operator-() const { return {-x, -y}; }

    Point& operator=(const Point& right) {
        x = right.x;
        y = right.y;
        return *this;
    }

    float getLength() const {
        return std::sqrt(x * x + y * y);
    }

    float getLengthSq() const {
        return x * x + y * y;
    }

    float getDistanceSq(const Point& other) const {
        float dx = other.x - x;
        float dy = other.y - y;
        return (dx * dx + dy * dy);
    }

    float getDistance(const Point& other) const {
        return std::sqrt(getDistanceSq(other));
    }

    Point normalize() const {
        float lenInv = 1.0f / getLength();
        return { x * lenInv, y * lenInv };
    }

    // Returns in radians
    float getAngle() const {
        return atan2f(y, x);
    }

    void swap() {
        std::swap(x, y);
    }

    glm::vec2 toGLM() const {
        return { x, y };
    }

    Point toPoints() const;

    Point toPixels() const;
};

constexpr Size::Size(const Point& other)
    : width(other.x), height(other.y) {}

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