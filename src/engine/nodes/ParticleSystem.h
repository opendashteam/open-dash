#pragma once

#include "Node.h"

namespace opendash::engine {

struct Particle {
    Point   pos, startPos;

    Color4F color, deltaColor;
    float   size,  deltaSize;
    float   rot,   deltaRot;

    float timeToLive;
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
        float angle;
        float angleCos, angleSin;
        float radius, deltaRadius;
    } radiusMode;
};

struct VaryingNumber {
    float value = 0.0f, variance = 0.0f;
};

struct VaryingPoint {
    Point value, variance;
};

struct VaryingColor {
    Color4F value, variance;
};

enum class ParticleMode {
    Gravity,
    Radius
};

class ParticleSystem : Node {
public:
    bool applyPListProperties(PList* plist);

    bool setTotalParticles(u32 totalParticles);

public:
    // These are all the properties of the particle system
    VaryingNumber lifetime;
    VaryingNumber angle;
    VaryingPoint  sourcePos;

    u32 totalParticles = 0;
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

    ParticleMode particleMode;

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

    int blendFuncSrc, blendFuncDst;
};

};