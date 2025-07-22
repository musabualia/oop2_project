#pragma once
#include <chrono>
#include <functional>

class Timer {
public:
    using TimerCallback = std::function<void()>;            // Callback function type

    Timer();                                                // Default constructor
    explicit Timer(float duration);                         // Initialize with duration

    void update(float dt);                                  // Update timer with delta time
    void reset();                                           // Reset timer to zero
    void restart();                                         // Restart timer
    void pause();                                           // Pause timer
    void resume();                                          // Resume timer

    bool isElapsed() const;                                 // Check if timer elapsed
    bool isElapsed(float duration) const;                   // Check if custom duration elapsed
    float getElapsed() const;                               // Get elapsed time
    float getRemaining() const;                             // Get remaining time
    float getDuration() const;                              // Get timer duration
    float getProgress() const;                              // Get progress (0.0 to 1.0)

    void setDuration(float duration);                       // Set timer duration
    void setElapsed(float elapsed);                         // Set elapsed time

    bool isRunning() const;                                 // Check if timer is running
    bool isPaused() const;                                  // Check if timer is paused
    bool isFinished() const;                                // Check if timer finished

    void setLooping(bool loop);                             // Enable/disable looping
    bool isLooping() const;                                 // Check if timer is looping

    void setTimeScale(float scale);                         // Set time scale factor
    float getTimeScale() const;                             // Get time scale

    void setOnComplete(TimerCallback callback);             // Set completion callback
    void setOnHalfway(TimerCallback callback);              // Set halfway callback
    void setOnQuarter(TimerCallback callback);              // Set quarter callback (25%)
    void setOnThreeQuarter(TimerCallback callback);         // Set three-quarter callback (75%)

    void setOnProgressReached(float progress, TimerCallback callback); // Set custom progress callback
    void clearCallbacks();                                  // Clear all callbacks

    void addTime(float seconds);                            // Add time to timer
    void subtractTime(float seconds);                       // Subtract time from timer
    bool isNearCompletion(float threshold = 0.9f) const;    // Check if near completion

private:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<float>;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint m_startTime;                                  // Timer start time
    Duration m_duration{ 0.0f };                            // Timer duration
    Duration m_pausedTime{ 0.0f };                          // Total paused time
    TimePoint m_pauseStart;                                 // Time when paused started
    bool m_isRunning = true;                                // Running state
    bool m_isPaused = false;                                // Paused state
    bool m_looping = false;                                 // Looping flag
    float m_timeScale = 1.0f;                               // Time scaling factor

    TimerCallback m_onComplete;                             // Complete callback
    TimerCallback m_onHalfway;                              // Halfway callback
    TimerCallback m_onQuarter;                              // Quarter callback
    TimerCallback m_onThreeQuarter;                         // Three-quarter callback

    struct ProgressCallback {                               // Custom progress callback
        float targetProgress;
        TimerCallback callback;
        bool triggered = false;
    };
    std::vector<ProgressCallback> m_progressCallbacks;      // Progress-based callbacks

    bool m_quarterTriggered = false;                        // Quarter callback triggered
    bool m_halfwayTriggered = false;                        // Halfway callback triggered
    bool m_threeQuarterTriggered = false;                   // Three-quarter callback triggered

    float getCurrentElapsed() const;                        // Get current elapsed time
    void checkAndTriggerCallbacks();                        // Check and trigger callbacks
    void resetCallbackTriggers();                           // Reset triggered states
    void handleCompletion();                                // Handle completion logic
};
