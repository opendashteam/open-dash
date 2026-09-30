#include "Tween.h"
#include "../core/macros.h"
#include "../core/Director.h"

namespace opendash::engine
{

inline float bounceTime(float time) {
    if (time < 1 / 2.75) {
        return 7.5625f * time * time;
    }

    else if (time < 2 / 2.75) {
        time -= 1.5f / 2.75f;
        return 7.5625f * time * time + 0.75f;
    }

    else if(time < 2.5 / 2.75) {
        time -= 2.25f / 2.75f;
        return 7.5625f * time * time + 0.9375f;
    }

    time -= 2.625f / 2.75f;
    return 7.5625f * time * time + 0.984375f;
}

template <typename T>
inline T lerpValue(const T& from, const T& to, float t) {
    return from + (to - from) * t;
}

std::unique_ptr<Tween> Tween::create(const TweenOptions &opt) {
    auto ret = std::make_unique<Tween>();

    if (!ret->init(opt)) {
        return nullptr;
    }

    return ret;
}

void Tween::update(float dt) {
    if (!running_ || paused_ || finished_ || !easeFn_) {
		running_ = false;
		return;
	}

    if (startDelayRemaining_ > 0.0f) {
        startDelayRemaining_ -= dt;
        if (startDelayRemaining_ > 0.0f) return;

        dt = -startDelayRemaining_;
        startDelayRemaining_ = 0.0f;
    }

	running_ = true;

    elapsedInCycle_ += dt;

    float t = opt_.duration <= 0.0f
		? 1.0f
		: std::min(elapsedInCycle_ / opt_.duration, 1.0f);
	
    float eased = easeFn_(reversed_ ? 1.0f - t : t);

    if (opt_.onUpdate) opt_.onUpdate(lerpValue(opt_.from, opt_.to, eased));
	
	if (opt_.duration <= 0.0f) {
		finished_ = true;
		running_ = false;
		if (opt_.onComplete) {
            if (opt_.deleteSelf) {
                if (auto director = Director::get()) director->removeTween(this);
            }
            opt_.onComplete();
        }
	} else if (t >= 1.0f) {
		if (yoyoMode_ && !reversed_) {
			reversed_ = true;
			elapsedInCycle_ = 0.0f;
			cyclesCompleted_++;
			return;
		}

        bool continueLooping = false;

        if (!stopRepeatingRequested_) {
            if (repeatForever_)
				continueLooping = true;

            else if (repeatsLeft_ > 0) {
				repeatsLeft_--;
				continueLooping = true;
			}
        }

        if (continueLooping) { 
            elapsedInCycle_ = 0.0f;
			reversed_ = false;
            cyclesCompleted_++;
        } else {
            finished_ = true;
			running_ = false;
            if (opt_.onComplete) {
                if (opt_.deleteSelf) {
                    if (auto director = Director::get()) director->removeTween(this);
                }
                opt_.onComplete();
            }
        }
    }
}

bool Tween::isFinished() {
	return finished_;
}

bool Tween::init(const TweenOptions& opt) {
    opt_ = opt;
    period_ = opt.period;    

    switch (opt.easingType) {
        case EasingType::Linear:
            easeFn_ = [opt](float t) { return t; };
            break;
        case EasingType::EaseIn:
            easeFn_ = [opt](float t) { return powf(t, opt.rate); };
            break;
        case EasingType::EaseOut:
            easeFn_ = [opt](float t) { return powf(t, 1 / opt.rate); };
            break;
        case EasingType::EaseInOut:
            easeFn_ = [opt](float t) {
                t *= 2;
                if (t < 1)
                {
                    return 0.5f * powf(t, opt.rate);
                }
                else
                {
                    return 1.0f - 0.5f * powf(2-t, opt.rate);
                }
            };
            break;
        case EasingType::ExponentialIn:
            easeFn_ = [opt](float t) {
                return t == 0 ? 0 : powf(2, 10 * (t/1 - 1)) - 1 * 0.001f;
            };
            break;
        case EasingType::ExponentialOut:
            easeFn_ = [opt](float t) {
                return t == 1 ? 1 : (-powf(2, -10 * t / 1) + 1);
            };
            break;
        case EasingType::ExponentialInOut:
            easeFn_ = [opt](float t) {
                t /= 0.5f;
                if (t < 1)
                {
                    t = 0.5f * powf(2, 10 * (t - 1));
                }
                else
                {
                    t = 0.5f * (-powf(2, -10 * (t - 1)) + 2);
                }

                return t;
            };
            break;
        case EasingType::SineIn:
            easeFn_ = [opt](float t) {
                return -1 * cosf(t * (float)CC_PI_2) + 1;
            };
            break;
        case EasingType::SineOut:
            easeFn_ = [opt](float t) {
                return sinf(t * (float)CC_PI_2);
            };
            break;
        case EasingType::SineInOut:
            easeFn_ = [opt](float t) {
                return -0.5f * (cosf((float)CC_PI_2 * t) - 1);
            };
            break;
        case EasingType::ElasticIn:
            easeFn_ = [opt, this](float t) {
                float newT = 0;
                if (t == 0 || t == 1)
                {
                    newT = t;
                }
                else
                {
                    float s = period_ / 4;
                    t -= 1;
                    newT = -powf(2, 10 * t) * sinf((t - s) * CC_PI_X_2 / period_);
                }

                return newT;
            };
            break;
        case EasingType::ElasticOut:
            easeFn_ = [opt, this](float t) {
                float newT = 0;

                if (t == 0 || t == 1)
                    newT = t;
                else
                {
                    float s = period_ / 4;
                    newT = powf(2, -10 * t) * sinf((t - s) * CC_PI_X_2 / period_) + 1;
                }

                return newT;
            };
            break;
        case EasingType::ElasticInOut:
            easeFn_ = [opt, this](float t) {
                float newT = 0;

                if (t == 0 || t == 1)
                    newT = t;
                else
                {
                    t *= 2;
                    if (!period_)
                        period_ = 0.3f * 1.5f;
                    
                    float s = period_ / 4;

                    t -= 1;
                    if (t < 0)
                        newT = -0.5f * powf(2, 10 * t) * sinf((t -s) * CC_PI_X_2 / period_);
                    else
                        newT = powf(2, -10 * t) * sinf((t - s) * CC_PI_X_2 / period_) * 0.5f + 1;
                }

                return newT;
            };
            break;
        case EasingType::BounceIn:
            easeFn_ = [opt](float t) {
                return 1 - bounceTime(1 - t);
            };
            break;
        case EasingType::BounceOut:
            easeFn_ = [opt](float t) {
                return bounceTime(t);
            };
            break;
        case EasingType::BounceInOut:
            easeFn_ = [opt](float t) {
                if (t < 0.5f)
                    return (1 - bounceTime(1 - t * 2)) * 0.5f;
                else
                    return bounceTime(t * 2 - 1) * 0.5f + 0.5f;
            };
            break;
        case EasingType::BackIn:
            easeFn_ = [opt](float t) {
                float overshoot = 1.70158f;
                return t * t * ((overshoot + 1) * t - overshoot);
            };
            break;
        case EasingType::BackOut:
            easeFn_ = [opt](float t) {
                float overshoot = 1.70158f;
                t -= 1;
                return t * t * ((overshoot + 1) * t + overshoot) + 1;
            };
            break;
        case EasingType::BackInOut:
            easeFn_ = [opt](float t) {
                float overshoot = 1.70158f * 1.525f;

                t *= 2;
                if (t < 1)
                    return (t * t * ((overshoot + 1) * t - overshoot)) / 2;
                else
                {
                    t -= 2;
                    return (t * t * ((overshoot + 1) * t + overshoot)) / 2 + 1;
                }
            };
            break;
        default:
            return false;
    }

    return true;
}

Tween& Tween::setStartDelay(float delaySeconds) {
	if (!running_)
	{
		startDelay_ = delaySeconds;
		startDelayRemaining_ = delaySeconds;
	}
    return *this;
}

Tween& Tween::enableYoyo() {
    yoyoMode_ = true;
    return *this;
}

Tween& Tween::disableYoyo() {
    yoyoMode_ = false;
    return *this;
}

Tween& Tween::setRepeatCount(int repeatCount) {
	if (!running_)
	{
		totalRepeats_ = repeatCount;
		repeatsLeft_ = repeatCount;
	}
    return *this;
}

Tween& Tween::repeatForever() {
    repeatForever_ = true;
    return *this;
}

Tween& Tween::stopRepeating() {
    stopRepeatingRequested_ = true;
    return *this;
}

Tween& Tween::restart() {
    stop();
    start();
    return *this;
}

Tween& Tween::pause() {
    paused_ = true;
    return *this;
}

Tween& Tween::resume() {
    paused_ = false;
    return *this;
}

Tween& Tween::start() {
    if (running_) return *this;
    running_ = true;

    return *this;
}

Tween& Tween::stop() {
    running_ = false;

	startDelay_ = 0.0f;
	repeatsLeft_ = totalRepeats_;
	elapsedInCycle_ = 0.0f;
	// startDelayRemaining_ = startDelay_;
    reversed_ = false;
    stopRepeatingRequested_ = false;
    cyclesCompleted_ = 0;
	finished_ = false;

    return *this;
}

float Tween::getCycleProgress() const {
	if (opt_.duration <= 0.0f || finished_)
	{
		return 1.0f;
	}

	// A full round trip is two cycles when yo-yo mode is enabled.
	
	float cycleDuration = opt_.duration;
	float singleCycleProgress = elapsedInCycle_ / cycleDuration;

	return std::clamp(singleCycleProgress, 0.0f, 1.0f);
}

float Tween::getTotalProgress() const {
	if (opt_.duration <= 0.0f)
		return 1.0f;

	if (repeatForever_)
		return 0.0f;

	if (finished_)
		return 1.0f;

	int cyclesPerRepeat = yoyoMode_ ? 2 : 1;
	int totalCycles = cyclesCompleted_ + (repeatsLeft_ + 1) * cyclesPerRepeat;

	return (cyclesCompleted_ + getCycleProgress()) / static_cast<float>(totalCycles);
}

float Tween::getDuration() const {
    return opt_.duration;
}

bool Tween::isRunning() const {
    return running_;
}


}