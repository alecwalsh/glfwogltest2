#include "TimeManager.h"

#include <GLFW/glfw3.h>

#include <cmath>

double TimeManager::GetFPS() {
    return std::round(1 / deltaTime);
}

void TimeManager::Tick() {
    frameCount++;
    auto newTime = glfwGetTime();
    deltaTime = newTime - elapsedTime;
    elapsedTime = newTime;
}

TimeManager& TimeManager::GetInstance() {
    static TimeManager tm{};
    return tm;
}