#pragma once

#include "../core/types.h"

namespace opendash::engine
{

enum class EasingType {
    Linear,
    
    EaseIn,
    EaseOut,
    EaseInOut,

    ExponentialIn,
    ExponentialOut,
    ExponentialInOut,

    SineIn,
    SineOut,
    SineInOut,

    ElasticIn,
    ElasticOut,
    ElasticInOut,

    BounceIn,
    BounceOut,
    BounceInOut,

    BackIn,
    BackOut,
    BackInOut,
};

enum class EasingDirection {
    In, Out, InOut
};

struct TweenOptions {

    // Required
    float from;
    float to;
    float duration;
    EasingType easingType;
    Callback<float> onUpdate;

    // Optional
    Callback<> onComplete;
    float rate = 2.0f; // for Ease functions
    float period = 0.3f; // for Elastic functions
};

class Tween {
public:
    static std::unique_ptr<Tween> create(const TweenOptions& opt);

    void update(float dt);
    bool isFinished();
    bool init(const TweenOptions& opt);

    /*
        Set a delay in seconds for this tween. This should always be called
        before `start()`, otherwise the delay will not be applied.
    */
    Tween& setStartDelay(float delaySeconds);

    /*
        Enable yo-yo mode. The tween will reverse before calling `onComplete()` or repeating.
        This is a no-op when duration is 0.
    */
    Tween& enableYoyo();

    /*
        Disable yo-yo mode. Future loops will restart the tween's progress instead of reversing.
    */
    Tween& disableYoyo();

    /*
        Set the number of times this tween should repeat before calling `onComplete()`.
        This should always be called before `start()`, otherwise the new repeat count
        will not be applied. Cancels infinite looping if `repeatForever()` was called earlier.
        You cannot change the repeat count if the tween is already running.
        This is a no-op when duration is 0.
    */
    Tween& setRepeatCount(int repeatCount);

    /*
        Sets the repeat count to infinity. The tween will loop until stopped by `stopRepeating()`.
        This overrides `setRepeatCount()` if called after it.
        This is a no-op when duration is 0.
    */
    Tween& repeatForever();

    /*
        Cancels looping entirely. This applies to both finite and infinite repeat count.
        If called during an active tween, scheduled loops in the future will be killed,
        and `onComplete()` is called once the tween finishes.
    */
    Tween& stopRepeating();

    /*
        Resets the tween's state and starts it up again instantly. This is an alternative to doing `->stop()->start();`.
    */
    Tween& restart();

    /*
        Pause the currently active tween.
    */   
    Tween& pause();

    /*
        Resume the currently active tween.
    */
    Tween& resume();

    /*
        Begin the tween. This should be called after applying repeat count or start delay.
    */
	Tween& start();

    /*
        Stop the currently active tween and reset it to the beginning.
    */
	Tween& stop();

    /*
        Get the progress of a single tween in a 0-1 range.
        If repeat is enabled, the progress returned will
        jump back to 0 on each cycle. With yo-yo mode,
        the forward motion and the backward motion will be treated
        as two separate blocks of progress (0 -> 1, 0 -> 1).
    */
    float getCycleProgress() const;

    /*
        Get the tween's lifetime progress in a 0-1 range.
        Returns 0 if infinite looping is enabled, unless
        the duration is 0, then it returns 1.
    */
    float getTotalProgress() const;

    /*
        Get the duration of a single cycle in seconds. This time does not include
        the reverse if yo-yo mode is enabled.
    */
	float getDuration() const;

    /*
        Returns true if the tween is active.
    */
	bool isRunning() const;

    // Tween() = default;
    // Tween(const Tween&) = delete;
    // Tween& operator=(const Tween&) = delete;
    // Tween(Tween&&) = delete;
    // Tween& operator=(Tween&&) = delete;   
private:

    // Fixed config
    TweenOptions opt_;
    std::function<float(float)> easeFn_ = nullptr;

    // Mutable config
    float startDelay_ = 0.0f;
    bool yoyoMode_ = false;
	bool repeatForever_ = false;
	int repeatsLeft_ = 0;
	int totalRepeats_ = 0;
    float period_ = 0.3f; // for Elastic functions

    // Runtime state
    float elapsedInCycle_ = 0.0f;
	float startDelayRemaining_ = 0.0f;
    bool reversed_ = false;
	bool running_ = false;
    bool paused_ = false;
    bool finished_ = false;
    bool stopRepeatingRequested_ = false;
    int cyclesCompleted_ = 0;
};

}