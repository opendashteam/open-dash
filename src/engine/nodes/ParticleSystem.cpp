#include "ParticleSystem.h"
#include "../AssetManager.h"
#include "../utilities/log.h"

namespace opendash::engine {

static constexpr TextureWrapParameters sharedParticleSystemWrapParameters {
    WrapMode::Clamp,
    WrapMode::Clamp
};

inline static float getVarColorValue(float value, float variance) {
    return std::clamp(value + variance * randomMinus1And1(), 0.0f, 1.0f);
}

Color4F VaryingColor::get(bool rgbVarSync) {
    Color4F ret;
    ret.r = getVarColorValue(value.r, variance.r);
    if (rgbVarSync) {
        ret.g = getVarColorValue(value.g, variance.r);
        ret.b = getVarColorValue(value.b, variance.r);
    } else {
        ret.g = getVarColorValue(value.g, variance.g);
        ret.b = getVarColorValue(value.b, variance.b);
    }
    ret.a = getVarColorValue(value.a, variance.a);
    return ret;
}

bool ParticleSystem::applyPListOptions(PList* plist) {
    int maxParticles = 0;
    plist->fetchInteger("maxParticles", maxParticles);
    if (!setTotalParticles(maxParticles))
        return false;

    plist->fetchFloat("angle", angle.value);
    plist->fetchFloat("angleVariance", angle.variance);
    
    plist->fetchFloat("duration", duration);

    int src = 0;
    plist->fetchInteger("blendFuncSource", src);
    additiveBlending = src == 1;
    
    plist->fetchFloat("startColorRed",   startColor.value.r);
    plist->fetchFloat("startColorGreen", startColor.value.g);
    plist->fetchFloat("startColorBlue",  startColor.value.b);
    plist->fetchFloat("startColorAlpha", startColor.value.a);
    
    plist->fetchFloat("startColorVarianceRed",   startColor.variance.r);
    plist->fetchFloat("startColorVarianceGreen", startColor.variance.g);
    plist->fetchFloat("startColorVarianceBlue",  startColor.variance.b);
    plist->fetchFloat("startColorVarianceAlpha", startColor.variance.a);
    
    plist->fetchFloat("finishColorRed",   endColor.value.r);
    plist->fetchFloat("finishColorGreen", endColor.value.g);
    plist->fetchFloat("finishColorBlue",  endColor.value.b);
    plist->fetchFloat("finishColorAlpha", endColor.value.a);
    
    plist->fetchFloat("finishColorVarianceRed",   endColor.variance.r);
    plist->fetchFloat("finishColorVarianceGreen", endColor.variance.g);
    plist->fetchFloat("finishColorVarianceBlue",  endColor.variance.b);
    plist->fetchFloat("finishColorVarianceAlpha", endColor.variance.a);

    plist->fetchFloat("startParticleSize", startSize.value);
    plist->fetchFloat("startParticleSizeVariance", startSize.variance);
    plist->fetchFloat("finishParticleSize", endSize.value);
    plist->fetchFloat("finishParticleSizeVariance", endSize.variance);
    
    float x, y;
    plist->fetchFloat("sourcePositionx", x);
    plist->fetchFloat("sourcePositiony", y);
    setPosition(x, y);
    plist->fetchFloat("sourcePositionVariancex", sourcePos.variance.x);
    plist->fetchFloat("sourcePositionVariancey", sourcePos.variance.y);
    
    plist->fetchFloat("rotationStart", startSpin.value);
    plist->fetchFloat("rotationStartVariance", startSpin.variance);
    
    plist->fetchFloat("rotationEnd", endSpin.value);
    plist->fetchFloat("rotationEndVariance", endSpin.variance);

    plist->fetchInteger("emitterType", (int&)particleMode);

    if (particleMode == ParticleMode::Gravity) {
        plist->fetchFloat("gravityx", gravityMode.gravity.x);
        plist->fetchFloat("gravityy", gravityMode.gravity.y);
        
        plist->fetchFloat("speed", gravityMode.speed.value);
        plist->fetchFloat("speedVariance", gravityMode.speed.variance);
        
        plist->fetchFloat("radialAcceleration", gravityMode.radialAccel.value);
        plist->fetchFloat("radialAccelVariance", gravityMode.radialAccel.variance);
        
        plist->fetchFloat("tangentialAcceleration", gravityMode.tangentialAccel.value);
        plist->fetchFloat("tangentialAccelVariance", gravityMode.tangentialAccel.variance);

        plist->fetchBoolean("rotationIsDir", gravityMode.rotationIsDir);
    } else {
        plist->fetchFloat("maxRadius", radiusMode.startRadius.value);
        plist->fetchFloat("maxRadiusVariance", radiusMode.startRadius.variance);
        plist->fetchFloat("minRadius", radiusMode.endRadius.value);
        // plist->fetchFloat("minRadiusVariance", radiusMode.endRadius.variance);
        plist->fetchFloat("rotatePerSecond", radiusMode.rotatePerSecond.value);
        plist->fetchFloat("rotatePerSecondVariance", radiusMode.rotatePerSecond.variance);
    }

    plist->fetchFloat("particleLifespan", lifetime.value);
    plist->fetchFloat("particleLifespanVariance", lifetime.variance);

    emissionRate = lifetime.value == 0.0f ? 0.0f : (float)maxParticles / lifetime.value;

    std::string texturePath;
    if (plist->fetchString("textureFileName", texturePath)) {
        if (!setTexture(texturePath))
            return false;
    }

    return true;
}

bool ParticleSystem::setTotalParticles(u32 totalParticles) {
    particles_.resize(totalParticles);
    return true;
}

void ParticleSystem::update(float dt) {
    // TODO: Here would be a "this->m_bWorldPosUninitialized = 1" in the decomp.
    //       Find out what that is.

    if (isActive_ && emissionRate != 0.0f) {
        float timeBetweenEmissions = 1.0f / emissionRate;

        if (!isFull()) {
            emissionCounter_ += dt;

            while (emissionCounter_ > timeBetweenEmissions) {
                addParticle();
                emissionCounter_ -= timeBetweenEmissions;
            }
        }

        elapsedTime_ += dt;
        if (duration != -1.0f && elapsedTime_ >= duration)
            stop();
    }

    currentPosition_ = {0, 0};
    if (positionType == PositionType::Free)
        currentPosition_ = pointToWorldTransform({0, 0});
    else if (positionType == PositionType::Relative)
        currentPosition_ = getPosition();

    if (isVisible()) {
        for (u32 i = 0; i < particleCount_;) {
            if (updateParticle(&particles_[i], dt))
                i++;
        }
    }

    spriteBatchDirty_ = true;
}

void ParticleSystem::draw(Graphics* gfx) {
    if (!texture_)
        return;

    if (spriteBatchDirty_) {
        spriteBatch_->resize(particleCount_);

        for (u32 i = 0; i < particleCount_; i++) {
            Particle* particle = &particles_[i];

            Point absPos = getParticleAbsolutePosition(particle);

            glm::vec2 size(particle->size);

            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(absPos.x, absPos.y, 0.0f));
            transform = glm::rotate(transform, glm::radians(-particle->rot), glm::vec3(0.0f, 0.0f, 1.0f));
            transform = glm::translate(transform, glm::vec3(-size * 0.5f, 0.0f));
            transform = glm::scale(transform, glm::vec3(size, 0.0f));

            spriteBatch_->setSprite(i, transform, glm::mat3(1.0f), particle->color);
        }
        spriteBatchDirty_ = false;
    }

