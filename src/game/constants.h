#pragma once

#include "../engine/nodes/CircleWave.h"
#include "../engine/core/types.h"

namespace opendash::constants
{
    namespace player {
        inline constexpr float kRotationDuration     = 26.0f / 60.0f;
        inline constexpr float kRotationDurationMini = 20.0f / 60.0f;

        inline constexpr float kSpeedNormal  = 0.9f;
        inline constexpr float kSpeedSlow    = 0.7f;
        inline constexpr float kSpeedFast    = 1.1f;
        inline constexpr float kSpeedFaster  = 1.3f;
        inline constexpr float kSpeedFastest = 1.6f;

        inline constexpr float kGravityNormal   = 0.958199f;
        inline constexpr double kGravitySlow    = 0.940199;
        inline constexpr double kGravityFast    = 0.957199;
        inline constexpr double kGravityFastest = 0.961199;

        inline constexpr float kJumpVelocityNormal    = 11.180032f;
        inline constexpr double kJumpVelocitySlow     = 10.620032;
        inline constexpr double kJumpVelocityFast     = 11.420032;
        inline constexpr double kJumpVelocityFastest  = 11.230032;

        inline constexpr float kTimeModNormal    = 5.770002f;
        inline constexpr double kTimeModSlow     = 5.980002;
        inline constexpr double kTimeModFast     = 5.870002;
        inline constexpr double kTimeModFastest  = 6.000002;

        inline constexpr float kStickDistanceClassic = 5.0f;
        inline constexpr float kStickDistancePlatformer = 10.0f;
        inline constexpr float kPlayerSqueezeToleranceClassic = 0.7f;
        inline constexpr float kPlayerSqueezeTolerancePlatformer = 0.8f;
        inline constexpr float kBlockInset = 0.3f;

        inline constexpr float kRespawnBlinkDuration = 0.4f;
        inline constexpr engine::u32 kRespawnBlinks  = 4;

        inline constexpr engine::Size kHitboxSizeDefault{30.0f, 30.0f};
        inline constexpr engine::Size kHitboxSizeSpider{27.0f, 27.0f};
        inline constexpr engine::Size kHitboxSizeWave{10.0f, 10.0f};
    }

    namespace presets {
        inline constexpr engine::CircleWaveOptions kCircleEffectJumpPadGeneric {
            .startRadius = 10.0f,
            .endRadius   = 40.0f,
            .duration    = 0.25f,
            .fadeIn      = false
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectRedJumpPadBig { // When not mini and red jump pad
            .startRadius = 12.0f,
            .endRadius   = 40.0f,
            .duration    = 0.25f,
            .fadeIn      = false
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPlayerSpawn {
            .startRadius = 70.0f,
            .endRadius   = 2.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .circleMode  = engine::CircleMode::Outline
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectJumpRingEnter {
            .startRadius = 5.0f,
            .endRadius   = 55.0f,
            .duration    = 0.25f,
            .fadeIn      = false,
            .circleMode  = engine::CircleMode::Outline
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectDualModeEnter {
            .startRadius = 50.0f,
            .endRadius   = 2.0f,
            .duration    = 0.25f,
            .fadeIn      = true
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectBecameMiniSize {
            .startRadius = 50.0f,
            .endRadius   = 2.0f,
            .duration    = 0.25f,
            .fadeIn      = true,
            .color       = {255, 0, 150}
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectBecameNormalSize {
            .startRadius = 10.0f,
            .endRadius   = 40.0f,
            .duration    = 0.3f,
            .fadeIn      = false,
            .color       = {0, 255, 150}
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectJumpRingHitGeneric {
            .startRadius = 35.0f,
            .endRadius   = 5.0f,
            .duration    = 0.35f,
            .fadeIn      = true,
            .easeOut     = true
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectRedJumpRingHit {
            .startRadius = 42.0f,
            .endRadius   = 5.0f,
            .duration    = 0.35f,
            .fadeIn      = true,
            .easeOut     = true
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalSize {
            .startRadius = 45.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalGravityFlipped {
            .startRadius = 45.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::GoldenYellow
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalGravityRestored {
            .startRadius = 45.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::VividBlue
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalSwing {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::GoldenYellow
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalSpider {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::VividRed
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalShip {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::Magenta
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalUFO {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::GoldenYellow
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalWave {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::GoldenYellow            
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalWaveExtra {
            .startRadius = 10.0f,
            .endRadius   = 60.0f,
            .duration    = 0.4f,
            .fadeIn      = false,
            .circleMode  = engine::CircleMode::Outline
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalBall {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::VividRed
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalMirrorOrange {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::BrightOrange
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalMirrorBlue {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::Cyan
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalTeleportOrange {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::GoldenYellow
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalTeleportBlue {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::Cyan
        };
        inline constexpr engine::CircleWaveOptions kCircleEffectPortalDualOff {
            .startRadius = 50.0f,
            .endRadius   = 5.0f,
            .duration    = 0.3f,
            .fadeIn      = true,
            .color       = engine::Color3B::NeonGreen
        };
    }
}