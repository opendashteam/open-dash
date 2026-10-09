#pragma once

#include "../core/types.h"
#include "../core/macros.h"

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

class Tween {
public:
    CREATE_FUNC(Tween)

    void update(float dt);
    bool isFinished();
    bool init();

    Tween& from(float from);
    Tween& to(float to);
    Tween& duration(float duration);
    Tween& type(const EasingType& type);
    Tween& onUpdate(Callback<float> callback);
    Tween& onComplete(Callback<> callback);
    Tween& rate(float rate); // for Ease functions
    Tween& period(float period); // for Elastic functions
    Tween& deleteSelf(); // (Runs AFTER onComplete()) Delete the tween from Director once completed

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
        Resets the tween's state and starts it up again instantly. This is an alternative to doing `stop()` followed by `start()`.
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
private:

    // Fixed config
    float from_ = 0.0f;
    float to_ = 0.0f;
    float duration_ = 0.0f;
    EasingType easingType_;
    Callback<float> onUpdate_;
    Callback<> onComplete_ = nullptr;
    float rate_ = 2.0f;
    float period_ = 0.3f;
    bool deleteSelf_ = false;
    std::function<float(float)> easeFn_ = nullptr;

    // Mutable config
    float startDelay_ = 0.0f;
    bool yoyoMode_ = false;
	bool repeatForever_ = false;
	int repeatsLeft_ = 0;
	int totalRepeats_ = 0;

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