    spriteBatch_->draw(texture_, getWorldTransform(), {1, 1, 1, 1}, additiveBlending);
}

void ParticleSystem::stop() {
    isActive_ = false;
    elapsedTime_ = duration;
    emissionCounter_ = 0;
}

void ParticleSystem::reset() {
    isActive_ = true;
    elapsedTime_ = 0;
    for (int i = 0; i < particleCount_; i++)
        particles_[i].remainingLife = 0;
}

void ParticleSystem::setTexture(Texture* texture) {
    texture_ = texture;
}

bool ParticleSystem::setTexture(const std::filesystem::path& texturePath) {
    texture_ = AssetManager::get()->fetchTexture(texturePath);
    if (!texture_) {
        log::err("Failed to set texture of particle system, could not load texture at path {}", texturePath.string());
        return false;
    }

    return true;
}

std::unique_ptr<ParticleSystem> ParticleSystem::create(const std::filesystem::path& path) {
    auto plist = AssetManager::get()->fetchParticleSystemOptions(path);
    if (!plist)
        return nullptr;

    auto ret = std::make_unique<ParticleSystem>();
    if (ret && ret->init() && ret->applyPListOptions(plist))
        return ret;
    return nullptr;
}

bool ParticleSystem::init() {
    scheduleUpdate();
    spriteBatch_ = SpriteBatch::create();
    return true;
}

