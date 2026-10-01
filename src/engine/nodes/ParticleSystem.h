#pragma once

#include "Node.h"
#include "../utilities/MathUtils.h"
#include "../core/SpriteBatch.h"

namespace opendash::engine {

struct Particle {
    Point   pos, startPos;

    Color4F color, deltaColor;
    float   size,  deltaSize;
    float   rot,   deltaRot;

    float remainingLife;
    float lifetimeInverse;
    float timeProgress;

    u32 batchIndex;

    float fadeInTime, fadeOutTime;
    float dynamicRot;

    bool hasFriction;
    float frictionPos;
    float frictionSize;
    float frictionRot;

    struct {
        Point velocity; // dir in cocos2d
        float radialAccel;
        float tangentialAccel;
    } gravityMode;

    struct {
        float angle, rotatePerSecond;
        float radius, deltaRadius;
        Point angleVectorIfNotRotating;
    } radiusMode;
};

struct VaryingNumber {
    float value = 0.0f, variance = 0.0f;

    inline float get() const {
        return value + variance * randomMinus1And1();
    }

    inline float getClamped(float min, float max) const {
        return std::clamp(get(), min, max);
    }
};

struct VaryingPoint {
    Point value, variance;

    inline Point get() {
        return {
            value.x + variance.x * randomMinus1And1(),
            value.y + variance.y * randomMinus1And1()
        };
    }
};

struct VaryingColor {
    Color4F value, variance;

    Color4F get(bool rgbVarSync);
};

enum class ParticleMode {
    Gravity,
    Radius
};

enum class PositionType {
    Free,
    Relative,
    Grouped
};

class ParticleSystem : public Node {
public:
    bool applyPListOptions(PList* plist);

    bool setTotalParticles(u32 totalParticles);

    inline u32 getTotalParticles() const {
        return particles_.size();
    }

    virtual void update(float dt) override;

    virtual void draw(Graphics* gfx) override;

    // Equivalent to `CCParticleSystem::stopSystem`
    void stop();

    // Equivalent to `CCParticleSystem::resetSystem`
    void reset();

    // Equivalent to `CCParticleSystem::resumeSystem`
    inline void resume() {
        isActive_ = true;
    }

    void setTexture(Texture* texture);

    bool setTexture(const std::filesystem::path& texturePath);

public:
    CREATE_FUNC(ParticleSystem);
    static std::unique_ptr<ParticleSystem> create(const std::filesystem::path& plistPath);

protected:
    bool init() override;

private:
    void initParticle(Particle* particle);

    bool addParticle();

    bool updateParticle(Particle* particle, float dt);

    void removeParticle(Particle* particle);

    Point getParticleAbsolutePosition(Particle* particle);

    inline bool isFull() const { return particleCount_ >= getTotalParticles(); }

public:
    // These are all the properties of the particle system
    VaryingNumber lifetime;
    VaryingNumber angle;
    VaryingPoint  sourcePos;

    PositionType positionType = PositionType::Free;

    float duration = 0.0f;
    float emissionRate = 0.0f;

    VaryingColor  startColor, endColor;
    VaryingNumber startSize, endSize;
    VaryingNumber startSpin, endSpin;

    VaryingNumber fadeInTime, fadeOutTime;
    
    VaryingNumber frictionPos;
    VaryingNumber frictionSize;
    VaryingNumber frictionRot;
    
    VaryingNumber respawn;

    bool startSpinEqualToEnd = false;
    bool startSizeEqualToEnd = false;
    bool startRadiusEqualToEnd = false;
    bool dynamicRotation = false;
    bool orderSensitive = false;
    bool startRGBVarSync = false, endRGBVarSync = false;

    ParticleMode particleMode = ParticleMode::Gravity;

    bool useUniformColor = false;
    Color4F uniformStartColor, uniformEndColor;

    struct {
        Point gravity = {0, 0};
        VaryingNumber speed;
        VaryingNumber tangentialAccel;
        VaryingNumber radialAccel;
        bool rotationIsDir = false;
    } gravityMode;

    struct {
        VaryingNumber startRadius, endRadius;
        VaryingNumber rotatePerSecond;
    } radiusMode;

    bool additiveBlending = false;

private:
    bool isActive_ = true;

    float emissionCounter_ = 0.0f;
    float elapsedTime_ = 0.0f;

    Texture* texture_ = nullptr;
    bool spriteBatchDirty_ = true;
    std::unique_ptr<SpriteBatch> spriteBatch_;

    u32 particleCount_ = 0;
    std::vector<Particle> particles_;

    Point currentPosition_;
};

};