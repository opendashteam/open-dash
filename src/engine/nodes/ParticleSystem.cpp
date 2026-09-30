#include "ParticleSystem.h"

namespace opendash::engine {

bool ParticleSystem::applyPListProperties(PList* plist) {
    int maxParticles = 0;
    plist->fetchInteger("maxParticles", maxParticles);
    if (!setTotalParticles(maxParticles))
        return false;

    plist->fetchFloat("angle", angle.value);
    plist->fetchFloat("angleVariance", angle.variance);
    
    plist->fetchFloat("duration", duration);

    plist->fetchInteger("blendFuncSource", blendFuncSrc);
    plist->fetchInteger("blendFuncDestination", blendFuncDst);
    
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
    plist->fetchFloat("startParticleSizeVariance", startSize.value);
    plist->fetchFloat("finishParticleSize", endSize.value);
    plist->fetchFloat("finishParticleSizeVariance", endSize.value);
    
    plist->fetchFloat("sourcePositionx", sourcePos.value.x);
    plist->fetchFloat("sourcePositiony", sourcePos.value.y);
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

    // FIXME: textureFileName
    assert(!plist->getDict().contains("textureFileName"));
    return true;
}

bool ParticleSystem::setTotalParticles(u32 totalParticles) {
    return true;
}

};