// Mostly reverse engineered from RobTop's modified CCParticleSystem::initParticle
void ParticleSystem::initParticle(Particle* particle) {
    particle->remainingLife = std::max(lifetime.get(), 0.0001f);
    particle->lifetimeInverse = 1.0f / particle->remainingLife;

    particle->pos = sourcePos.get();

    particle->color = startColor.get(startRGBVarSync);

    Color4F col = endColor.get(endRGBVarSync);
    particle->deltaColor = {
        (col.r - particle->color.r) * particle->lifetimeInverse,
        (col.g - particle->color.g) * particle->lifetimeInverse,
        (col.b - particle->color.b) * particle->lifetimeInverse,
        (col.a - particle->color.a) * particle->lifetimeInverse
    };

    particle->fadeInTime  = fadeInTime.getClamped(0.0f, particle->remainingLife);
    particle->fadeOutTime = fadeOutTime.getClamped(0.0f, particle->remainingLife - particle->fadeInTime);

    particle->size = std::max(startSize.get(), 0.0f);

    particle->frictionPos  = frictionPos.get();
    particle->frictionSize = frictionSize.get();
    particle->frictionRot  = frictionRot.get();

    particle->hasFriction = particle->frictionPos  != 0.0f ||
                            particle->frictionSize != 0.0f ||
                            particle->frictionRot  != 0.0f;

    float size = endSize.get();
    float deltaSize;

    if (startSizeEqualToEnd) {
        if ((particle->size + size) < 0.0f)
            deltaSize = -particle->size;
        else
            deltaSize = size;
    } else
        deltaSize = glm::max(size, 0.0f) - particle->size;

    particle->deltaSize = deltaSize * particle->lifetimeInverse;

    // Yes, RobTop casts it into an int
    int spin = startSpin.get();
    if (spin > 360)
        spin %= 360;

    particle->rot = ((float)spin < 0.0f) ? (float)spin + 360.0f : (float)spin;

    if (dynamicRotation) {
        particle->deltaRot = 0.0f;
        
        particle->dynamicRot = getPerpendicularAngleCCW(particle->rot);
    } else {
        particle->dynamicRot = 0.0f;

        float rot = endSpin.get();
        if (!startSpinEqualToEnd)
            rot = rot - particle->rot;
        particle->deltaRot = rot * particle->lifetimeInverse;
    }

    if (positionType == PositionType::Free)
        particle->startPos = pointToWorldTransform({0, 0});
    else if (positionType == PositionType::Relative)
        particle->startPos = getPosition();

    float angleValue = rad(angle.get());

    if (particleMode == ParticleMode::Gravity) {
        Point v(cosf(angleValue), sinf(angleValue));

        particle->gravityMode.velocity = Point(cosf(angleValue), sinf(angleValue)) * gravityMode.speed.get();

        particle->gravityMode.radialAccel     = gravityMode.radialAccel.get();
        particle->gravityMode.tangentialAccel = gravityMode.tangentialAccel.get();

        if (gravityMode.rotationIsDir || dynamicRotation)
            particle->rot = -deg(particle->gravityMode.velocity.getAngle());
    } else {
        particle->radiusMode.radius = radiusMode.startRadius.get();
        float deltaRadius = radiusMode.endRadius.get();

        if (!startRadiusEqualToEnd)
            deltaRadius = deltaRadius - particle->radiusMode.radius;

        particle->radiusMode.deltaRadius = deltaRadius * particle->lifetimeInverse;

        particle->radiusMode.angle = angleValue;
        particle->radiusMode.rotatePerSecond = rad(radiusMode.rotatePerSecond.get());

        if (particle->radiusMode.rotatePerSecond == 0.0f)
            particle->radiusMode.angleVectorIfNotRotating = {cosf(angleValue), sinf(angleValue)};

        if (gravityMode.rotationIsDir || dynamicRotation) {
            particle->rot += 90.0f - deg(angleValue);

            if (dynamicRotation && particle->radiusMode.rotatePerSecond == 0.0f)
                particle->dynamicRot = 0.0f;
        }
    }

    // FIXME!!!!!! ADD FRICTION I FORGOR TO ADD IT!!!!!
}

