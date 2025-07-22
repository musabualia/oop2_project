#include "Core/Timer.h"
#include <algorithm>

Timer::Timer() : m_startTime(Clock::now()) {}

Timer::Timer(float duration)
    : m_startTime(Clock::now()), m_duration(duration) {
}

void Timer::update(float dt) {
    // Apply time scaling to delta time
    dt *= m_timeScale;

    // Check and trigger callbacks based on progress
    checkAndTriggerCallbacks();

    // Handle completion and looping
    if (isElapsed()) {
        handleCompletion();
    }
}

void Timer::reset() {
    m_startTime = Clock::now();
    m_pausedTime = Duration{ 0.0f };
    m_isPaused = false;
    resetCallbackTriggers();
}

void Timer::restart() {
    reset();
    m_isRunning = true;
}

void Timer::pause() {
    if (!m_isPaused && m_isRunning) {
        m_isPaused = true;
        m_pauseStart = Clock::now();
    }
}

void Timer::resume() {
    if (m_isPaused) {
        m_pausedTime += Clock::now() - m_pauseStart;
        m_isPaused = false;
    }
}

bool Timer::isElapsed() const {
    return m_duration.count() > 0.0f && getCurrentElapsed() >= m_duration.count();
}

bool Timer::isElapsed(float duration) const {
    return getCurrentElapsed() >= duration;
}

float Timer::getElapsed() const {
    return getCurrentElapsed();
}

float Timer::getRemaining() const {
    if (m_duration.count() <= 0.0f) return 0.0f;
    return std::max(0.0f, m_duration.count() - getCurrentElapsed());
}

float Timer::getDuration() const {
    return m_duration.count();
}

float Timer::getProgress() const {
    if (m_duration.count() <= 0.0f) return 1.0f;
    return std::min(1.0f, getCurrentElapsed() / m_duration.count());
}

void Timer::setDuration(float duration) {
    m_duration = Duration{ duration };
    resetCallbackTriggers(); // Reset triggers when duration changes
}

void Timer::setElapsed(float elapsed) {
    auto elapsedDuration = Duration{ elapsed };
    auto totalOffset = std::chrono::duration_cast<Clock::duration>(elapsedDuration + m_pausedTime);
    m_startTime = Clock::now() - totalOffset;
    resetCallbackTriggers(); // Reset triggers when elapsed time changes
}

bool Timer::isRunning() const {
    return m_isRunning;
}

bool Timer::isPaused() const {
    return m_isPaused;
}

bool Timer::isFinished() const {
    return isElapsed();
}

float Timer::getCurrentElapsed() const {
    if (!m_isRunning) return 0.0f;

    auto now = Clock::now();
    auto totalElapsed = now - m_startTime - m_pausedTime;

    if (m_isPaused) {
        totalElapsed -= (now - m_pauseStart);
    }

    // Apply time scale to elapsed time
    float scaledElapsed = std::chrono::duration_cast<Duration>(totalElapsed).count() * m_timeScale;
    return scaledElapsed;
}

void Timer::setLooping(bool loop) {
    m_looping = loop;
}

bool Timer::isLooping() const {
    return m_looping;
}

void Timer::setTimeScale(float scale) {
    m_timeScale = std::max(0.1f, scale); // Prevent negative or zero scaling
}

float Timer::getTimeScale() const {
    return m_timeScale;
}

// Callback setters
void Timer::setOnComplete(TimerCallback callback) {
    m_onComplete = callback;
}

void Timer::setOnHalfway(TimerCallback callback) {
    m_onHalfway = callback;
}

void Timer::setOnQuarter(TimerCallback callback) {
    m_onQuarter = callback;
}

void Timer::setOnThreeQuarter(TimerCallback callback) {
    m_onThreeQuarter = callback;
}

void Timer::setOnProgressReached(float progress, TimerCallback callback) {
    ProgressCallback pc;
    pc.targetProgress = std::clamp(progress, 0.0f, 1.0f);
    pc.callback = callback;
    pc.triggered = false;
    m_progressCallbacks.push_back(pc);
}

void Timer::clearCallbacks() {
    m_onComplete = nullptr;
    m_onHalfway = nullptr;
    m_onQuarter = nullptr;
    m_onThreeQuarter = nullptr;
    m_progressCallbacks.clear();
    resetCallbackTriggers();
}

// Utility methods
void Timer::addTime(float seconds) {
    m_duration = Duration{ m_duration.count() + seconds };
}

void Timer::subtractTime(float seconds) {
    float newDuration = std::max(0.0f, m_duration.count() - seconds);
    m_duration = Duration{ newDuration };
}

bool Timer::isNearCompletion(float threshold) const {
    return getProgress() >= threshold;
}

void Timer::checkAndTriggerCallbacks() {
    if (!m_isRunning || m_isPaused || m_duration.count() <= 0.0f) return;

    float progress = getProgress();

    // Check quarter progress (25%)
    if (!m_quarterTriggered && progress >= 0.25f && m_onQuarter) {
        m_quarterTriggered = true;
        m_onQuarter();
    }

    // Check halfway progress (50%)
    if (!m_halfwayTriggered && progress >= 0.5f && m_onHalfway) {
        m_halfwayTriggered = true;
        m_onHalfway();
    }

    // Check three-quarter progress (75%)
    if (!m_threeQuarterTriggered && progress >= 0.75f && m_onThreeQuarter) {
        m_threeQuarterTriggered = true;
        m_onThreeQuarter();
    }

    // Check custom progress callbacks
    for (auto& pc : m_progressCallbacks) {
        if (!pc.triggered && progress >= pc.targetProgress && pc.callback) {
            pc.triggered = true;
            pc.callback();
        }
    }
}

void Timer::resetCallbackTriggers() {
    m_quarterTriggered = false;
    m_halfwayTriggered = false;
    m_threeQuarterTriggered = false;

    // Reset custom progress callback triggers
    for (auto& pc : m_progressCallbacks) {
        pc.triggered = false;
    }
}

void Timer::handleCompletion() {
    // Trigger completion callback
    if (m_onComplete) {
        m_onComplete();
    }

    // Handle looping
    if (m_looping) {
        restart();
    }
}