#pragma once
#include <cstdint>
#include <chrono>
#include <functional>
#include <utility>
#include <list>

class Timer {
  public:
    enum class Type : std::uint8_t {
        Oneshot,
        Repeat,
    };

    Type type;
    std::chrono::milliseconds millis;
    std::function<void()> f;
    unsigned int hits = 0;
};

class TimeManager {
  public:
    unsigned int frameCount = 0;
    double elapsedTime = 0;
    double deltaTime = 0;

    double GetFPS();
    void Tick(); // Called every frame

    template <typename F> void AddTimer(Timer::Type type, std::chrono::milliseconds ms, F&& f) {
// TODO: Remove #if when switching to C++20
#if __cpp_aggregate_paren_init >= 201902L
        timers.emplace_back(type, ms, std::forward<F>(f));
#else
        timers.push_back(Timer{type, ms, std::forward<F>(f)});
#endif // __cpp_aggregate_paren_init >= 201902L
    }

    static TimeManager& GetInstance();
    // Deleted to prevent copies
    TimeManager(const TimeManager&) = delete;
    void operator=(const TimeManager&) = delete;

  private:
    TimeManager() = default;
    ~TimeManager() = default;

    std::list<Timer> timers;
};