bool ParticleSystem::addParticle() {
    if (isFull())
        return false;

    Particle* particle = &particles_[particleCount_];
    initParticle(particle);
    particleCount_++;
    return true;
}

// Mostly reverse engineered from RobTop's modified CCParticleSystem::update
bool ParticleSystem::updateParticle(Particle* p, float dt) {
    p->remainingLife -= dt;

    if (p->remainingLife <= 0.0f) {
        removeParticle(p);
        return false;
    }

    p->rot += p->deltaRot * dt;

    if (particleMode == ParticleMode::Gravity) {
        auto mode = &p->gravityMode;

        Point acceleration = gravityMode.gravity;

        Point radial = {0, 0};
        if (p->pos != Point(0.0f, 0.0f))
            radial = p->pos.normalize();

        Point tangential = { -radial.y, radial.x };

        acceleration += radial     * mode->radialAccel;
        acceleration += tangential * mode->tangentialAccel;

        mode->velocity += acceleration * dt;
        p->pos += mode->velocity * dt;

        if (p->dynamicRot != 0.0f) {
            float rot = (p->dynamicRot - deg(mode->velocity.getAngle())) - p->rot;

            if (rot > 180.0f)
                rot -= 360.0f;
            if (rot < -180.0f)
                rot += 360.0f;

            p->rot += rot * dt * 10.0f;
        }
    } else { // ParticleMode::Radial
        auto mode = &p->radiusMode;

        mode->radius += mode->deltaRadius * dt;

        if (p->dynamicRot == 0.0f) {
            if (mode->rotatePerSecond == 0.0f)
                p->pos = -mode->angleVectorIfNotRotating * mode->radius;
            else {
                float angle = clampAngle(deg(mode->angle + mode->rotatePerSecond * dt));
                mode->angle = rad(angle);

                p->pos.x = -cosf(mode->angle) * mode->radius;
                p->pos.y = -sinf(mode->angle) * mode->radius;
            }
        } else {
            mode->angle += mode->angle + mode->rotatePerSecond * dt;
            Point newPos = {
                -cosf(mode->angle) * mode->radius,
                -sinf(mode->angle) * mode->radius
            };

            float rot = p->dynamicRot - deg((p->pos - newPos).getAngle()) - p->rot;

            p->pos = newPos;

            if (rot > 180.0f)
                rot -= 360.0f;
            if (rot < -180.0f)
                rot += 360.0f;

            if (p->timeProgress < 0.1f)
                p->rot += rot;
            else
                p->rot += rot * dt * 10.0f;
        }
    }

    p->timeProgress += dt;

    // I have no clue why RobTop added this as you can just use what cocos2d already had
    if (useUniformColor) {
        float fraction = p->timeProgress * p->lifetimeInverse;
        p->color.r = std::lerp(uniformStartColor.r, uniformEndColor.r, fraction);
        p->color.g = std::lerp(uniformStartColor.g, uniformEndColor.g, fraction);
        p->color.b = std::lerp(uniformStartColor.b, uniformEndColor.b, fraction);
    } else {
        p->color.r += p->deltaColor.r * dt;
        p->color.g += p->deltaColor.g * dt;
        p->color.b += p->deltaColor.b * dt;
    }

    p->color.a += p->deltaColor.a * dt;

    p->size = std::max(p->size + p->deltaSize * dt, 0.0f);
    p->rot  = clampAngle(p->rot);

    return true;
}

void ParticleSystem::removeParticle(Particle* particle) {
    u32 index = particle - particles_.data();

    if (index != particleCount_ - 1)
        particles_[index] = particles_[particleCount_ - 1];
    particleCount_--;

    // FIXME: Add isAutoRemoveOnFinish
}

Point ParticleSystem::getParticleAbsolutePosition(Particle* particle) {
    if (positionType == PositionType::Free || positionType == PositionType::Relative)
        return particle->pos - (currentPosition_ - particle->startPos);
    else
        return particle->pos;
}

};