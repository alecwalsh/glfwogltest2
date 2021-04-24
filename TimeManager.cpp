#include "TimeManager.h"

#include <GLFW/glfw3.h>

#include <stdexcept>

#include <cmath>

double TimeManager::GetFPS() {
    return std::round(1 / deltaTime);
}

void TimeManager::Tick() {
    frameCount++;

    double newTime = glfwGetTime();
    if (newTime == 0) throw std::runtime_error{"Error in glfwGetTime"};

    deltaTime = newTime - elapsedTime;
    elapsedTime = newTime;

    for (auto it = timers.begin(); it != timers.end();) {
        auto& [type, millis, f, hits] = *it;

        if (std::chrono::duration<double>{elapsedTime} > (hits + 1) * millis) {
            f();
            hits++;

            if (type == Timer::Type::Oneshot) {
                it = timers.erase(it);
                continue;
            }
        }

        ++it;
    }
}

TimeManager& TimeManager::GetInstance() {
    static TimeManager tm{};
    return tm;
}